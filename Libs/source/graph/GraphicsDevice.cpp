#include "graph/GraphicsDevice.h"
#include "graph/VertexBuffer.h"
#include "graph/IndexBuffer.h"
#include "graph/VertexDeclaration.h"
#include "graph/BaseTexture.h"
#include "graph/VertexShader.h"
#include "graph/PixelShader.h"
#include "graph/ShaderProgram.h"
#include "graph/2d/2DRenderer.h"

namespace izanagi
{
namespace graph
{
    // コンストラクタ
    CGraphicsDevice::CGraphicsDevice()
    {
        m_Allocator = IZ_NULL;

        m_2DRenderer = IZ_NULL;

        m_RT = IZ_NULL;
        m_Depth = IZ_NULL;
    
        FILL_ZERO(m_Texture, sizeof(m_Texture));
        FILL_ZERO(m_SamplerState, sizeof(m_SamplerState));

        m_Flags.is_call_begin = IZ_FALSE;
        m_Flags.is_render_2d = IZ_FALSE;
        m_Flags.is_force_set_state = IZ_FALSE;
        m_Flags.is_lost_device = IZ_FALSE;
    }

    // デストラクタ
    CGraphicsDevice::~CGraphicsDevice()
    {
        SAFE_RELEASE(m_RT);
        SAFE_RELEASE(m_Depth);

        SAFE_RELEASE(m_2DRenderer);
    }

    // シーン描画開始
    IZ_BOOL CGraphicsDevice::BeginScene(
        IZ_DWORD nClearFlags,
        IZ_COLOR nClearColor/*= 0*/,
        IZ_FLOAT fClearZ/*= 1.0f*/,
        IZ_DWORD nClearStencil/*= 0*/)
    {
        return BeginScene(
                IZ_NULL, 0,
                IZ_NULL,
                nClearFlags,
                nClearColor,
                fClearZ,
                nClearStencil);
    }

    // シーン描画開始
    IZ_BOOL CGraphicsDevice::BeginScene(
        CRenderTarget** pRT,
        IZ_UINT nCount,
        IZ_DWORD nClearFlags,
        IZ_COLOR nClearColor/*= 0*/,
        IZ_FLOAT fClearZ/*= 1.0f*/,
        IZ_DWORD nClearStencil/*= 0*/)
    {
        return BeginScene(
                pRT, nCount,
                IZ_NULL,
                nClearFlags,
                nClearColor,
                fClearZ,
                nClearStencil);
    }

    // 2D描画開始
    IZ_BOOL CGraphicsDevice::Begin2D()
    {
        IZ_ASSERT(m_2DRenderer != IZ_NULL);

        // 描画開始
        IZ_BOOL ret = m_2DRenderer->BeginDraw();

        if (ret) {
            // 2D描画用レンダーステート設定
            SaveRenderState();

            SetRenderState(E_GRAPH_RS_ZWRITEENABLE, IZ_FALSE);
            SetRenderState(E_GRAPH_RS_ZENABLE, IZ_FALSE);
            SetRenderState(E_GRAPH_RS_ALPHATESTENABLE, IZ_FALSE);
            SetRenderState(E_GRAPH_RS_ALPHABLENDENABLE, IZ_TRUE);
            SetRenderState(E_GRAPH_RS_BLENDMETHOD, E_GRAPH_ALPHA_BLEND_NORMAL);
            SetRenderState(E_GRAPH_RS_FILLMODE, E_GRAPH_FILL_MODE_SOLID);
            SetRenderState(E_GRAPH_RS_CULLMODE, E_GRAPH_CULL_DEFAULT);

            SetShaderProgram(IZ_NULL);

            m_Flags.is_render_2d = IZ_TRUE;
        }

        return ret;
    }

    // 2D描画終了
    IZ_BOOL CGraphicsDevice::End2D()
    {
        IZ_BOOL ret = IZ_TRUE;

        if (m_Flags.is_render_2d) {
            IZ_ASSERT(m_2DRenderer != IZ_NULL);

            // 描画終了
            ret = m_2DRenderer->EndDraw(this);

            // レンダーステートを元に戻す
            if (ret) {
                LoadRenderState();
            }
        }

        m_Flags.is_render_2d = IZ_FALSE;

        return ret;
    }

    // 2D描画バッファフラッシュ
    IZ_BOOL CGraphicsDevice::Flush2D()
    {
        IZ_BOOL ret = m_2DRenderer->Flush(this);
        return ret;
    }

    // 2Dスプライト描画
    IZ_BOOL CGraphicsDevice::Draw2DSprite(
        const CFloatRect& rcSrc,
        const CIntRect& rcDst,
        const IZ_COLOR color/*= IZ_COLOR_RGBA(255, 255, 255, 255)*/)
    {
        IZ_ASSERT(m_2DRenderer != IZ_NULL);
        IZ_ASSERT(m_Flags.is_render_2d);

        IZ_BOOL ret = IZ_FALSE;

        if (m_Flags.is_render_2d) {
            ret = m_2DRenderer->DrawSprite(
                    this,
                    rcSrc, rcDst,
                    color);
        }

        return ret;
    }

    // 2Dスプライト描画
    IZ_BOOL CGraphicsDevice::Draw2DSpriteEx(
        const CIntRect& rcSrc,
        const CIntRect& rcDst,
        const IZ_COLOR color/*= IZ_COLOR_RGBA(255, 255, 255, 255)*/)
    {
        IZ_ASSERT(m_2DRenderer != IZ_NULL);
        IZ_ASSERT(m_Flags.is_render_2d);

        IZ_BOOL ret = IZ_FALSE;

        if (m_Flags.is_render_2d) {
            ret = m_2DRenderer->DrawSpriteEx(
                    this,
                    rcSrc, rcDst,
                    color);
        }

        return ret;
    }

    // 2D矩形描画
    IZ_BOOL CGraphicsDevice::Draw2DRect(
        const CIntRect& rcDst,
        const IZ_COLOR color)
    {
        IZ_ASSERT(m_2DRenderer != IZ_NULL);
        IZ_ASSERT(m_Flags.is_render_2d);

        IZ_BOOL ret = IZ_FALSE;

        if (m_Flags.is_render_2d) {
            ret = m_2DRenderer->DrawRect(
                    this,
                    rcDst,
                    color);
        }

        return ret;
    }

    // 2Dライン描画
    IZ_BOOL CGraphicsDevice::Draw2DLine(
        const CIntPoint& ptStart,
        const CIntPoint& ptGoal,
        const IZ_COLOR color)
    {
        IZ_ASSERT(m_2DRenderer != IZ_NULL);
        IZ_ASSERT(m_Flags.is_render_2d);

        IZ_BOOL ret = IZ_FALSE;

        if (m_Flags.is_render_2d) {
            ret = m_2DRenderer->DrawLine(
                    this,
                    ptStart, ptGoal,
                    color);
        }

        return ret;
    }

    // Draw a triangle.
    IZ_BOOL CGraphicsDevice::Draw2DTriangle(
        const CIntPoint& pt0,
        const CIntPoint& pt1,
        const CIntPoint& pt2,
        const IZ_COLOR color)
    {
        IZ_ASSERT(m_2DRenderer != IZ_NULL);
        IZ_ASSERT(m_Flags.is_render_2d);

        IZ_BOOL ret = IZ_FALSE;

        if (m_Flags.is_render_2d) {
            ret = m_2DRenderer->DrawTriangle(
                this,
                pt0, pt1, pt2,
                color);
        }

        return ret;
    }

    // 2D描画モードセット
    void CGraphicsDevice::Set2DRenderOp(E_GRAPH_2D_RENDER_OP nOp)
    {
        IZ_ASSERT(m_2DRenderer != IZ_NULL);
        m_2DRenderer->SetRenderOp(this, nOp);
    }

    // 2D描画モード取得
    E_GRAPH_2D_RENDER_OP CGraphicsDevice::Get2DRenderOp() const
    {
        IZ_ASSERT(m_2DRenderer != IZ_NULL);
        return m_2DRenderer->GetRenderOp();
    }

    // ユーザー定義のシェーダをセット
    void CGraphicsDevice::SetUserDefs2DShader(
        CVertexShader* vs,
        CPixelShader* ps)
    {
        IZ_ASSERT(m_2DRenderer != IZ_NULL);
        m_2DRenderer->SetUserDefsShader(vs, ps);
    }

    // 描画モードがユーザー定義の時のみ2D用のシェーダープログラムを取得
    CShaderProgram* CGraphicsDevice::Get2DShaderProgramIfRenderOpIsUserDefs()
    {
        IZ_ASSERT(m_2DRenderer != IZ_NULL);
        CShaderProgram* ret = m_2DRenderer->Get2DShaderProgramIfRenderOpIsUserDefs();

        return ret;
    }

    // テクスチャセット
    IZ_BOOL CGraphicsDevice::SetTexture(IZ_UINT nStage, CBaseTexture* pTex)
    {
        if (m_Texture[nStage] == pTex) {
            // すでにセットされている
            return IZ_TRUE;
        }
        else {
            // 異なるテクスチャ
            if (m_Flags.is_render_2d) {
                // 2D描画中の場合
                // バッファに溜まっている分を描画してしまう
                if (!m_2DRenderer->Flush(this)) {
                    IZ_ASSERT(IZ_FALSE);
                    return IZ_FALSE;
                }
            }
        }

        VRETURN(SetTextureInternal(nStage, pTex));

        return IZ_TRUE;
    }

    IZ_BOOL CGraphicsDevice::SetVertexBufferInstanced(
        IZ_UINT streamIdx,
        IZ_UINT offsetByte,
        IZ_UINT stride,
        E_GRAPH_VB_USAGE usage,
        IZ_UINT divisor,
        CVertexBuffer* vb)
    {
        // Not supported...
        IZ_ASSERT(IZ_FALSE);
        return IZ_TRUE;
    }

    // テクスチャ取得
    CBaseTexture* CGraphicsDevice::GetTexture(IZ_UINT nStage)
    {
        IZ_ASSERT(nStage < TEX_STAGE_NUM);
        return m_Texture[nStage];
    }

    // シェーダプログラム取得
    CShaderProgram* CGraphicsDevice::GetShaderProgram()
    {
        return m_RenderState.curShader;
    }

    // レンダーターゲットセット
    IZ_BOOL CGraphicsDevice::PushRenderTarget(CRenderTarget** rt, IZ_UINT num)
    {
        IZ_ASSERT(rt != IZ_NULL);
        IZ_ASSERT(num <= MAX_MRT_NUM);

        IZ_BOOL ret = IZ_FALSE;

        // 現在のレンダーターゲットをプッシュ
        for (IZ_UINT i = 0; i < num; ++i) {
            CRenderTarget* curRT = GetRenderTarget(i);

            if ((rt != IZ_NULL) && (curRT != rt[i]))
            {
                ret = m_RTMgr[i].Push(curRT);
                if (!ret) {
                    break;
                }
            }
        }

        if (ret) {
            SetRenderTarget(rt, num);
        }

        return ret;
    }

    void CGraphicsDevice::SetRenderTarget(CRenderTarget** rt, IZ_UINT num)
    {
        // レンダーターゲットを入れ替える
        SetRenderTargetInternal(rt, num);

        // TODO
        // 強制的に０番目のサーフェスのサイズにビューポートを変換する
        const SViewport& curVp = GetViewport();
        SViewport vp;
        memcpy(&vp, &curVp, sizeof(vp));
        vp.x = vp.y = 0;
        vp.width = rt[0]->GetWidth();
        vp.height = rt[0]->GetHeight();
        SetViewport(vp);
    }

    // 深度・ステンシルセット
    IZ_BOOL CGraphicsDevice::PushDepthStencil(CRenderTarget* rt)
    {
        IZ_BOOL ret = IZ_FALSE;

        CRenderTarget* curDepthRT = GetDepthSrencil();

        // 現在の深度をプッシュ
        if ((rt != IZ_NULL)
            && (curDepthRT != rt))
        {
            ret = m_DepthMgr.Push(curDepthRT);
        }

        if (ret) {
            SetDepthStencil(rt);
        }

        return ret;
    }

    IZ_BOOL CGraphicsDevice::DrawIndexedInstancedPrimitive(
        E_GRAPH_PRIM_TYPE prim_type,
        IZ_UINT vtxOffset,
        IZ_UINT vtxNum,
        IZ_UINT idxOffset,
        IZ_UINT nPrimCnt)
    {
        return DrawIndexedPrimitive(prim_type, vtxOffset, vtxNum, idxOffset, nPrimCnt);
    }

    IZ_BOOL CGraphicsDevice::DrawInstancedPrimitive(
        E_GRAPH_PRIM_TYPE prim_type,
        IZ_UINT idxOffset,
        IZ_UINT nPrimCnt)
    {
        return DrawPrimitive(prim_type, idxOffset, nPrimCnt);
    }

    IZ_BOOL CGraphicsDevice::SetViewport(const SViewport& vp)
    {
        if (m_Flags.is_render_2d) {
            // TODO
            // 2D描画中は不可
            return IZ_TRUE;
        }

        auto ret = m_RenderState.SetViewport(this, vp);

        IZ_ASSERT(ret);
        return ret;
    }

    // Sets stencil function.
    void CGraphicsDevice::SetStencilFunc(
        E_GRAPH_CMP_FUNC cmp,
        IZ_INT ref,
        IZ_DWORD mask)
    {
        m_RenderState.SetStencilFunc(this, cmp, ref, mask);
    }

    // Sets stencil operations.
    void CGraphicsDevice::SetStencilOp(
        E_GRAPH_STENCIL_OP pass,
        E_GRAPH_STENCIL_OP zfail,
        E_GRAPH_STENCIL_OP fail,
        IZ_BOOL isFront/*= IZ_TRUE*/)
    {
        m_RenderState.SetStencilOp(this, isFront, pass, zfail, fail);
    }
}   // namespace graph
}   // namespace izanagi
