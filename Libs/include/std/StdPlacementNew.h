#if !defined(__IZANAGI_STD_PLACEMENT_NEW_H__)
#define __IZANAGI_STD_PLACEMENT_NEW_H__

#include "izDefs.h"

#define IZ_DECL_PLACEMENT_NEW() \
    inline void* operator new(size_t size) throw() { IZ_ASSERT(IZ_FALSE); return IZ_NULL; } \
    inline void* operator new(size_t size, void* buf) throw() { return buf; } \
    inline void operator delete(void* data) throw() {} \
    inline void operator delete(void* data, void* buf) throw() {}

namespace izanagi {
    class CPlacementNew {
    protected:
        CPlacementNew() {}
        ~CPlacementNew() {}

        NO_COPIABLE(CPlacementNew);

    public:
        IZ_DECL_PLACEMENT_NEW();
    };
}   // namespace izanagi

#endif  // #if !defined(__IZANAGI_STD_PLACEMENT_NEW_H__)
