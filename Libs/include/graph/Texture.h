#if !defined(__IZANAGI_GRAPH_TEXTURE_H__)
#define __IZANAGI_GRAPH_TEXTURE_H__

#include "izDefs.h"
#include "izStd.h"
#include "graph/BaseTexture.h"
#include "graph/Surface.h"

namespace izanagi
{
namespace graph
{
    class CRenderTarget;

    /**
     */
    class CTexture : public CBaseTexture
    {
    protected:
        CTexture() {}
        virtual ~CTexture() {}

        NO_COPIABLE(CTexture);

    public:
        virtual E_GRAPH_TEX_TYPE GetTexType()
        {
            return E_GRAPH_TEX_TYPE_PLANE;
        }

        // ロック
        virtual IZ_UINT Lock(
            IZ_UINT level,
            void** data,
            IZ_BOOL isReadOnly,
            IZ_BOOL isDiscard = IZ_FALSE)
        {
            IZ_ASSERT(IZ_FALSE);
            return 0;
        }

        // アンロック
        virtual IZ_BOOL Unlock(IZ_UINT level)
        {
            IZ_ASSERT(IZ_FALSE);
            return IZ_FALSE;
        }

        virtual IZ_BOOL Write(
            IZ_UINT level,
            void* data,
            IZ_UINT x, IZ_UINT y,
            IZ_UINT width, IZ_UINT height)
        {
            IZ_ASSERT(IZ_FALSE);
            return IZ_FALSE;
        }

        virtual CRenderTarget* AsRenderTarget()
        {
            return IZ_NULL;
        }

        virtual void overrideNativeResource(void* rsc, E_GRAPH_OVERRIDE_NATIVE_RSC_BEHAVIOUR behaviour)
        {
            // Usualy unsupoort.
            IZ_ASSERT(IZ_FALSE);
        }
    };
}   // namespace graph
}   // namespace izanagi

#endif  // #if !defined(__IZANAGI_GRAPH_TEXTURE_H__)
