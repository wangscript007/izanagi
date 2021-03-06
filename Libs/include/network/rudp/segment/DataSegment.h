﻿#if !defined(_IZANAGI_NETWORK_DATA_SEGMENT_H__)
#define _IZANAGI_NETWORK_DATA_SEGMENT_H__

#include "network/rudp/segment/Segment.h"

/*
*  DATa Segment
*
*   0 1 2 3 4 5 6 7 8            15
*  +-+-+-+-+-+-+-+-+---------------+
*  |0|1|0|0|0|0|0|0|       6       |
*  +-+-+-+-+-+-+-+-+---------------+
*  | Sequence #    |   Ack Number  |
*  +---------------+---------------+
*  |           Checksum            |
*  +---------------+---------------+
*  | ...                           |
*  +-------------------------------+
*
*/
namespace izanagi {
namespace net {
    class DataSegment : public Segment
    {
        friend class Segment;
        friend class ReliableUDP;

    public:
        virtual Type GetType() const override
        {
            return Type::DAT;
        }

        virtual const IZ_CHAR* GetSegmentType() override
        {
            return "DAT";
        }

    private:
        DataSegment() {}

        DataSegment(
            IMemoryAllocator* allocator,
            IZ_INT sequenceNumber,
            IZ_INT acknowledgedNumber,
            IZ_UINT8* bytes,
            IZ_UINT offset,
            IZ_UINT length);

        virtual ~DataSegment();

        virtual void WriteBytes(IOutputStream* wr) override;

        virtual void ReadBytes(
            IMemoryAllocator* allocator,
            IZ_UINT8* bytes, IZ_UINT offset, IZ_UINT length) override;

        IZ_UINT Size() const
        {
            return m_Size;
        }

        const void* Data() const
        {
            return m_Data;
        }

    private:
        void* m_Data{ nullptr };
        IZ_UINT m_Size{ 0 };
    };
}   // namespace net
}   // namespace izanagi

#endif  // _IZANAGI_NETWORK_DATA_SEGMENT_H__