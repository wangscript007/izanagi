#include "ModelApp.h"
#include "CustomInterp.h"

/////////////////////////////////////////////////

// ジョイントマトリクスを描画時に設定するためのハンドラ
class CSampleMdlRenderHandler : public izanagi::IMshRenderHandler {
    // NOTE
    // IMshRenderHandlerが持つインスタンス作成用メソッドに対して
    // コンストラクタを見せるために必要
    friend class izanagi::IMshRenderHandler;

private:
    CSampleMdlRenderHandler()
    {
        m_pShader = IZ_NULL;
    }

    ~CSampleMdlRenderHandler()
    {
        SAFE_RELEASE(m_pShader);
    }

public:
    virtual void BeginRenderMesh();
    virtual void EndRenderMesh();

    virtual void SetJointMatrix(
        IZ_UINT nIdx,
        const izanagi::math::SMatrix44& mtx);

    virtual void CommitChanges();

public:
    void SetShader(izanagi::IShader* pShader)
    {
        SAFE_REPLACE(m_pShader, pShader);
    }

    izanagi::IShader* GetShader() { return m_pShader; }

private:
    izanagi::IShader* m_pShader;

    IZ_UINT m_nCnt;
    izanagi::math::SMatrix44 m_Mtx[48];

    izanagi::IZ_SHADER_HANDLE m_Handle;
};

void CSampleMdlRenderHandler::BeginRenderMesh()
{
    m_nCnt = 0;

    izanagi::math::SMatrix44::SetUnit(m_Mtx[0]);
    izanagi::math::SMatrix44::SetUnit(m_Mtx[1]);
    izanagi::math::SMatrix44::SetUnit(m_Mtx[2]);
    izanagi::math::SMatrix44::SetUnit(m_Mtx[3]);

    m_Handle = 0;
}

void CSampleMdlRenderHandler::EndRenderMesh()
{
}

void CSampleMdlRenderHandler::SetJointMatrix(
    IZ_UINT nIdx,
    const izanagi::math::SMatrix44& mtx)
{
    izanagi::math::SMatrix44::Copy(m_Mtx[m_nCnt], mtx);
    m_nCnt++;
}

void CSampleMdlRenderHandler::CommitChanges()
{
    if (m_Handle == 0) {
        m_Handle = m_pShader->GetParameterByName("vJointMatrix");
        IZ_ASSERT(m_Handle > 0);
    }

    m_pShader->SetParamValue(
        m_Handle,
        m_Mtx,
        sizeof(izanagi::math::SMatrix44) * m_nCnt);

    m_pShader->CommitChanges();
}

static CSampleMdlRenderHandler* s_MdlRenderHandler = IZ_NULL;

/////////////////////////////////////////////////

#define MSH_FILE_NAME    "data/miku.msh"
#define SKL_FILE_NAME    "data/miku.skl"
//#define ANM_FILE_NAME   "data/walk.anm"
#define ANM_FILE_NAME   "data/walk2.anm"
#define IMG_IDX        (0)
#define CAMERA_Z    (-30.0f)

CModelApp::CModelApp()
{
    m_Img = IZ_NULL;
    m_Mdl = IZ_NULL;
    m_Msh = IZ_NULL;
    m_Skl = IZ_NULL;
    m_Shd = IZ_NULL;
    m_Anm = IZ_NULL;

    FILL_ZERO(m_Mtrl, sizeof(m_Mtrl));

    m_RenderGraph = IZ_NULL;
    m_Renderer = IZ_NULL;
}

CModelApp::~CModelApp()
{
}

// 初期化.
IZ_BOOL CModelApp::InitInternal(
    izanagi::IMemoryAllocator* allocator,
    izanagi::graph::CGraphicsDevice* device,
    izanagi::sample::CSampleCamera& camera)
{
    IZ_BOOL result = IZ_TRUE;

    // Texture
    {
        izanagi::CFileInputStream in;
        VRETURN(in.Open("data/image.img"));

        m_Img = izanagi::CImage::CreateImage(
                    allocator,
                    device,
                    &in);

        VGOTO(result = (m_Img != IZ_NULL), __EXIT__);
    }

    // Animation
    {
        izanagi::CFileInputStream in;
        VRETURN(in.Open(ANM_FILE_NAME));

        m_Anm = izanagi::CAnimation::CreateAnimation(
                    allocator,
                    &in);

        VGOTO(result = (m_Anm != IZ_NULL), __EXIT__);

        izanagi::CAnimation::SetUserFuncInterpScalar(CCustomInterp::InterpScalar);
        izanagi::CAnimation::SetUserFuncInterpVector(CCustomInterp::InterpVector);
    }

    // Timeline
    {
        m_Timeline.Init(
            m_Anm->GetAnimationTime(),
            0.0f);
        m_Timeline.SetIsLoop(IZ_TRUE);
        m_Timeline.SetIsReverse(IZ_FALSE);
        m_Timeline.Start();
        m_Timeline.Reset();
    }

    // Mesh
    {
        izanagi::CFileInputStream in;
        VRETURN(in.Open(MSH_FILE_NAME));

        m_Msh = izanagi::CMesh::CreateMesh(
                    allocator,
                    device,
                    &in);
        
        VGOTO(result = (m_Msh != IZ_NULL), __EXIT__);
    }

    // Skelton
    {
        izanagi::CFileInputStream in;
        VRETURN(in.Open(SKL_FILE_NAME));

        m_Skl = izanagi::CSkeleton::CreateSkeleton(
                    allocator,
                    &in);

        VGOTO(result = (m_Skl != IZ_NULL), __EXIT__);
    }

    // Shader
    {
        izanagi::CFileInputStream in;
        VRETURN(in.Open("data/SkinShader.shd"));

        m_Shd = izanagi::CShaderBasic::CreateShader<izanagi::CShaderBasic>(
                    allocator,
                    device,
                    &in);

        VGOTO(result = (m_Shd != IZ_NULL), __EXIT__);

        // ジョイントマトリクスを描画時に設定するためのハンドラ
        s_MdlRenderHandler = izanagi::IMshRenderHandler::CreateMshRenderHandler<CSampleMdlRenderHandler>(allocator);
        s_MdlRenderHandler->SetShader(m_Shd);
        VGOTO(result = (s_MdlRenderHandler != IZ_NULL), __EXIT__);
    }

    // Model
    {
        m_Mdl = izanagi::CModel::CreateModel(
                    allocator,
                    m_Msh,
                    m_Skl,
                    s_MdlRenderHandler);

        VGOTO(result = (m_Mdl != IZ_NULL), __EXIT__);
    }

    // Material
    {
        izanagi::CFileInputStream in;
        IZ_CHAR tmp[32];

        for (IZ_UINT i = 0; i < COUNTOF(m_Mtrl); i++)
        {
            IZ_SPRINTF(tmp, sizeof(tmp), "data/material_%d.mtrl\0", i);

            VRETURN(in.Open(tmp));

            m_Mtrl[i] = izanagi::CMaterial::CreateMaterial(allocator, &in);
            VGOTO((result = m_Mtrl[i] != IZ_NULL), __EXIT__);

            in.Finalize();
        }

        m_Shd->SetName("DefaultShader");

        for (IZ_UINT i = 0; i < COUNTOF(m_Mtrl); i++)
        {
            IZ_UINT num = m_Mtrl[i]->GetTexNum();

            IZ_PCSTR name = m_Mtrl[i]->GetTexInfoByIdx(num > 1 ? 1 : 0)->name.GetString();

            IZ_BOOL isToon = (::strncmp(name, "toon", 4) == 0);

            m_Mtrl[i]->SetTexture(name, m_Img->GetTexture(isToon ? 0 : 1));
            m_Mtrl[i]->SetShader(m_Shd);

            m_Mdl->SetMaterial(0, m_Mtrl[i]);
        }
    }

    // レンダーグラフ
    m_RenderGraph = izanagi::CRenderGraph::CreateRenderGraph(allocator, 5);
    VGOTO(result = (m_RenderGraph != IZ_NULL), __EXIT__);

    // レンダラ
    m_Renderer = izanagi::CSceneRenderer::CreateSceneRenderer(allocator);
    VGOTO(result = (m_Renderer != IZ_NULL), __EXIT__);

    // カメラ
    camera.Init(
        izanagi::math::CVector4(0.0f, 5.0f, CAMERA_Z, 1.0f),
        izanagi::math::CVector4(0.0f, 5.0f, 0.0f, 1.0f),
        izanagi::math::CVector4(0.0f, 1.0f, 0.0f, 1.0f),
        1.0f,
        500.0f,
        izanagi::math::CMath::Deg2Rad(60.0f),
        (IZ_FLOAT)SCREEN_WIDTH / SCREEN_HEIGHT);
    camera.Update();

__EXIT__:
    if (!result) {
        ReleaseInternal();
    }

    return IZ_TRUE;
}

// 解放.
void CModelApp::ReleaseInternal()
{
    SAFE_RELEASE(m_Img);
    SAFE_RELEASE(m_Mdl);
    SAFE_RELEASE(m_Msh);
    SAFE_RELEASE(m_Skl);
    SAFE_RELEASE(m_Shd);
    SAFE_RELEASE(m_Anm);

    for (IZ_UINT i = 0; i < COUNTOF(m_Mtrl); i++)
    {
        SAFE_RELEASE(m_Mtrl[i]);
    }

    SAFE_RELEASE(m_RenderGraph);
    SAFE_RELEASE(m_Renderer);

    SAFE_RELEASE(s_MdlRenderHandler);
}

// 更新.
void CModelApp::UpdateInternal(izanagi::graph::CGraphicsDevice* device)
{
    GetCamera().Update();

    // 時間更新
    IZ_FLOAT fElapsed = GetTimer(0).GetTime();
    fElapsed /= 1000.0f;

    IZ_FLOAT time = m_Timeline.GetTime();

    // アニメーション適用
    m_Mdl->ApplyAnimation(time, m_Anm);

    m_Mdl->Update();

    m_RenderGraph->BeginRegister();
    {
        // 位置は原点なので
        m_RenderGraph->Register(
            GetCamera(),
            izanagi::math::CVector4(),
            m_Mdl);
    }
    m_RenderGraph->EndRegister();

    m_Timeline.Advance(fElapsed * 30.0f);
}

namespace {
    inline void _SetShaderParam(
        izanagi::CShaderBasic* shader,
        const char* name,
        const void* value,
        IZ_UINT bytes)
    {
        izanagi::IZ_SHADER_HANDLE handle = shader->GetParameterByName(name);
        IZ_ASSERT(handle != 0);

        shader->SetParamValue(
            handle,
            value,
            bytes);
    }
}

// 描画.
void CModelApp::RenderInternal(izanagi::graph::CGraphicsDevice* device)
{
    izanagi::sample::CSampleCamera& camera = GetCamera();

#if 0
    IZ_UINT passCnt = m_Shd->Begin(0, IZ_FALSE);
    {
        IZ_ASSERT(passCnt >= 1);
        if (m_Shd->BeginPass(0)) {
            // シェーダ定数セット
            {
                _SetShaderParam(
                    m_Shd,
                    "g_mW2C",
                    (void*)&camera.GetParam().mtxW2C,
                    sizeof(camera.GetParam().mtxW2C));
            }

            // テクスチャセット
            //device->SetTexture(0, m_Img->GetTexture(IMG_IDX));

            m_Shd->CommitChanges();

            // モデル描画
            m_Mdl->Render();

            m_Shd->EndPass();
        }
    }
    m_Shd->End();
#else
    _SetShaderParam(
        m_Shd,
        "g_mW2C",
        (void*)&camera.GetParam().mtxW2C,
        sizeof(camera.GetParam().mtxW2C));

    m_RenderGraph->Render(
        device,
        m_Renderer,
        s_MdlRenderHandler);
#endif
}
