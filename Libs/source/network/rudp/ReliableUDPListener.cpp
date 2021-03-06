﻿#include "network/rudp/ReliableUDPListener.h"
#include "network/rudp/segment/Segment.h"
#include "network/NetworkUDP_libuv.h"
#include "network/IPv4Endpoint.h"

namespace izanagi {
namespace net {
    void ReliableUDPListener::Init(
        IMemoryAllocator* allocator,
        Udp* udp)
    {
        if (m_Udp == nullptr) {
            m_allocator = allocator;
            m_Udp = udp;

            m_RecieveThread = std::thread(std::bind(&ReliableUDPListener::ProcRecieve, this));
        }
    }

    ReliableUDP* ReliableUDPListener::Accept()
    {
        if (IsClosed())
        {
            IZ_ASSERT(IZ_FALSE);
            return nullptr;
        }

        std::lock_guard<std::mutex> locker(m_AcceptedClientListWaiter);

        if (m_AcceptedClientList.GetItemNum() == 0)
        {
            return nullptr;
        }

        auto item = m_AcceptedClientList.GetTop();
        auto ret = item->GetData();
        
        item->Leave();

        return ret;
    }

	// 終了.
	void ReliableUDPListener::Close()
	{
		if (IsClosed()) {
			return;
		}

		m_CurState = State::WILL_CLOSE;

		if (m_RecieveThread.joinable()) {
			m_RecieveThread.join();
		}

		m_CurState = State::CLOSED;

		auto item = m_RecievedClientMap.GetOrderTop();

		while (item != IZ_NULL) {
			auto hashItem = item->GetData();
			auto next = item->GetNext();
			auto client = hashItem->GetData();

			client->Close();

			ReliableUDPClient::Delete(
				m_allocator,
				client);

			item = next;
		}
	}

    IZ_UINT ReliableUDPListener::acceptedNum()
    {
        std::lock_guard<std::mutex> lock(m_RecievedClientMapLocker);

        IZ_UINT ret = 0;

        auto item = m_RecievedClientMap.GetOrderTop();

        while (item != IZ_NULL) {
            auto hashItem = item->GetData();
            auto next = item->GetNext();
            auto client = hashItem->GetData();

            if (client->IsConnected()) {
                ret++;
            }

            item = next;
        }

        return ret;
    }

    IZ_UINT ReliableUDPListener::sendToAll(void* data, IZ_UINT size)
    {
        if (IsClosed()) {
            return 0;
        }

        IZ_UINT ret = 0;

        std::lock_guard<std::mutex> lock(m_RecievedClientMapLocker);

        auto item = m_RecievedClientMap.GetOrderTop();

        while (item != IZ_NULL) {
            auto hashItem = item->GetData();
            auto next = item->GetNext();
            auto client = hashItem->GetData();

            if (client->Send(data, 0, size)) {
                ret++;
            }

            item = next;
        }

        return ret;
    }

    void ReliableUDPListener::ProcRecieve()
    {
        while (!WillClose())
        {
            IPv4Endpoint remoteEp;
            auto segment = OnRecieveSegment(remoteEp);

            if (segment)
            {
                std::lock_guard<std::mutex> lock(m_RecievedClientMapLocker);

                ReliableUDPClient* client = nullptr;

                if (!IsClosed())
                {
                    if (segment->IsSynchronousSegment())
                    {
                        // まずは登録済みかどうか探す.
                        if (!m_RecievedClientMap.ContainsKey(remoteEp))
                        {
                            // 未登録の場合は登録する.

                            client = ReliableUDPClient::Create(
                                m_allocator,
                                this,
                                remoteEp);

                            m_RecievedClientMap.Add(client->GetHashItem());
                        }
                    }
                }

                if (client == nullptr)
                {
                    client = m_RecievedClientMap.FindData(remoteEp);
                }

                if (client != nullptr)
                {
                    client->EnqueueSegment(segment);
                }
            }
        }
    }

    Udp* ReliableUDPListener::GetUdp()
    {
        return m_Udp;
    }

    ReliableUDPListener::ReliableUDPClient* ReliableUDPListener::ReliableUDPClient::Create(
        IMemoryAllocator* allocator,
        ReliableUDPListener* server,
        const IPv4Endpoint& ep)
    {
        void* p = ALLOC(allocator, sizeof(ReliableUDPClient));
        VRETURN(p != IZ_NULL);

        ReliableUDPClient* ret = new(p)ReliableUDPClient(server, ep);
        ret->Init(allocator, server->GetUdp(), RUDPParameter());

        return ret;
    }

	void ReliableUDPListener::ReliableUDPClient::Delete(
		IMemoryAllocator* allocator,
		ReliableUDPClient* client)
	{
		client->~ReliableUDPClient();
		FREE(allocator, client);
	}

    ReliableUDPListener::ReliableUDPClient::ReliableUDPClient(
        ReliableUDPListener* server,
        const IPv4Endpoint& ep)
    {
        m_RemoteEp = ep;
        m_Parent = server;

        m_listItem.Init(this);
        m_hashItem.Init(ep, this);
    }

    void ReliableUDPListener::ReliableUDPClient::EnqueueSegment(Segment* segment)
    {
        if (!IsClosed())
        {
            sys::Lock locker(*this);
            m_RecievedSegmentQueue.AddTail(segment->GetListItem(0));
        }
    }

    void ReliableUDPListener::ReliableUDPClient::OnOpenConnection()
    {
        std::lock_guard<std::mutex> locker(m_Parent->m_AcceptedClientListWaiter);

#if 0
        // 一定サイズを超えていたいら
        if (this.Parent.AcceptedClientList.Count > MAX)
        {
            // Wait
        }
#endif

        m_Parent->m_AcceptedClientList.AddTail(this->GetListItem());
    }

    Segment* ReliableUDPListener::ReliableUDPClient::OnRecieveSegment()
    {
        if (!IsClosed())
        {
            sys::Lock locker(*this);

            if (m_RecievedSegmentQueue.GetItemNum() == 0)
            {
                return nullptr;
            }

            auto item = m_RecievedSegmentQueue.GetTop();

            auto ret = item->GetData();

            item->Leave();

            return ret;
        }

        return nullptr;
    }

    void ReliableUDPListener::ReliableUDPClient::OnSendSegment(Segment* segment)
    {
        IZ_CHAR str[64];
        segment->ToString(str, sizeof(str));

        IZ_PRINTF("Send Segment [%s]\n", str);

        IZ_UINT8 tmp[64];

        void* data = tmp;
        IZ_UINT length = sizeof(tmp);

        if (m_isAllocated) {
            data = m_sendData;
            length = m_Parameter.MaxSegmentSize;
        }

        CMemoryOutputStream ws(data, length);

        segment->WriteBytes(&ws);
        auto bytes = ws.GetBuffer();
        auto size = ws.GetCurPos();
        m_Udp->sendTo(bytes, size, m_RemoteEp);
    }
}   // namespace net
}   // namespace izanagi
