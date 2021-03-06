#include "graph/gles2/RenderTarget_GLES2.h"
#include "graph/ParamValueConverter.h"
#include "graph/gles2/GraphicsDevice_GLES2.h"
#include "graph/gles2/Texture_GLES2.h"
#include "graph/GraphUtil.h"

namespace izanagi
{
namespace graph
{
    // コンストラクタ
    CRenderTargetGLES2::CRenderTargetGLES2()
    {
        m_Texture = 0;
    }

    // デストラクタ
    CRenderTargetGLES2::~CRenderTargetGLES2()
    {
        if (m_Texture > 0) {
            if (GetPixelFormat() == E_GRAPH_PIXEL_FMT_D24S8) {
                CALL_GL_API(::glDeleteRenderbuffers(1, &m_Texture));
            }
            else {
                CALL_GL_API(::glDeleteTextures(1, &m_Texture));
            }
        }

		FREE(m_Allocator, m_TemporaryData);
		m_allocatedSize = 0;

        SAFE_RELEASE(m_Device);
    }

    // レンダーターゲット作成
    CRenderTargetGLES2* CRenderTargetGLES2::CreateRenderTarget(
        CGraphicsDeviceGLES2* device,
        IMemoryAllocator* allocator,
        IZ_UINT width, 
        IZ_UINT height,
        E_GRAPH_PIXEL_FMT fmt)
    {
        IZ_ASSERT(device != IZ_NULL);

        IZ_BOOL result = IZ_TRUE;
        IZ_UINT8* buf = IZ_NULL;
        CRenderTargetGLES2* instance = IZ_NULL;

        size_t size = sizeof(CRenderTargetGLES2);

        // メモリ確保
        buf = (IZ_UINT8*)ALLOC_ZERO(allocator, size);
        result = (buf != IZ_NULL);
        VGOTO(result, __EXIT__);

        // インスタンス作成
        instance = new (buf) CRenderTargetGLES2;
        {
            instance->m_Allocator = allocator;
            SAFE_REPLACE(instance->m_Device, device);

            instance->AddRef();
        }

        // 本体作成
        result = instance->CreateBody_RenderTarget(
            width, height,
            fmt);
        VGOTO(result, __EXIT__);

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

    CRenderTargetGLES2* CRenderTargetGLES2::CreateDepthStencilRenderTarget(
        CGraphicsDeviceGLES2* device,
        IMemoryAllocator* allocator,
        IZ_UINT width, 
        IZ_UINT height)
    {
        IZ_ASSERT(device != IZ_NULL);

        IZ_BOOL result = IZ_TRUE;
        IZ_UINT8* buf = IZ_NULL;
        CRenderTargetGLES2* instance = IZ_NULL;

        size_t size = sizeof(CRenderTargetGLES2);

        // メモリ確保
        buf = (IZ_UINT8*)ALLOC_ZERO(allocator, size);
        result = (buf != IZ_NULL);
        VGOTO(result, __EXIT__);

        // インスタンス作成
        instance = new (buf) CRenderTargetGLES2;
        {
            instance->m_Allocator = allocator;
            SAFE_REPLACE(instance->m_Device, device);

            instance->AddRef();
        }

        // 本体作成
        result = instance->CreateBody_DepthStencilRenderTarget(width, height);
        VGOTO(result, __EXIT__);

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

    CRenderTargetGLES2* CRenderTargetGLES2::CreateDummyRenderTarget(
        IMemoryAllocator* allocator,
        IZ_UINT width,
        IZ_UINT height)
    {
        IZ_BOOL result = IZ_TRUE;
        IZ_UINT8* buf = IZ_NULL;
        CRenderTargetGLES2* instance = IZ_NULL;

        size_t size = sizeof(CRenderTargetGLES2);

        // メモリ確保
        buf = (IZ_UINT8*)ALLOC_ZERO(allocator, size);
        result = (buf != IZ_NULL);
        VGOTO(result, __EXIT__);

        // インスタンス作成
        instance = new (buf) CRenderTargetGLES2;
        {
            instance->m_Allocator = allocator;

            instance->AddRef();
        }

        // 情報をセット
        // PixelFormatはダミー
        instance->SetTextureInfo(
            width, height,
            E_GRAPH_PIXEL_FMT_RGBA8);

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

    // 本体作成（レンダーターゲット）
    IZ_BOOL CRenderTargetGLES2::CreateBody_RenderTarget(
        IZ_UINT width, 
        IZ_UINT height,
        E_GRAPH_PIXEL_FMT fmt)
    {
        CALL_GL_API(::glGenTextures(1, &m_Texture));
        VRETURN(m_Texture > 0);

        // コンストラクタで現在のテクスチャを保持しつつ設定したいテクスチャにバインド、デストラクタで元に戻す.
        CTextureOperator texOp(m_Device, m_Texture);

        GLenum glInternal, glFormat, glType;

        CTargetParamValueConverter::ConvAbstractToTarget_PixelFormat(
            fmt,
            glInternal,
            glFormat,
            glType);

        CALL_GL_API(::glTexImage2D(
                GL_TEXTURE_2D,
                0,
                glInternal,
                width, height,
                0,
                glFormat,
                glType,
                IZ_NULL));

        SetTextureInfo(width, height, fmt);

        return IZ_TRUE;
    }

    IZ_BOOL CRenderTargetGLES2::CreateBody_DepthStencilRenderTarget(
        IZ_UINT width,
        IZ_UINT height)
    {
#if 0
        // TODO
        // Stencil

        CALL_GL_API(::glGenRenderbuffers(1, &m_Texture));
        VRETURN(m_Texture > 0);

        CALL_GL_API(::glBindRenderbuffer(GL_RENDERBUFFER, m_Texture));

        CALL_GL_API(::glRenderbufferStorage(
            GL_RENDERBUFFER,
            GL_DEPTH_COMPONENT16,
            width, height));

        // 戻す
        CALL_GL_API(::glBindRenderbuffer(GL_RENDERBUFFER, 0));

        SetTextureInfo(width, height, E_GRAPH_PIXEL_FMT_D24S8);
#else
        // NOTE
        // http://gamedev.stackexchange.com/questions/3082/getting-a-texture-from-a-renderbuffer-in-opengl
        // 一番下の回答
        // Higher than OpenGL3.2

        CALL_GL_API(::glGenTextures(1, &m_Texture));
        VRETURN(m_Texture > 0);

        // コンストラクタで現在のテクスチャを保持しつつ設定したいテクスチャにバインド、デストラクタで元に戻す.
        CTextureOperator texOp(m_Device, m_Texture);

        CALL_GL_API(::glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_DEPTH_COMPONENT,
            width, height,
            0,
            GL_DEPTH_COMPONENT,
            GL_UNSIGNED_INT,
            IZ_NULL));

        // TODO
        SetTextureInfo(width, height, E_GRAPH_PIXEL_FMT_RGBA8);
#endif

        return IZ_TRUE;
    }

    // テクスチャ情報取得
    void CRenderTargetGLES2::SetTextureInfo(
        IZ_UINT width,
        IZ_UINT height,
        E_GRAPH_PIXEL_FMT fmt)
    {
        //IZ_ASSERT(m_Texture != 0);

        m_TexInfo.width = width;
        m_TexInfo.height = height;
    
        m_TexInfo.level = 1;
        m_TexInfo.fmt = fmt;

        m_TexInfo.usage = E_GRAPH_RSC_USAGE_STATIC;

        m_TexInfo.is_rendertarget = IZ_TRUE;
        m_TexInfo.is_dynamic = IZ_FALSE;
        m_TexInfo.is_on_sysmem = IZ_FALSE;
        m_TexInfo.is_on_vram = IZ_TRUE;
    }

    IZ_BOOL CRenderTargetGLES2::IsPrepared() const
    {
        return (m_Texture != IZ_NULL);
    }

    // 本体解放
    IZ_BOOL CRenderTargetGLES2::Disable()
    {
        return IZ_TRUE;
    }

    // リセット
    IZ_BOOL CRenderTargetGLES2::Restore()
    {
        return IZ_TRUE;
    }

	// ロック
	IZ_UINT CRenderTargetGLES2::Lock(
		IZ_UINT level,
		void** data,
		IZ_BOOL isReadOnly,
		IZ_BOOL isDiscard/*= IZ_FALSE*/)
	{
		if (isReadOnly) {
			auto width = GetWidth(level);
			auto height = GetHeight(level);

			auto fmt = GetPixelFormat();
			auto bpp = CGraphUtil::GetBPP(fmt);
			auto pitch = width * bpp;
			auto size = pitch * GetHeight(level);

			if (m_TemporaryData == IZ_NULL) {
				m_TemporaryData = ALLOC(m_Allocator, size);
			}
			else if (size > m_allocatedSize) {
				m_TemporaryData = REALLOC(m_Allocator, m_TemporaryData, size);
			}

			IZ_ASSERT(m_TemporaryData);

			m_allocatedSize = size;

			GLenum glInternal, glFormat, glType;

			CTargetParamValueConverter::ConvAbstractToTarget_PixelFormat(
				fmt,
				glInternal,
				glFormat,
				glType);

			// コンストラクタで現在のテクスチャを保持しつつ設定したいテクスチャにバインド、デストラクタで元に戻す.
			CTextureOperator texOp(m_Device, m_Texture);

			CALL_GL_API(::glGetTexImage(
				GL_TEXTURE_2D,
				level,
				glFormat,
				glType,
				m_TemporaryData));

			*data = m_TemporaryData;

			return pitch;
		}
		else {
			// Not support...
			IZ_ASSERT(false);
			return 0;
		}
	}

	// アンロック
	IZ_BOOL CRenderTargetGLES2::Unlock(IZ_UINT level)
	{
		// TODO
		// Support only for reading...
		return IZ_TRUE;
	}
}   // namespace graph
}   // namespace izanagi
