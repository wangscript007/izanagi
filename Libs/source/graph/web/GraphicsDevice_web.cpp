#include "graph/gles2/GraphicsDevice_GLES2.h"
#include "graph/gles2/Texture_GLES2.h"
#include "graph/gles2/CubeTexture_GLES2.h"
#include "graph/gles2/VertexBuffer_GLES2.h"
#include "graph/gles2/IndexBuffer_GLES2.h"
#include "graph/gles2/ShaderProgram_GLES2.h"
#include "graph/gles2/VertexDeclaration_GLES2.h"
#include "graph/gles2/FrameBufferObject.h"
#include "graph/gles2/RenderTarget_GLES2.h"
#include "graph/2d/2DRenderer.h"

#include <GL/glut.h>

namespace izanagi
{
namespace graph
{
    class CGraphicsDeviceEmscripten : public CGraphicsDeviceGLES2 {
    public:
        CGraphicsDeviceEmscripten();
        virtual ~CGraphicsDeviceEmscripten();

    public:
        virtual IZ_BOOL Reset(const void* initialParam);
    };

    CGraphicsDevice* CGraphicsDevice::s_Instance = IZ_NULL;

    // �C���X�^���X�쐬
    CGraphicsDevice* CGraphicsDevice::CreateGraphicsDevice(IMemoryAllocator* allocator)
    {
        if (s_Instance != IZ_NULL) {
            // �쐬�ς݂Ȃ̂ŉ������Ȃ�
            return s_Instance;
        }

        IZ_ASSERT(allocator != IZ_NULL);

        IZ_BOOL result = IZ_TRUE;
        IZ_UINT8* buf = IZ_NULL;
        CGraphicsDeviceEmscripten* instance = IZ_NULL;

        // �������m��
        buf = (IZ_UINT8*)ALLOC_ZERO(allocator, sizeof(CGraphicsDeviceEmscripten));
        VGOTO(buf != IZ_NULL, __EXIT__);

        // �C���X�^���X�쐬
        instance = new(buf) CGraphicsDeviceEmscripten;
        {
            instance->m_Allocator = allocator;
            instance->AddRef();
        }

        if (!result) {
            goto __EXIT__;
        }

        s_Instance = instance;
    
    __EXIT__:
        if (!result) {
            if (instance != IZ_NULL) {
                SAFE_RELEASE(instance);
            }
            else if (buf != IZ_NULL) {
                allocator->Free(buf);
            }
        }

        return instance;
    }

    // �R���X�g���N�^
    CGraphicsDeviceEmscripten::CGraphicsDeviceEmscripten()
    {
    }

    // �f�X�g���N�^
    CGraphicsDeviceEmscripten::~CGraphicsDeviceEmscripten()
    {
    }

    // ���Z�b�g
    IZ_BOOL CGraphicsDeviceEmscripten::Reset(const void* initialParam)
    {
        const SGraphicsDeviceInitParams& param = *(reinterpret_cast<const SGraphicsDeviceInitParams*>(initialParam));

        // �{�̍쐬
        IZ_BOOL ret = CreateBody(param);

        if (ret) {
            // 2D�`�揉����
            m_2DRenderer = C2DRenderer::Create2DRenderer(this, m_Allocator);
            VRETURN(m_2DRenderer != IZ_NULL);

            // FBO
            m_FBO = CGraphicsDeviceGLES2::CreateFBO(this, m_Allocator);
            VRETURN(m_FBO != IZ_NULL);
        }

        if (ret) {
            // NOTE
            // izanagi�ł͍�����W�n�Ȃ̂�
            // �J�����O�̕W����CounterClockWise�ɂ���
            CALL_GLES2_API(::glFrontFace(GL_CCW));

            // �r���[�|�[�g
            SViewport vp;
            {
                vp.x = 0;
                vp.y = 0;
                vp.width = param.screenWidth;
                vp.height = param.screenHeight;
                vp.minZ = 0.0f;
                vp.maxZ = 1.0f;
            }
            SetViewport(vp);

            // �����_�[�X�e�[�g�̌��ݒl���擾
            m_RenderState.GetParamsFromGraphicsDevice(this);

            // �X�e�[�g
            // �����ݒ�
            m_Flags.is_force_set_state = IZ_TRUE;
            SetDefaultRenderState();
            m_Flags.is_force_set_state = IZ_FALSE;

            // DummyRenderTarget
            {
                m_RT = CGraphicsDeviceGLES2::CreateDummyRenderTarget(
                    m_Allocator,
                    param.screenWidth, param.screenHeight);
                VRETURN(m_RT != IZ_NULL);

                m_Depth = CGraphicsDeviceGLES2::CreateDummyRenderTarget(
                    m_Allocator,
                    param.screenWidth, param.screenHeight);
                VRETURN(m_Depth != IZ_NULL);
                
                // ���݂̃����_�[�^�[�Q�b�g�Ƃ��ĕێ�
                SAFE_REPLACE(m_RenderState.curRT[0], m_RT);
                SAFE_REPLACE(m_RenderState.curDepth, m_Depth);
            }
        }

        return ret;
    }

    // �{�̍쐬
    IZ_BOOL CGraphicsDeviceGLES2::CreateBody(const SGraphicsDeviceInitParams& sParams)
    {
        m_ScreenWidth = sParams.screenWidth;
        m_ScreenHeight = sParams.screenHeight;

        return IZ_TRUE;
    }

    void CGraphicsDeviceGLES2::OnTerminate()
    {
        // Nothing is done...
    }

    // ����
    IZ_BOOL CGraphicsDeviceGLES2::Present()
    {
        glutSwapBuffers();
        return IZ_TRUE;
    }
}   // namespace graph
}   // namespace izanagi