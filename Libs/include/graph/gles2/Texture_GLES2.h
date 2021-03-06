#if !defined(__IZANAGI_GRAPH_TEXTURE_GLES2_H__)
#define __IZANAGI_GRAPH_TEXTURE_GLES2_H__

#include "graph/GraphDefs.h"
#include "graph/Texture.h"
#include "graph/gles2/TextureProxy.h"

namespace izanagi
{
namespace graph
{
    class CGraphicsDeviceGLES2;

    class CTextureGLES2 : public CTextureProxy<CTexture>
    {
        friend class CGraphicsDeviceGLES2;

    protected:
        // データからテクスチャ作成
        static CTextureGLES2* CreateTextureFromMemory(
            CGraphicsDeviceGLES2* device,
            IMemoryAllocator* allocator,
            const void* data,
            IZ_UINT dataSize,
            E_GRAPH_PIXEL_FMT fmt);

        // テクスチャ作成
        static CTextureGLES2* CreateTexture(
            CGraphicsDeviceGLES2* device,
            IMemoryAllocator* allocator,
            IZ_UINT width,
            IZ_UINT height,
            IZ_UINT mipLevel,
            E_GRAPH_PIXEL_FMT fmt,
            E_GRAPH_RSC_USAGE rscType);

    protected:
        inline CTextureGLES2();
        virtual inline ~CTextureGLES2();

    protected:
        // 本体作成（テクスチャ）
        IZ_BOOL CreateBody_Texture(
            IZ_UINT width,
            IZ_UINT height,
            IZ_UINT mipLevel,
            E_GRAPH_PIXEL_FMT fmt,
            E_GRAPH_RSC_USAGE usage);

        void SetTextureInfo(
            IZ_BOOL isRT,
            IZ_UINT width,
            IZ_UINT height,
            IZ_UINT mipLevel,
            E_GRAPH_PIXEL_FMT fmt,
            E_GRAPH_RSC_USAGE rscType);

        void Initialize();

    public:
        // ロック
        virtual IZ_UINT Lock(
            IZ_UINT level,
            void** data,
            IZ_BOOL isReadOnly,
            IZ_BOOL isDiscard = IZ_FALSE);

        // アンロック
        virtual IZ_BOOL Unlock(IZ_UINT level);

        virtual IZ_BOOL Write(
            IZ_UINT level,
            void* data,
            IZ_UINT x, IZ_UINT y,
            IZ_UINT width, IZ_UINT height) override;

    public:
        virtual IZ_BOOL IsPrepared() const;

        virtual IZ_BOOL Disable();

        virtual IZ_BOOL Restore();

    protected:
        virtual TEX_HANDLE GetTexHandle()
        {
            return m_Texture;
        }

    public:
        GLuint GetRawInterface() { return m_Texture; }

    protected:
        CGraphicsDeviceGLES2* m_Device;

        // 本体
        GLuint m_Texture;

        IZ_BOOL m_IsInitialized;

        IZ_UINT m_Size;

        GLenum m_GLInternal;
        GLenum m_GLFormat;
        GLenum m_GLType;

        IZ_UINT m_LockedSize;
        IZ_INT m_LockedLevel;

        void* m_TemporaryData;
    };

    class CTextureOperator
    {
    public:
        CTextureOperator(CGraphicsDeviceGLES2* device, GLuint targetTexHandle);
        ~CTextureOperator();

        IZ_BOOL operator==(const CBaseTexture* rhs)
        {
            return (m_CurTex == rhs);
        }
        IZ_BOOL operator!=(const CBaseTexture* rhs)
        {
            return (m_CurTex != rhs);
        }

    private:
        CBaseTexture* m_CurTex;
        GLuint m_TargetTexHandle;
    };
}   // namespace graph
}   // namespace izanagi

#endif  // #if !defined(__IZANAGI_GRAPH_TEXTURE_GLES2_H__)
