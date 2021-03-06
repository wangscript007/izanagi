﻿#if !defined(_IZANAGI_NETWORK_ACKNOWLEDGEMENT_SEGMENT_H__)
#define _IZANAGI_NETWORK_ACKNOWLEDGEMENT_SEGMENT_H__

#include "network/rudp/segment/Segment.h"

/*
*  ACK Segment
*
*   0 1 2 3 4 5 6 7 8            15
*  +-+-+-+-+-+-+-+-+---------------+
*  |0|1|0|0|0|0|0|0|       6       |
*  +-+-+-+-+-+-+-+-+---------------+
*  | Sequence #    |   Ack Number  |
*  +---------------+---------------+
*  |           Checksum            |
*  +---------------+---------------+
*
*/
namespace izanagi {
namespace net {
    class AcknowledgementSegment : public Segment
    {
        friend class Segment;
		friend class ReliableUDP;

    protected:
        AcknowledgementSegment() {}
        virtual ~AcknowledgementSegment() {}

        AcknowledgementSegment(
            IZ_INT sequenceNumber,
            IZ_INT acknowledgedNumber)
            : Segment(Type::ACK, sequenceNumber, RUDP_HEADER_LEN)
        {
            SetAcknowledgedNumber(acknowledgedNumber);
        }

		void Init(
			IMemoryAllocator* allocator,
			IZ_INT sequenceNumber,
			IZ_INT acknowledgedNumber)
		{
			m_allocator = allocator;

			m_Flags = (int)Type::ACK;
			m_SequenceNumber = sequenceNumber;
			m_HeaderLength = RUDP_HEADER_LEN;

			SetAcknowledgedNumber(acknowledgedNumber);
		}

    public:
        virtual Type GetType() const override
        {
            return Type::ACK;
        }

        virtual const IZ_CHAR* GetSegmentType() override
        {
            return "ACK";
        }

        virtual void WriteBytes(IOutputStream* wr) override
        {
            Segment::WriteBytes(wr);

            // For CheckSum
            wr->Write(0);
            wr->Write(0);
        }

        virtual void ReadBytes(
            IMemoryAllocator* allocator,
            IZ_UINT8* bytes, IZ_UINT offset, IZ_UINT length) override
        {
            Segment::ReadBytes(allocator, bytes, offset, length);

            IZ_ASSERT(CheckFlag(Type::ACK));
        }
    };
}   // namespace net
}   // namespace izanagi 

#endif  // #if !defined(_IZANAGI_NETWORK_ACKNOWLEDGEMENT_SEGMENT_H__)
