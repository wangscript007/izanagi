#include "graph/dx9/GraphicsDevice_DX9.h"
#include "graph/dx9/Texture_DX9.h"
#include "graph/dx9/CubeTexture_DX9.h"
#include "graph/dx9/Surface_DX9.h"
#include "graph/dx9/VertexBuffer_DX9.h"
#include "graph/dx9/IndexBuffer_DX9.h"
#include "graph/dx9/VertexShader_DX9.h"
#include "graph/dx9/PixelShader_DX9.h"
#include "graph/dx9/VertexDeclaration_DX9.h"

namespace izanagi
{
namespace graph
{
    /**
    * メモリからテクスチャ作成
    */
    CTexture* CGraphicsDeviceDX9::CreateTextureFromMemory(
        void* data,
        IZ_UINT dataSize,
        E_GRAPH_PIXEL_FMT fmt)
    {
        CTexture* pTexture = CTextureDX9::CreateTextureFromMemory(
                                this,
                                m_Allocator,
                                data,
                                dataSize,
                                fmt);
        return pTexture;
    }

    /**
    * テクスチャ作成
    */
    CTexture* CGraphicsDeviceDX9::CreateTexture(
        IZ_UINT width,
        IZ_UINT height,
        IZ_UINT mipLevel,
        E_GRAPH_PIXEL_FMT fmt,
        E_GRAPH_RSC_TYPE rscType)
    {
        CTextureDX9* pTexture = CTextureDX9::CreateTexture(
                                this,
                                m_Allocator,
                                width, height,
                                mipLevel,
                                fmt,
                                rscType);

        if ((pTexture != IZ_NULL) && pTexture->IsOnVram()) {
            // リセット用リストに登録
            m_ResetTexture = InsertResource(pTexture, m_ResetTexture);
        }

        return pTexture;
    }

    /**
    * キューブテクスチャ作成
    */
    CCubeTexture* CGraphicsDeviceDX9::CreateCubeTexture(
        IZ_UINT width,
        IZ_UINT height,
        IZ_UINT mipLevel,
        E_GRAPH_PIXEL_FMT fmt,
        IZ_BOOL bIsDynamic)
    {
        CCubeTexture* pTexture = CCubeTextureDX9::CreateCubeTexture(
                                    this,
                                    m_Allocator,
                                    width,
                                    height,
                                    mipLevel,
                                    fmt);
        return pTexture;
    }

    /**
    * レンダーターゲット作成
    */
    CTexture* CGraphicsDeviceDX9::CreateRenderTarget(
        IZ_UINT width, IZ_UINT height,
        E_GRAPH_PIXEL_FMT fmt)
    {
        CTextureDX9* pRT = CTextureDX9::CreateRenderTarget(
                            this,
                            m_Allocator,
                            width,
                            height,
                            fmt);

        if (pRT != IZ_NULL) {
            // リセット用リストに登録
            m_ResetTexture = InsertResource(pRT, m_ResetTexture);
        }

        return pRT;
    }

    // 深度・ステンシルサーフェス作成
    CSurface* CGraphicsDeviceDX9::CreateDepthStencilSurface(
        IZ_UINT width, 
        IZ_UINT height,
        E_GRAPH_PIXEL_FMT fmt)
    {
        CSurface* ret = CSurfaceDX9::CreateDepthStencilSurface(
                            m_Allocator,
                            this,
                            width, height,
                            fmt);

        return ret;
    }

    /**
    * システムメモリ上にテクスチャ作成
    */
    CTexture* CGraphicsDeviceDX9::CreateTextureOnSysMem(
        IZ_UINT width, IZ_UINT height,
        E_GRAPH_PIXEL_FMT fmt,
        IZ_UINT mipLevel)
    {
        CTexture* pTexture = CTextureDX9::CreateTexture(
                                this,
                                m_Allocator,
                                width,
                                height,
                                mipLevel,
                                fmt,
                                E_GRAPH_RSC_TYPE_STATIC,
                                IZ_TRUE);
        return pTexture;
    }

    /**
    * 頂点バッファ作成
    */
    CVertexBuffer* CGraphicsDeviceDX9::CreateVertexBuffer(
        IZ_UINT stride,
        IZ_UINT vtxNum,
        E_GRAPH_RSC_TYPE createType)
    {
        CVertexBufferDX9* pVB = CVertexBufferDX9::CreateVertexBuffer(
                                this,
                                m_Allocator,
                                stride,
                                vtxNum,
                                createType);

        if ((pVB != IZ_NULL)
            && pVB->IsDynamic())
        {
            // リセット用リストに登録
            m_ResetVB = InsertResource(pVB, m_ResetVB);
        }

        return pVB;
    }

    /**
    * インデックスバッファ作成
    */
    CIndexBuffer* CGraphicsDeviceDX9::CreateIndexBuffer(
        IZ_UINT nIdxNum,
        E_GRAPH_INDEX_BUFFER_FMT fmt,
        E_GRAPH_RSC_TYPE createType)
    {
        CIndexBufferDX9* pIB = CIndexBufferDX9::CreateIndexBuffer(
                                this,
                                m_Allocator,
                                nIdxNum,
                                fmt,
                                createType);

        if ((pIB != IZ_NULL)
            && pIB->IsDynamic())
        {
            // リセット用リストに登録
            m_ResetIB = InsertResource(pIB, m_ResetIB);
        }

        return pIB;
    }

    /**
    * 頂点シェーダ作成
    */
    CVertexShader* CGraphicsDeviceDX9::CreateVertexShader(const void* pProgram)
    {
        CVertexShader* ret = CVertexShaderDX9::CreateVertexShader(
                                this,
                                m_Allocator,
                                pProgram);
        return ret;
    }

    /**
    * ピクセルシェーダ作成
    */
    CPixelShader* CGraphicsDeviceDX9::CreatePixelShader(const void* pProgram)
    {
        CPixelShader* ret = CPixelShaderDX9::CreatePixelShader(
                                this,
                                m_Allocator,
                                pProgram);
        return ret;
    }

    /**
    * 頂点宣言作成
    */
    CVertexDeclaration* CGraphicsDeviceDX9::CreateVertexDeclaration(const SVertexElement* pElem, IZ_UINT nNum)
    {
        CVertexDeclaration* ret = CVertexDeclarationDX9::CreateVertexDeclaration(
                                    this,
                                    m_Allocator,
                                    pElem,
                                    nNum);
        return ret;
    }

    // リソース挿入
    template <class _T>
    _T* CGraphicsDeviceDX9::InsertResource(_T* p, _T* pListTop)
    {
        if (pListTop == IZ_NULL) {
            pListTop = p;
        }
        else {
            _T* pElem = pListTop;
            _T* pTail = IZ_NULL;

            while (pElem != IZ_NULL) {
                pTail = pElem;
                pElem = pElem->m_Next;
            }

            pTail->m_Next = p;
        }

        return pListTop;
    }

    // リソース解放
    template <class _T>
    _T* CGraphicsDeviceDX9::RemoveResource(_T* p, _T* pListTop)
    {
        if (pListTop == p) {
            pListTop = pListTop->m_Next;
        }
        else {
            _T* pElem = pListTop;
            _T* prev = IZ_NULL;

            while (pElem != IZ_NULL) {
                if (pElem == p) {
                    prev->m_Next = pElem->m_Next;
                    break;
                }

                prev = pElem;
                pElem = pElem->m_Next;
            }
        }

        return pListTop;
    }

    // テクスチャ削除
    void CGraphicsDeviceDX9::RemoveTexture(CTextureDX9* p)
    {
        m_ResetTexture = RemoveResource(p, m_ResetTexture);
    }

    // 頂点バッファ削除
    void CGraphicsDeviceDX9::RemoveVertexBuffer(CVertexBufferDX9* p)
    {
        m_ResetVB = RemoveResource(p, m_ResetVB);
    }

    // インデックスバッファ削除
    void CGraphicsDeviceDX9::RemoveIndexBuffer(CIndexBufferDX9* p)
    {
        m_ResetIB = RemoveResource(p, m_ResetIB);
    }
}   // namespace graph
}   // namespace izanagi