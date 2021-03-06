#ifdef __IZ_DX9__
#if !defined(__IZANAGI_GRAPH_TEXTURE_DX9_H__)
#define __IZANAGI_GRAPH_TEXTURE_DX9_H__

#include "izDX9Defs.h"
#include "graph/Texture.h"

namespace izanagi
{
namespace graph
{
    class CGraphicsDeviceDX9;
    class CSurfaceDX9;

    class CTextureDX9 : public CTexture
    {
        friend class CGraphicsDeviceDX9;

    private:
        // ファイルからテクスチャ作成
        static CTextureDX9* CreateTextureFromFile(
            CGraphicsDeviceDX9* device,
            IMemoryAllocator* allocator,
            IZ_PCSTR path,
            E_GRAPH_PIXEL_FMT fmt);

        // データからテクスチャ作成
        static CTextureDX9* CreateTextureFromMemory(
            CGraphicsDeviceDX9* device,
            IMemoryAllocator* allocator,
            const void* data,
            IZ_UINT dataSize,
            E_GRAPH_PIXEL_FMT fmt);

        // テクスチャ作成
        static CTextureDX9* CreateTexture(
            CGraphicsDeviceDX9* device,
            IMemoryAllocator* allocator,
            IZ_UINT width,
            IZ_UINT height,
            IZ_UINT mipLevel,
            E_GRAPH_PIXEL_FMT fmt,
            E_GRAPH_RSC_USAGE rscType,
            IZ_BOOL isOnSysMem = IZ_FALSE);

    private:
        inline CTextureDX9();
        virtual inline ~CTextureDX9();

    private:
        template <typename _T, typename _Func>
        static CTextureDX9* CreateBody_From(
            CGraphicsDeviceDX9* device,
            IMemoryAllocator* allocator,
            _T tInput,
            IZ_UINT inputSize,
            _Func& func,
            E_GRAPH_PIXEL_FMT fmt);

        // 本体作成（テクスチャ）
        IZ_BOOL CreateBody_Texture(
            IZ_UINT width,
            IZ_UINT height,
            IZ_UINT mipLevel,
            E_GRAPH_PIXEL_FMT fmt,
            E_GRAPH_RSC_USAGE usage,
            IZ_BOOL isOnSysMem);

        // テクスチャ情報取得
        void GetTextureInfo();

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

        virtual void overrideNativeResource(void* rsc, E_GRAPH_OVERRIDE_NATIVE_RSC_BEHAVIOUR behaviour) override;

    public:
        virtual IZ_BOOL IsPrepared() const;

        virtual IZ_BOOL Disable();

        virtual IZ_BOOL Restore();

    private:
        virtual TEX_HANDLE GetTexHandle()
        {
            return m_Texture;
        }

    public:
        D3D_TEXTURE* GetRawInterface() { return m_Texture; }

    private:
        CGraphicsDeviceDX9* m_Device;

        // 本体
        D3D_TEXTURE* m_Texture;

        CTextureDX9* m_Next;
    };
}   // namespace graph
}   // namespace izanagi

#endif  // #if !defined(__IZANAGI_GRAPH_TEXTURE_DX9_H__)
#endif  // #ifdef __IZ_DX9__