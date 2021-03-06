﻿using System;
using System.Collections;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace RUDP
{
    public class ReliableUDP
    {
        enum State
        {
            NONE,           // 未接続.
            CLOSED,         // 終了.
            SYN_SENT,       // 接続要求送信済み.
            SYN_RECV,       // 接続要求受信済み.
            ESTABLISHED,    // 通信確立.
            WILL_CLOSE,     // 終了処理中.
        }

        /// <summary>
        /// 最大シーケンス番号.
        /// セグメントヘッダ内のシーケンス番号は 1byte(=8bit) なので、最大は 255(= 2^8 - 1) になる.
        /// </summary>
        private static int MAX_SEQUENCE_NUMBER = 255;

        /// <summary>
        /// Maximum number of sent segments.
        /// </summary>
        int SendQueueSize = 32;

        /// <summary>
        /// Maximum number of received segments
        /// </summary>
        int MaxRecvListSize = 32;

        /// <summary>
        /// UDP.
        /// </summary>
        protected UdpClient Udp
        {
            get;
            set;
        }

        /// <summary>
        /// 現在のステート.
        /// </summary>
        AtomicValue<State> CurState = new AtomicValue<State>(State.NONE);

        /// <summary>
        /// RUDPパラメータ.
        /// </summary>
        Parameter _Parameter;

        Parameter Parameter
        {
            get
            {
                if (_Parameter == null)
                {
                    _Parameter = new Parameter();
                }
                return _Parameter;
            }
            set
            {
                _Parameter = value;
            }
        }

        Counter Counter = new Counter();

        AutoResetEvent ConnectEvent = new AutoResetEvent(false);

        /// <summary>
        /// セグメント受信スレッド.
        /// </summary>
        Thread RecieveThread;

        /// <summary>
        /// タイマースレッド.
        /// </summary>
        TimerWorker RetransWorker;

        /// <summary>
        /// UnAckedSentSegmentListに余裕ができるのを待つ.
        /// </summary>
        AutoResetEvent VacantUnAckedSentSegListWait = new AutoResetEvent(false);

        /// <summary>
        /// InSequenceSegmentListにAddされるのを待つ.
        /// </summary>
        AutoResetEvent InSequenceSegWait = new AutoResetEvent(false);

        /// <summary>
        /// UnAckedSentSegmentListが空でなくなるのを待つ.
        /// </summary>
        AutoResetEvent NotEmptyUnAckedSentSegListWait = new AutoResetEvent(false);

        /// <summary>
        /// 送信したが応答が来ていないセグメントのリスト.
        /// </summary>
        SafeList<Segment> UnAckedSentSegmentList = new SafeList<Segment>();

        Object RecieveQueueLocker = new Object();

        /// <summary>
        /// シーケンス内のセグメントのリスト.
        /// </summary>
        SafeList<Segment> InSequenceSegmentList = new SafeList<Segment>();

        /// <summary>
        /// シーケンス外のセグメントのリスト.
        /// </summary>
        SortedList<int, Segment> OutSequenceSegmentList = new SortedList<int, Segment>();

        /// <summary>
        /// 受信したが確認応答を送っていないセグメントのリスト.
        /// </summary>
        SafeList<Segment> UnAckedRecievedSegmentList = new SafeList<Segment>();

        public bool IsClosed
        {
            get
            {
                //return this.CurState.Equals(State.CLOSED);
                return this.CurState == State.CLOSED;
            }
        }

        protected bool WillClose
        {
            get
            {
                return this.CurState == State.CLOSED || this.CurState == State.WILL_CLOSE;
            }
        }

        public bool IsConnected
        {
            get
            {
                return this.CurState == State.ESTABLISHED;
            }
        }

        protected ReliableUDP()
        {
        }

        /// <summary>
        /// コンストラクタ.
        /// </summary>
        /// <param name="udp"></param>
        /// <param name="parameter"></param>
        public ReliableUDP(
            UdpClient udp,
            Parameter parameter)
        {
            this.Udp = udp;
            this.Parameter = parameter;

            // セグメントの受信は別スレッドで受け取る.
            this.RecieveThread = new Thread(ProcRecieve);
            this.RecieveThread.Start();

            //this.RetransWorker = new TimerWorker(ProcRetransmit);
        }

        /// <summary>
        /// 接続.
        /// </summary>
        /// <param name="ep"></param>
        public void Connect(IPEndPoint ep)
        {
            this.Udp.Connect(ep);

            this.CurState.Store(State.SYN_SENT);

            var rand = new Random(Environment.TickCount);
            var segment = new SynchronousSegment(
                this.Counter.SetSequenceNumber(rand.Next(MAX_SEQUENCE_NUMBER)),
                this.Parameter);

            // 同期要求を送る.
            SendAndQueueSegment(segment);

            // 接続が確立されるまで待つ
            this.ConnectEvent.WaitOne();

            if (this.CurState != State.ESTABLISHED)
            {
                // TODO
                // ここから下は失敗したときの処理
            }
        }

        /// <summary>
        /// データ受信.
        /// </summary>
        /// <param name="bytes"></param>
        /// <returns></returns>
        public int Recieve(ref byte[] bytes)
        {
            if (this.IsClosed)
            {
                return 0;
            }

            if (this.InSequenceSegmentList.Count == 0)
            {
                InSequenceSegWait.WaitOne();
            }

            int totalSize = 0;

            lock (this.InSequenceSegmentList.Locker)
            {
                List<Segment> willRemoveList = new List<Segment>();

                for (int i = 0; i < this.InSequenceSegmentList.Count; i++)
                {
                    Segment segment = this.InSequenceSegmentList[i];

                    if (segment is ResetSegment)
                    {
                        willRemoveList.Add(segment);
                        break;
                    }
                    else if (segment is FinishSegment)
                    {
                        if (totalSize <= 0)
                        {
                            willRemoveList.Add(segment);
                            totalSize = -1;
                        }
                        break;
                    }
                    else if (segment is DataSegment)
                    {
                        var s = segment as DataSegment;
                        
                        if (s.Data.Length + totalSize > bytes.Length)
                        {
                            if (totalSize <= 0)
                            {
                                throw new ApplicationException();
                            }
                            break;
                        }

                        System.Array.Copy(
                            s.Data,
                            0,
                            bytes,
                            totalSize,
                            s.Data.Length);

                        totalSize += s.Data.Length;
                        willRemoveList.Add(segment);
                    }
                }

                for (int i = 0; i < willRemoveList.Count; i++)
                {
                    this.InSequenceSegmentList.Remove(willRemoveList[i]);
                }
            }

            return totalSize;
        }

        /// <summary>
        /// データ送信.
        /// </summary>
        /// <param name="bytes"></param>
        /// <param name="offset"></param>
        /// <param name="length"></param>
        /// <returns></returns>
        public int Send(byte[] bytes, int offset, int length)
        {
            if (this.IsClosed)
            {
                return 0;
            }

            int totalBytes = 0;

            while (totalBytes < length)
            {
                int sendBytes = Math.Min(
                    this.Parameter.MaxSegmentSize - Segment.RUDP_HEADER_LEN,
                    length - totalBytes);

                if (sendBytes <= 0)
                {
                    throw new ArgumentOutOfRangeException();
                }

                // NOTE
                // AcknowledgementSegment は受信したあとにハンドリングしないので
                // AcknowledgementSegment からのシーケンス番号は更新されない.
                // そのため、Listner - Client でシーケンス番号に齟齬が生じる.
                // そこで、DataSegment や FinishSegment などを送るときにシーケンス番号をリセットするように
                // LastInSequenceNumber ではなく、独自に管理しているシーケンス番号を使うようにする.

                var segment = new DataSegment(
                    this.Counter.NextSequenceNumber(),
                    this.Counter.GetLastInSequenceNumber(),
                    bytes,
                    offset,
                    length);

                SendAndQueueSegment(segment);

                totalBytes += length;
            }

            return totalBytes;
        }

        /// <summary>
        /// 終了.
        /// </summary>
        public void Close()
        {
            if (this.IsClosed)
            {
                return;
            }

            switch ((State)this.CurState)
            {
                case State.SYN_SENT:
                    // 接続待ちを起こす.
                    this.ConnectEvent.Set();
                    break;
                case State.SYN_RECV:
                case State.ESTABLISHED:
                    // NOTE
                    // AcknowledgementSegment は受信したあとにハンドリングしないので
                    // AcknowledgementSegment からのシーケンス番号は更新されない.
                    // そのため、Listner - Client でシーケンス番号に齟齬が生じる.
                    // そこで、DataSegment や FinishSegment などを送るときにシーケンス番号をリセットするように
                    // LastInSequenceNumber ではなく、独自に管理しているシーケンス番号を使うようにする.

                    var segment = new FinishSegment(this.Counter.NextSequenceNumber());
                    SendSegment(segment);

                    // TODO
                    // スレッドなどを止める.
                    // 現状は recv がブロッキングされるので、スレッドを止められない.
                    break;
                default:
                    break;
            }

            this.CurState.Store(State.CLOSED);

            this.VacantUnAckedSentSegListWait.Set();
            this.NotEmptyUnAckedSentSegListWait.Set();
            this.InSequenceSegWait.Set();
        }

        /// <summary>
        /// セグメントの送信と返事待ちセグメントキューへの登録.
        /// </summary>
        /// <param name="segment"></param>
        private void SendAndQueueSegment(Segment segment)
        {
            while (this.UnAckedSentSegmentList.Count >= SendQueueSize
                || this.Counter.GetOutStandingSegmentCount() >= this.Parameter.MaxNumberOfOutStandingSegs)
            {
                this.VacantUnAckedSentSegListWait.WaitOne();
            }

            // TODO
            // UnAckedSentSegmentListの数で代用できないか？
            // 返事待ちセグメントの数を増やす.
            this.Counter.IncrementOutStandingSegmentCount();

            // 返事のない送信済み（返事待ち）セグメントリストに登録.
            this.UnAckedSentSegmentList.Add(segment);

            // UnAckedSentSegmentListが空でなくなったので待機イベントに通知.
            this.NotEmptyUnAckedSentSegListWait.Set();

            // TODO
            // 返事のない送信済み（返事待ち）セグメントについて再送するためのスレッドを起こす.

            // セグメント送信.
            SendSegment(segment);

            // データセグメントの場合はリスナーにパケットを送信したことを通知する.
            if (segment is DataSegment)
            {
                OnSentPacket();
            }
        }

        /// <summary>
        /// セグメント送信.
        /// </summary>
        /// <param name="segment"></param>
        private void SendSegment(Segment segment)
        {
            // ACK(返事)の抱き合わせ設定
            if (segment is DataSegment
                || segment is ResetSegment
                || segment is FinishSegment
                || segment is NullSegment)
            {
                // 受信したけど返事をしていないセグメントの有無を確認.
                if (this.UnAckedRecievedSegmentList.Count > 0)
                {
                    // 返事待ちセグメントが存在すれば、ACK（返事）フラグをセグメントに設定する.
                    // どの番号に対応するACK（返事）なのかをセット.
                    segment.SetAcknowledgedNumber(this.Counter.GetLastInSequenceNumber());
                }
            }

            if (segment is DataSegment
                || segment is ResetSegment
                || segment is FinishSegment)
            {
                // TODO
                // Reset null segment timer
            }

            // 送信.
            OnSendSegment(segment);
        }

        /// <summary>
        /// セグメント受信.
        /// </summary>
        /// <returns></returns>
        private Segment RecieveSegment()
        {
            Segment segment;

            if ((segment = OnRecieveSegment()) != null)
            {
                if (segment is DataSegment
                    || segment is NullSegment
                    || segment is ResetSegment
                    || segment is FinishSegment
                    || segment is SynchronousSegment)
                {
                    // 確認応答を送っていないセグメントリストに登録.
                    this.UnAckedRecievedSegmentList.Add(segment);
                }

                // TODO
                // 受信したということは接続がまだ生きているので殺されないようにリセットする.
            }

            return segment;
        }

        /// <summary>
        /// セグメントを受信して、セグメントのタイプに応じて処理を振り分ける.
        /// </summary>
        private void ProcRecieve()
        {
            while (!this.WillClose)
            {
                var segment = RecieveSegment();

                if (segment != null)
                {
                    if (segment is SynchronousSegment)
                    {
                        HandleSYNSegment(segment as SynchronousSegment);
                    }
                    else if (segment is ExtendAckSegment)
                    {
                        HandleEAKSegment(segment as ExtendAckSegment);
                    }
                    else if (segment is AcknowledgementSegment)
                    {
                        // なにもしない.
                    }
                    else
                    {
                        HandleSegment(segment);
                    }

                    CheckAndGetAck(segment);
                }

                Thread.Yield();
            }
        }

        private void CheckAndGetAck(Segment segment)
        {
            int ackNumber = segment.GetAcknowledgedNumber();

            if (ackNumber < 0)
            {
                return;
            }

            if (this.CurState == State.SYN_RECV)
            {
                OpenConnection();

                // 接続要求を受信した返事に対する返事なので、接続を開く.
                this.CurState.Store(State.ESTABLISHED);
            }

            this.UnAckedSentSegmentList.ForeachWithRemoving(
                (Segment s) =>
                {
                    return (CompareSequenceNumbers(s.SequenceNumber, ackNumber) <= 0);
                });

            this.VacantUnAckedSentSegListWait.Set();
        }

        /// <summary>
        /// 受信した接続要求(SYNchronous)セグメントを処理する.
        /// </summary>
        /// <param name="segment"></param>
        private void HandleSYNSegment(SynchronousSegment segment)
        {
            switch (this.CurState.Load())
            {
                case State.NONE:  // 未接続.
                    this.Counter.SetLastInSequenceNumber(segment.SequenceNumber);

                    this.CurState.Store(State.SYN_RECV);

                    // NOTE
                    // State.NONE 状態で SynchronousSegment を受信するのは Listener なので
                    // SynchronousSegment を送信し返すタイミングでシーケンス番号を新規設定する？
                    // でも、もらった番号をそのまま使えばいい気もするが・・・

                    var rand = new Random(Environment.TickCount);
                    var syncSegment = new SynchronousSegment(
                        this.Counter.SetSequenceNumber(rand.Next(MAX_SEQUENCE_NUMBER)),
                        this.Parameter == null ? new Parameter() : this.Parameter);

                    syncSegment.SetAcknowledgedNumber(segment.SequenceNumber);

                    SendAndQueueSegment(syncSegment);

                    this.UnAckedRecievedSegmentList.Remove(segment);

                    break;
                case State.SYN_SENT:    // 接続要求送信済み.
                    this.Counter.SetLastInSequenceNumber(segment.SequenceNumber);

                    // 返事を返す.
                    SendAcknowledgement(segment);

                    // 接続確立.
                    OpenConnection();

                    break;
            }
        }

        private void HandleEAKSegment(ExtendAckSegment segment)
        {
            var ackNumbers = segment.AckNumbers;

            int lastInSequence = segment.GetAcknowledgedNumber();
            int lastOutSequence = ackNumbers[ackNumbers.Length - 1];

            // 送信したけど応答がないセグメントリストに対象となるものがないか調べる.
            lock (this.UnAckedSentSegmentList)
            {
                List<Segment> willRemoveList = new List<Segment>();

                for (int i = 0; i < this.UnAckedSentSegmentList.Count; i++)
                {
                    var s = this.UnAckedSentSegmentList[i];

                    if (CompareSequenceNumbers(s.SequenceNumber, lastInSequence) <= 0)
                    {
                        // シーケンス内に入っているので、どこかで応答を受けたはず.
                        willRemoveList.Add(s);
                        continue;
                    }

                    for (int n = 0; n < ackNumbers.Length; n++)
                    {
                        if (CompareSequenceNumbers(s.SequenceNumber, ackNumbers[n]) == 0)
                        {
                            willRemoveList.Add(s);
                            break;
                        }
                    }
                }

                foreach (var s in willRemoveList)
                {
                    this.UnAckedSentSegmentList.Remove(s);
                }

                // 残ったのものは再送する.
                for (int i = 0; i < this.UnAckedSentSegmentList.Count; i++)
                {
                    var s = this.UnAckedSentSegmentList[i];

                    if (CompareSequenceNumbers(lastInSequence, s.SequenceNumber) < 0
                        && CompareSequenceNumbers(lastOutSequence, s.SequenceNumber) > 0)
                    {
                        // 再送.
                        RetransmitSegment(s);
                    }
                }
            }
        }

        /// <summary>
        /// 受信したセグメントを処理.
        /// </summary>
        /// <param name="segment"></param>
        private void HandleSegment(Segment segment)
        {
            // リセットセグメントがきたので通信をリセットする.
            if (segment is ResetSegment)
            {
                // TODO
            }

            // これ以上なにも送られてこないことが通知されてきた.
            if (segment is FinishSegment)
            {
                switch((State)this.CurState)
                {
                    case State.SYN_SENT:
                        // 同期セグメント送信状態の場合は、返事待ちしているところ（connect）を解除して先に進むようにする.
                        this.ConnectEvent.Set();
                        break;
                    case State.CLOSED:
                        // すでに終了しているので何もしない.
                        break;
                    default:
                        this.CurState.Store(State.WILL_CLOSE);
                        break;
                }
            }

            // シーケンス番号が正しいかどうか.
            bool isInSequence = false;

            lock (this.RecieveQueueLocker)
            {
                // 期待するシーケンス番号.
                var nextSeqNumber = NextSequenceNumber(Counter.GetLastInSequenceNumber());

                if (CompareSequenceNumbers(segment.SequenceNumber, Counter.GetLastInSequenceNumber()) >= 0)
                {
                    // 送信したものと同じもの or 番号が小さいものがきた(すでに届いているものがきた)ので無視する.
                }
                else if (CompareSequenceNumbers(segment.SequenceNumber, nextSeqNumber) == 0)
                {
                    // 期待する番号がきた.
                    isInSequence = true;

                    if (this.InSequenceSegmentList.Count + this.OutSequenceSegmentList.Count < MaxRecvListSize)
                    {
                        // キューに余裕がある.

                        // シーケンス番号を更新.
                        this.Counter.SetLastInSequenceNumber(segment.SequenceNumber);

                        if (segment is DataSegment
                            || segment is ResetSegment
                            || segment is FinishSegment)
                        {
                            // read で読み込むセグメントはリストに登録.
                            this.InSequenceSegmentList.Add(segment);
                            InSequenceSegWait.Set();
                        }

#if false
                        // 順番通りにデータが来たことを通知.
                        if (segment is DataSegment)
                        {
                            OnRecivedPacketInOrder(segment as DataSegment);
                        }
#endif

                        // シーケンス番号が変わったので、out-of-sequenceリストから正しい順序のものを InSequenceSegmentQueue に移す.
                        UpdateOutSequeceQueue();
                    }
                    else
                    {
                        // リストがいっぱいなので、何もしない.
                    }
                }
                else
                {
                    // シーケンス番号が順番がずれた（パケット順序がおかしくなった）ので、out-of-sequenceリストに登録する.

                    if (this.InSequenceSegmentList.Count + this.OutSequenceSegmentList.Count < MaxRecvListSize)
                    {
                        bool added = false;

                        for (int i = 0; i < this.OutSequenceSegmentList.Count; i++)
                        {
                            var s = this.OutSequenceSegmentList.Values[i];

                            if (CompareSequenceNumbers(segment.SequenceNumber, s.SequenceNumber) == 0)
                            {
                                // 登録済み.
                                added = true;
                                break;
                            }
                        }

                        if (!added)
                        {
                            // 未登録なので、登録する.
                            this.OutSequenceSegmentList.Add(segment.SequenceNumber, segment);
                        }

#if false
                        // 順序がずれたパケットを受けたことを通知する.
                        if (segment is DataSegment)
                        {
                            OnRecivedPacketOutOfOrder(segment as DataSegment);
                        }
#endif
                    }
                }
                
                if (isInSequence && (segment is DataSegment
                                    || segment is NullSegment
                                    || segment is FinishSegment)
                    )
                {
                    // 受信した応答を返す.
                    SendAcknowledgement(segment);
                }
                else if (this.OutSequenceSegmentList.Count > this.Parameter.MaxOutOfSequence)
                //else if (this.OutSequenceSegmentList.Count > 0)
                {
                    // out-of-sequenceセグメントの数が許容最大値を超えた.
                    SendExtendAcknowledgement();
                }
                else if (this.UnAckedRecievedSegmentList.Count > this.Parameter.MaxCumulativeAck)
                {
                    // 受信したけど確認応答を送っていないセグメントの数が許容最大値を超えた.
                    SendSingleAcknowledgement(segment);
                }
                else
                {
                    // TODO
                    // 上記以外の場合は、定期的にACKセグメント送る.
                }
            }
        }

        /// <summary>
        /// シーケンス番号をチェックして、out-of-sequenceリストから正しい順序のものを InSequenceSegmentQueue に移す.
        /// </summary>
        private void UpdateOutSequeceQueue()
        {
            lock(this.RecieveQueueLocker)
            {
                // 削除リスト.
                List<int> willRemoveList = new List<int>();

                for (int i = 0; i < this.OutSequenceSegmentList.Count; i++)
                {
                    var segment = this.OutSequenceSegmentList.Values[i];

                    // 期待するシーケンス番号.
                    var nextSequenceNumber = NextSequenceNumber(this.Counter.GetLastInSequenceNumber());

                    // シーケンス番号が更新されたことで、正しい番号になる可能性があるものを探す.
                    if (CompareSequenceNumbers(segment.SequenceNumber, nextSequenceNumber) == 0)
                    {
                        // シーケンス番号を更新
                        this.Counter.SetLastInSequenceNumber(segment.SequenceNumber);

                        if (segment is DataSegment
                            || segment is ResetSegment
                            || segment is FinishSegment)
                        {
                            // シーケンス処理内セグメントリストに登録.
                            this.InSequenceSegmentList.Add(segment);

                            this.InSequenceSegWait.Set();
                        }

                        // 削除リストに登録.
                        willRemoveList.Add(this.OutSequenceSegmentList.Keys[i]);
                    }
                }

                // out-of-sequenceリストから移動したものを削除.
                foreach(var key in willRemoveList)
                {
                    var segment = this.OutSequenceSegmentList[key];

                    // 返事をする
                    SendSingleAcknowledgement(segment);

                    this.OutSequenceSegmentList.Remove(key);
                }
            }
        }

        /// <summary>
        /// シーケンス番号を比較.
        /// </summary>
        /// <param name="s1"></param>
        /// <param name="s2"></param>
        /// <returns>等しければ 0, s1<s2 なら 1, それ以外は -1 を返す. </returns>
        private static int CompareSequenceNumbers(int s1, int s2)
        {
            // NOTE
            // RFC1982
            // http://www5d.biglobe.ne.jp/stssk/rfc/rfc1982j.html

            // シーケンス番号に加算できる数(n)は,
            //  0 <= n <= 2^(BITS - 1) - 1
            // でないといけない.
            // たとえば、BITS = 8 なら、
            //  0 <= n <= 2^7 - 1 (= 127)
            // つまり、n は最大シーケンス番号(2^BITS)の半分から 1 マイナスした値になる.

            //  s1 + n = s2 <-> s2 - s1 = n
            //  0 <= s2 - s1 <= 2^(BITS - 1) - 1
            //  <-> 0 <= s2 - s1 < 2^(BITS - 1)
            // たとえば、BITS = 8 なら,
            //  0 <= s2 - s1 <= 127 <-> 0 <= s2 - s1 < 128
            // つまり、シーケンス番号の差分は最大シーケンス番号(2^BITS)の半分未満でないといけない.

            // たとえば,
            //  s1 + n = s2 -> 200 + 127 = 327
            // ということはありえる.
            // しかし、BITS=8 とすると、s' の値は最大値(255)を超えているため 327 ではなく、71(=327 - 256)となる.
            // この場合だと、
            //  s2 - s1 = 71 - 200 = -129
            // となり、マイナスになってしまう.
            // そのため,
            //  s1 > s2
            // のチェックが必要.また、値は 255 を超えて一周しているので、その差分は,
            //  s1 - s2 > 2^(BITS - 1)
            // となる.
            // たとえば,
            //   s1 +  n  =  s2
            //  128 + 127 = 255 -> s1 < s2
            //  129 + 127 = 256 = 0 -> s1 > s2 -> s1 - s2 = 129 - 0 = 129 > 128 (= 2^7)

            // s１がs２より小さいとは、ｓ１とｓ２が異なりかつ以下が満たされる場合とします.
            // (i1 < i2 and i2 - i1 < 2^(SERIAL_BITS - 1)) or (i1 > i2 and i1 - i2 > 2^(SERIAL_BITS - 1))

            // s１がs２より大きいとは、ｓ１とｓ２が異なりかつ以下が満たされる場合とします.
            // (i1 < i2 and i2 - i1 > 2^(SERIAL_BITS - 1)) or (i1 > i2 and i1 - i2 < 2^(SERIAL_BITS - 1))

            if (s1 == s2)
            {
                return 0;
            }
            else if ((s1 < s2 && (s2 - s1) < MAX_SEQUENCE_NUMBER / 2)
                || (s1 > s2 && (s1 - s2) > MAX_SEQUENCE_NUMBER / 2))
            {
                // less than
                return 1;
            }
            else
            {
                // "greater than" or "invalid".
                return -1;
            }
        }

        /// <summary>
        /// 次のシーケンス番号を取得する.
        /// </summary>
        /// <param name="sequenceNumber"></param>
        /// <returns></returns>
        private static int NextSequenceNumber(int sequenceNumber)
        {
            return (sequenceNumber + 1) % MAX_SEQUENCE_NUMBER;
        }

        /// <summary>
        /// 受信した返事を返す.
        /// </summary>
        /// <param name="segment">返事をする対象のセグメント</param>
        private void SendAcknowledgement(Segment segment)
        {
            lock(this.RecieveQueueLocker)
            {
                // 受信したけど out-of-sequence 状態のセグメントがある場合.
                if (this.OutSequenceSegmentList.Count > 0)
                {
                    SendExtendAcknowledgement();
                    return;
                }

                SendSingleAcknowledgement(segment);
            }
        }

        /// <summary>
        /// out-of-sequenceセグメントがあったことについて返事をする.
        /// </summary>
        private void SendExtendAcknowledgement()
        {
            lock (this.RecieveQueueLocker)
            {
                // 受信したけど out-of-sequence 状態のセグメントが無いのに呼ばれた.
                if (this.OutSequenceSegmentList.Count == 0)
                {
                    return;
                }

                // out-of-sequenceセグメントのシーケンス番号を集める.
                int[] ackNumbers = new int[this.OutSequenceSegmentList.Count];

                for (int i = 0; i < ackNumbers.Length; i++)
                {
                    var seqNumber = this.OutSequenceSegmentList.Keys[i];
                    var segment = this.OutSequenceSegmentList.Values[i];

                    // シーケンス番号チェック
                    if (segment.SequenceNumber != seqNumber)
                    {
                        throw new ApplicationException();
                    }

                    ackNumbers[i] = segment.SequenceNumber;

                    // 受信したけど返信していないセグメントをリストから削除.
                    this.UnAckedRecievedSegmentList.Remove(segment);
                }

                // out-of-sequenceセグメントリストをリセット.
                this.OutSequenceSegmentList.Clear();

                int lastInSequenceNumber = this.Counter.GetLastInSequenceNumber();

                var sendSegment = new ExtendAckSegment(
                    NextSequenceNumber(lastInSequenceNumber),
                    lastInSequenceNumber,
                    ackNumbers);

                // 送信.
                SendSegment(sendSegment);
            }
        }

        /// <summary>
        /// 受信した返事を返す.
        /// </summary>
        /// <param name="segment">返事をする対象のセグメント</param>
        private void SendSingleAcknowledgement(Segment segment)
        {
            // 受信したけど返信していないセグメントの数が 0 のときは何もしない.
            if (this.UnAckedRecievedSegmentList.Count == 0)
            {
                return;
            }

#if false
            var lastInSequence = this.Counter.GetLastInSequenceNumber();

            var sendSegment = new AcknowledgementSegment(
                NextSequenceNumber(lastInSequence),
                lastInSequence);
#else
            var sendSegment = new AcknowledgementSegment(
                NextSequenceNumber(segment.SequenceNumber),
                segment.SequenceNumber);
#endif

            // 送信.
            SendSegment(sendSegment);

            if (segment != null)
            {
                // NOTE
                // 順番が入れ替わった場合もあるのでここでシーケンス番号チェックはできない

                // 受信したが確認応答を送っていないセグメントのリストから削除.
                this.UnAckedRecievedSegmentList.Remove(segment);
            }
        }

        /// <summary>
        /// 接続確立.
        /// </summary>
        private void OpenConnection()
        {
            if (this.CurState == State.ESTABLISHED)
            {
                // TODO
                // すでに接続確立されている場合.
            }
            else
            {
                this.CurState.Store(State.ESTABLISHED);

                this.ConnectEvent.Set();

                OnOpenConnection();
            }
        }

        /// <summary>
        /// セグメントを再送.
        /// </summary>
        /// <param name="segment"></param>
        private void RetransmitSegment(Segment segment)
        {
            // 再送回数を増やす.
            segment.RetryCount++;

            if (this.Parameter.MaxRetransmission != 0
                && segment.RetryCount > this.Parameter.MaxRetransmission)
            {
                // TODO
                // 最大再送回数を超えた場合は接続が失敗していることにする.
                return;
            }

            Console.Write("RetransmitSegment --- ");
            SendSegment(segment);
        }

        private void ProcRetransmit()
        {
            if (this.UnAckedRecievedSegmentList.IsEmpty())
            {
                this.NotEmptyUnAckedSentSegListWait.WaitOne();
                this.NotEmptyUnAckedSentSegListWait.Reset();
            }
            
            // 返事を受けていないセグメントを再送.
            this.UnAckedSentSegmentList.Foreach(this.RetransmitSegment);
        }

        /// <summary>
        /// セグメント受信実装.
        /// </summary>
        /// <returns></returns>
        protected virtual Segment OnRecieveSegment()
        {
            IPEndPoint remoteEp = new IPEndPoint(IPAddress.Any, 0);

            var ret = OnRecieveSegment(ref remoteEp);
            return ret;
        }

        /// <summary>
        /// セグメント受信実装.
        /// </summary>
        /// <param name="remote"></param>
        /// <returns></returns>
        protected virtual Segment OnRecieveSegment(ref IPEndPoint remote)
        {
            var data = this.Udp.Receive(ref remote);
            var segment = Segment.Parse(data, 0, data.Length);

            Console.WriteLine("Recieve Segment [{0}]", segment.ToString());

            return segment;
        }

#if false
        static bool isValid = false;

        /// <summary>
        /// セグメント送信実装.
        /// </summary>
        /// <param name="segment"></param>
        protected virtual void OnSendSegment(Segment segment)
        {
            // 疑似パケットロス発生

            if (isValid)
            {
                using (var ws = new MemoryStream())
                {
                    segment.WriteBytes(ws);
                    var bytes = ws.GetBuffer();
                    var length = ws.Position;
                    this.Udp.Send(bytes, (int)length);
                }
            }
            else
            {
                // TODO
                isValid = true;
            }
        }
#elif false
        Queue<Segment> TempQueue = new Queue<Segment>();
        static bool isValid = false;

        /// <summary>
        /// セグメント送信実装.
        /// </summary>
        /// <param name="segment"></param>
        protected virtual void OnSendSegment(Segment segment)
        {
            // 疑似パケット順番入れ替え

            if (!this.IsConnected || segment is AcknowledgementSegment)
            {
                using (var ws = new MemoryStream())
                {
                    Console.WriteLine("Send Segment [{0}]", segment.ToString());
                    segment.WriteBytes(ws);
                    var bytes = ws.GetBuffer();
                    var length = ws.Position;
                    this.Udp.Send(bytes, (int)length);
                }
            }
            else
            {
                if (isValid)
                {
                    using (var ws = new MemoryStream())
                    {
                        {
                            Console.WriteLine("Send Segment [{0}]", segment.ToString());
                            segment.WriteBytes(ws);
                            var bytes = ws.GetBuffer();
                            var length = ws.Position;
                            this.Udp.Send(bytes, (int)length);
                        }
                    }

                    using (var ws = new MemoryStream())
                    {
                        if (TempQueue.Count > 0)
                        {
                            var s = TempQueue.Dequeue();
                            Console.WriteLine("Send Segment [{0}]", s.ToString());
                            s.WriteBytes(ws);
                            var bytes = ws.GetBuffer();
                            var length = ws.Position;
                            this.Udp.Send(bytes, (int)length);
                        }
                    }
                }
                else
                {
                    isValid = true;
                    TempQueue.Enqueue(segment);
                }
            }
        }
#else
        /// <summary>
        /// セグメント送信実装.
        /// </summary>
        /// <param name="segment"></param>
        protected virtual void OnSendSegment(Segment segment)
        {
            Console.WriteLine("Send Segment [{0}]", segment.ToString());

            using (var ws = new MemoryStream())
            {
                segment.WriteBytes(ws);
                var bytes = ws.GetBuffer();
                var length = ws.Position;
                this.Udp.Send(bytes, (int)length);
            }
        }
#endif

        protected virtual void OnOpenConnection()
        {
            // 継承先で実装.
        }

        protected virtual void OnSentPacket()
        {
            // 継承先で実装.
        }
    }
}
