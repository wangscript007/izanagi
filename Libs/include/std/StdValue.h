#if !defined(__IZANAGI_STD_STD_VALUE_H__)
#define __IZANAGI_STD_STD_VALUE_H__

#include "izDefs.h"

namespace izanagi {
    /** 多態性を持った値クラス
     */
    class CValue {
    public:
        enum TYPE {
            TYPE_INT8,
            TYPE_UINT8,
            TYPE_INT16,
            TYPE_UINT16,
            TYPE_INT32,
            TYPE_UINT32,
            TYPE_FLOAT,
            TYPE_PTR,

            TYPE_NUM,
            TYPE_FORCE_INT32 = 0x7fffffff,
        };

    public:
        CValue()
        {
            m_Type = TYPE_UINT32;
            m_ValInt64 = 0;
        }
        ~CValue() {}

        CValue(IZ_INT8 n) { SetValue(n); }
        CValue(IZ_UINT8 n) { SetValue(n); }
        CValue(IZ_INT16 n) { SetValue(n); }
        CValue(IZ_UINT16 n) { SetValue(n); }
        CValue(IZ_INT32 n) { SetValue(n); }
        CValue(IZ_UINT32 n) { SetValue(n); }
        CValue(IZ_FLOAT f) { SetValue(f); }
        CValue(void* p) { SetValue(p); }

        CValue(const CValue& rhs)
        {
            *this = rhs;
        }

        const CValue& operator=(const CValue& rhs)
        {
            m_Type = rhs.m_Type;
            m_ValInt64 = rhs.m_ValInt64;
            return *this;
        }

        template <typename _T>
        const CValue& operator=(const _T& rhs)
        {
            this->SetValue(rhs);
            return *this;
        }

    public:
        // タイプ取得
        TYPE GetType() const
        {
            return m_Type;
        }

        // 値取得
        template <typename _T>
        _T GetValue() const
        {
            return *(reinterpret_cast<_T*>(m_ValPtr));
        }

        operator IZ_INT8() const
        {
            return GetValueAsInt32();
        }
        operator IZ_UINT8() const
        {
            return GetValueAsUInt32();
        }
        operator IZ_INT16() const
        {
            return GetValueAsInt32();
        }
        operator IZ_UINT16() const
        {
            return GetValueAsUInt32();
        }
        operator IZ_INT32() const
        {
            return GetValueAsInt32();
        }
        operator IZ_UINT32() const
        {
            return GetValueAsUInt32();
        }
        operator IZ_FLOAT() const
        {
            return GetValueAsFloat();
        }
        operator void*() const
        {
            return GetValueAsPtr();
        }

        // 値取得
        IZ_INT32 GetValueAsInt32() const
        {
            return static_cast<IZ_INT32>(m_ValInt64);
        }

        IZ_UINT32 GetValueAsUInt32() const
        {
            return static_cast<IZ_UINT32>(m_ValUInt64);
        }

        IZ_FLOAT GetValueAsFloat() const
        {
            return m_ValFloat;
        }

        IZ_BOOL GetValueAsBool() const
        {
            return m_ValBool;
        }

        void* GetValueAsPtr() const
        {
            return m_ValPtr;
        }
        
        // 値セット
        void SetValue(IZ_INT8 v)
        {
            m_Type = TYPE_INT8;
            m_ValInt64 = v;
        }

        void SetValue(IZ_UINT8 v)
        {
            m_Type = TYPE_UINT8;
            m_ValUInt64 = v;
        }

        void SetValue(IZ_INT16 v)
        {
            m_Type = TYPE_INT16;
            m_ValInt64 = v;
        }

        void SetValue(IZ_UINT16 v)
        {
            m_Type = TYPE_UINT16;
            m_ValUInt64 = v;
        }

        void SetValue(IZ_INT32 v)
        {
            m_Type = TYPE_INT32;
            m_ValInt64 = v;
        }

        void SetValue(IZ_UINT32 v)
        {
            m_Type = TYPE_UINT32;
            m_ValUInt64 = v;
        }

        void SetValue(IZ_FLOAT v)
        {
            m_Type = TYPE_FLOAT;
            m_ValFloat = v;
        }

        void SetValue(void* v)
        {
            m_Type = TYPE_PTR;
            m_ValPtr = v;
        }

        IZ_BOOL operator==(const CValue& rhs)
        {
            return (m_ValInt64 == rhs.m_ValInt64);
        }

        IZ_BOOL operator!=(const CValue& rhs)
        {
            return !(*this == rhs);
        }
        
    protected:
        TYPE m_Type;
        union {
            IZ_INT64  m_ValInt64;
            IZ_UINT64 m_ValUInt64;
            IZ_FLOAT  m_ValFloat;
            IZ_BOOL   m_ValBool;
            void*     m_ValPtr;
        };
    };

    template <>
    inline IZ_INT8 CValue::GetValue<IZ_INT8>() const { return static_cast<IZ_INT8>(m_ValInt64); }

    template <>
    inline IZ_UINT8 CValue::GetValue<IZ_UINT8>() const { return static_cast<IZ_UINT8>(m_ValUInt64); }

    template <>
    inline IZ_INT16 CValue::GetValue<IZ_INT16>() const { return static_cast<IZ_INT16>(m_ValInt64); }

    template <>
    inline IZ_UINT16 CValue::GetValue<IZ_UINT16>() const { return static_cast<IZ_UINT16>(m_ValUInt64); }

    template <>
    inline IZ_INT32 CValue::GetValue<IZ_INT32>() const { return static_cast<IZ_INT32>(m_ValInt64); }

    template <>
    inline IZ_UINT32 CValue::GetValue<IZ_UINT32>() const { return static_cast<IZ_UINT32>(m_ValUInt64); }

    template <>
    inline IZ_FLOAT CValue::GetValue<IZ_FLOAT>() const { return m_ValFloat; }
}   // namespace izanagi

#endif  // #if !defined(__IZANAGI_STD_STD_VALUE_H__)
