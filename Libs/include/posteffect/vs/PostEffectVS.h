#if !defined(__IZANAGI_POSTEFFECT_VS_H__)
#define __IZANAGI_POSTEFFECT_VS_H__

#include "izStd.h"
#include "izGraph.h"
#include "izMath.h"
#include "../PostEffectDefs.h"

namespace izanagi {
    // 頂点シェーダ基本
    class CPostEffectVS : public CObject {
        friend class CPostEffectVSManager;

    public:
        // インスタンス作成
        template <class _T>
        static _T* Create(
            IMemoryAllocator* pAllocator,
            graph::CGraphicsDevice* pDevice,
            IZ_UINT8* pProgram);

    protected:
        CPostEffectVS();
        virtual ~CPostEffectVS();

        NO_COPIABLE(CPostEffectVS);

        IZ_DEFINE_INTERNAL_RELEASE();

    protected:
        // シェーダ作成
        IZ_BOOL CreateShader(
            graph::CGraphicsDevice* device,
            IZ_UINT8* pProgram);

    public:
        // シェーダパラメータ初期化済みかどうか
        virtual IZ_BOOL IsInitilizedShaderParameter();

        // シェーダパラメータ初期化
        virtual void InitShaderParameter(
            graph::CGraphicsDevice* device,
            graph::CShaderProgram* program);

        // 描画
        void PrepareRender(
            graph::CGraphicsDevice* device,
            graph::CShaderProgram* program,
            IZ_FLOAT fPosOffsetX = 0.0f,
            IZ_FLOAT fPosOffsetY = 0.0f,
            const SFloatRect* pTexCoord = IZ_NULL);

        // パラメータセット
        virtual void RegisterParameter(
            const math::SVector4* pVector,
            IZ_UINT num);

        // 頂点シェーダ本体を取得
        graph::CVertexShader* GetVertexShader();

    protected:
        // 共通パラメータセット
        void ApplyShaderParameter(
            graph::CGraphicsDevice* device,
            graph::CShaderProgram* program,
            IZ_FLOAT fPosOffsetX,
            IZ_FLOAT fPosOffsetY,
            const SFloatRect* pTexCoord);

        // パラメータセット
        virtual void ApplyShaderParameter(
            graph::CGraphicsDevice* device,
            graph::CShaderProgram* program);

    protected:
        // 頂点バッファセット
        inline void SetVertexBuffer(graph::CVertexBuffer* pVB);

        // 頂点宣言セット
        inline void SetVertexDeclaration(graph::CVertexDeclaration* pVtxDecl);

        // 頂点シェーダタイプ取得
        inline E_POSTEFFECT_VTX_SHADER GetVSType() const;

    protected:
        enum {
            COMMON_HANDLE_PosOffset = 0,
            COMMON_HANDLE_TexParam,

            COMMON_HANDLE_NUM,
        };

    protected:
        IMemoryAllocator* m_Allocator;

        E_POSTEFFECT_VTX_SHADER m_Type;

        graph::CVertexShader* m_pShader;
        graph::CVertexBuffer* m_pVB;
        graph::CVertexDeclaration* m_pVtxDecl;

        // 共通シェーダ定数ハンドル
        SHADER_PARAM_HANDLE m_hCommonHandle[COMMON_HANDLE_NUM];

        math::SVector4 m_vecPosOffset;
        math::SVector4 m_vecTexParam;
    };

    // inline ****************************

    // インスタンス作成
    template <class _T>
    _T* CPostEffectVS::Create(
        IMemoryAllocator* pAllocator,
        graph::CGraphicsDevice* pDevice,
        IZ_UINT8* pProgram)
    {
        _T* pInstance = IZ_NULL;

        // メモリ確保
        IZ_UINT8* pBuf = (IZ_UINT8*)ALLOC_ZERO(pAllocator, sizeof(_T));
        IZ_BOOL result = (pBuf != IZ_NULL);
        VGOTO(result, __EXIT__);

        // インスタンス作成
        pInstance = new(pBuf) _T;
        {
            pInstance->AddRef();
            pInstance->m_Allocator = pAllocator;

            // シェーダ作成
            result = pInstance->CreateShader(pDevice, pProgram);
            VGOTO(result, __EXIT__);
        }

    __EXIT__:
        if (!result) {
            if (pInstance != IZ_NULL) {
                SAFE_RELEASE(pInstance);
            }
            else if (pBuf != NULL) {
                pAllocator->Free(pBuf);
            }
        }
        return pInstance;
    }

    // 頂点バッファセット
    void CPostEffectVS::SetVertexBuffer(graph::CVertexBuffer* pVB)
    {
        SAFE_REPLACE(m_pVB, pVB);
    }

    // 頂点宣言セット
    void CPostEffectVS::SetVertexDeclaration(graph::CVertexDeclaration* pVtxDecl)
    {
        SAFE_REPLACE(m_pVtxDecl, pVtxDecl);
    }

    // 頂点シェーダタイプ取得
    E_POSTEFFECT_VTX_SHADER CPostEffectVS::GetVSType() const
    {
        return m_Type;
    }
}   // namespace izanagi

#endif  // #if !defined(__IZANAGI_POSTEFFECT_VS_H__)
