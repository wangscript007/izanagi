#include "graph/GraphUtil.h"
#include "graph/gles2/Texture_GLES2.h"
#include "graph/ParamValueConverter.h"
#include "graph/gles2/GraphicsDevice_GLES2.h"

namespace izanagi
{
namespace graph
{
    // データからテクスチャ作成
    CTextureGLES2* CTextureGLES2::CreateTextureFromMemory(
        CGraphicsDeviceGLES2* device,
        IMemoryAllocator* allocator,
        const void* data,
        IZ_UINT dataSize,
        E_GRAPH_PIXEL_FMT fmt)
    {
        // TODO
        return IZ_FALSE;
    }

    // コンストラクタ
    CTextureGLES2::CTextureGLES2()
    {
        m_Device = IZ_NULL;

        m_Texture = 0;

        m_IsInitialized = IZ_FALSE;

        m_Size = 0;

        m_LockedSize = 0;
        m_LockedLevel = -1;

        m_TemporaryData = IZ_NULL;

        m_GLFormat = GL_RGBA;
        m_GLType = GL_UNSIGNED_BYTE;
    }

    // デストラクタ
    CTextureGLES2::~CTextureGLES2()
    {
        SAFE_RELEASE(m_Device);

        CALL_GL_API(::glDeleteTextures(1, &m_Texture));

        FREE(m_Allocator, m_TemporaryData);
    }

    // テクスチャ作成
    CTextureGLES2* CTextureGLES2::CreateTexture(
        CGraphicsDeviceGLES2* device,
        IMemoryAllocator* allocator,
        IZ_UINT width,
        IZ_UINT height,
        IZ_UINT mipLevel,
        E_GRAPH_PIXEL_FMT fmt,
        E_GRAPH_RSC_USAGE rscType)
    {
        IZ_BOOL result = IZ_TRUE;
        IZ_UINT8* buf = IZ_NULL;
        CTextureGLES2* instance = IZ_NULL;

        // 最大のミップレベルを計算
        IZ_UINT maxMipLevel = 1;
        {
            IZ_UINT nW = width;
            IZ_UINT nH = height;
            while ((nW > 1) && (nH > 1)) {
                nW >>= 1;
                nH >>= 1;
                ++maxMipLevel;
            }
        }

        size_t size = sizeof(CTextureGLES2);

        if (mipLevel <= maxMipLevel) {
            mipLevel = (mipLevel == 0 ? maxMipLevel : mipLevel);
        }
        else {
            mipLevel = 1;
        }

        // メモリ確保
        buf = (IZ_UINT8*)ALLOC_ZERO(allocator, size);
        IZ_UINT8* top = buf;

        result = (buf != IZ_NULL);
        VGOTO(result, __EXIT__);

        // インスタンス作成
        instance = new (buf)CTextureGLES2;
        {
            buf += sizeof(CTextureGLES2);

            instance->AddRef();
            instance->m_Allocator = allocator;

            SAFE_REPLACE(instance->m_Device, device);
        }

        IZ_ASSERT(CStdUtil::GetPtrDistance(top, buf) == size);

        // 本体作成
        result = instance->CreateBody_Texture(
                    width,
                    height,
                    mipLevel,
                    fmt,
                    rscType);
        if (!result) {
            goto __EXIT__;
        }

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

    // 本体作成（テクスチャ）
    IZ_BOOL CTextureGLES2::CreateBody_Texture(
        IZ_UINT width,
        IZ_UINT height,
        IZ_UINT mipLevel,
        E_GRAPH_PIXEL_FMT fmt,
        E_GRAPH_RSC_USAGE rscType)
    {
        CALL_GL_API(::glGenTextures(1, &m_Texture));
        VRETURN(m_Texture > 0);

        SetTextureInfo(
            IZ_FALSE,
            width, height,
            mipLevel,
            fmt, rscType);

        CTextureOperator texOp(m_Device, m_Texture);

        Initialize();

        return IZ_TRUE;
    }

    // テクスチャ情報取得
    void CTextureGLES2::SetTextureInfo(
        IZ_BOOL isRT,
        IZ_UINT width,
        IZ_UINT height,
        IZ_UINT mipLevel,
        E_GRAPH_PIXEL_FMT fmt,
        E_GRAPH_RSC_USAGE usage)
    {
        IZ_ASSERT(m_Texture != 0);

        auto isCompressed = CGraphUtil::IsCompressedPixelFormat(fmt);

        m_TexInfo.width = width;
        m_TexInfo.height = height;
    
        // TODO
        m_TexInfo.level = isCompressed ? 1 : mipLevel;

        m_TexInfo.fmt = fmt;

        m_TexInfo.usage = usage;

        m_TexInfo.is_rendertarget = isRT;
        m_TexInfo.is_dynamic = (usage == E_GRAPH_RSC_USAGE_DYNAMIC);
        m_TexInfo.is_on_sysmem = IZ_FALSE;
        m_TexInfo.is_on_vram = isRT;


        if (isCompressed) {
            if (fmt == E_GRAPH_PIXEL_FMT_DXT1) {
                // RGBA 1/8
                m_Size = (width * height) >> 1;
            }
            else {
                // RGBA 1/4
                m_Size = width * height;
            }
        }
        else {
            IZ_UINT bpp = CGraphUtil::GetBPP(fmt);

            m_Size = width * height * bpp;
        }

        CTargetParamValueConverter::ConvAbstractToTarget_PixelFormat(
            fmt,
            m_GLInternal,
            m_GLFormat,
            m_GLType);
    }

    void CTextureGLES2::Initialize()
    {
        if (!m_IsInitialized) {
            GLuint width = GetWidth();
            GLuint height = GetHeight();

            auto isCompressed = CGraphUtil::IsCompressedPixelFormat(m_TexInfo.fmt);

            if (isCompressed) {
                CALL_GL_API(::glCompressedTexImage2D(
                    GL_TEXTURE_2D,
                    0,
                    m_GLFormat,
                    width,
                    height,
                    0,
                    m_Size,
                    IZ_NULL));
            }
            else {
                for (IZ_UINT i = 0; i < m_TexInfo.level; i++) {
                    GLuint w = width >> i;
                    GLuint h = height >> i;

                    CALL_GL_API(::glTexImage2D(
                        GL_TEXTURE_2D,
                        i,
                        m_GLInternal,
                        w, h,
                        0,
                        m_GLFormat,
                        m_GLType,
                        IZ_NULL));
                }
            }

            m_IsInitialized = IZ_TRUE;
        }
    }

    /**
    * ロック
    */
    IZ_UINT CTextureGLES2::Lock(
        IZ_UINT level,
        void** data,
        IZ_BOOL isReadOnly,
        IZ_BOOL isDiscard/*= IZ_FALSE*/)
    {
		// TODO
		// Not support read...

        IZ_ASSERT(m_Texture != 0);

        // レベル指定
        VRETURN_VAL(level < m_TexInfo.level, 0);

        IZ_UINT width = GetWidth(level);

#if 0
        IZ_UINT bpp = CGraphUtil::GetBPP(GetPixelFormat());

        IZ_UINT pitch = width * bpp;

        // TODO
        // Align

        size_t size = pitch * GetHeight(level);
#else
        auto fmt = GetPixelFormat();

        IZ_UINT bpp = 0;
        IZ_UINT pitch = 0;
        size_t size = 0;

        if (CGraphUtil::IsCompressedPixelFormat(fmt)) {
            IZ_ASSERT(level == 0);

            auto height = GetHeight(level);

            IZ_ASSERT((width & 0x03) == 0);
            IZ_ASSERT((height & 0x03) == 0);

            // NOTE
            // DX9合わせにする.
            // https://msdn.microsoft.com/ja-jp/library/cc323934.aspx
            // DXTn フォーマットのピッチは、ブロック 1 行のバイト数を指す.
            // たとえば、幅が 16 である場合、ピッチは 4 ブロックになる (DXT1 の場合は 4*8、DXT2-5 の場合は 4*16).

            if (fmt == E_GRAPH_PIXEL_FMT_DXT1) {
                pitch = (width >> 2) * 8;
            }
            else {
                pitch = (width >> 2) * 16;
            }

            size = pitch * (height >> 2);
        }
        else {
            bpp = CGraphUtil::GetBPP(fmt);
            pitch = width * bpp;
            size = pitch * GetHeight(level);
        }
#endif

        if (m_TemporaryData == IZ_NULL) {
            m_TemporaryData = ALLOC(m_Allocator, size);
        }
        else if (size > m_Size) {
            m_TemporaryData = REALLOC(m_Allocator, m_TemporaryData, size);
        }

        VRETURN(m_TemporaryData != IZ_NULL);

        m_LockedSize = size;
        m_LockedLevel = level;

        *data = m_TemporaryData;

        return pitch;
    }

    /**
    * アンロック
    */
    IZ_BOOL CTextureGLES2::Unlock(IZ_UINT level)
    {
        VRETURN(m_LockedLevel == level);

        IZ_BOOL isLocked = (m_LockedSize > 0);

        if (isLocked) {
            CTextureOperator texOp(m_Device, m_Texture);

            if (texOp != this) {
                Initialize();
            }
            
            IZ_UINT width = GetWidth(level);
            IZ_UINT height = GetHeight(level);

            CALL_GL_API(::glTexSubImage2D(
                GL_TEXTURE_2D,
                level,
                0, 0,
                width, height,
                m_GLFormat,
                m_GLType,
                m_TemporaryData));

            m_LockedSize = 0;
        }

#if 0
        if (!IsDynamic()) {
            FREE(m_Allocator, m_TemporaryData);
            m_TemporaryData = IZ_NULL;
        }
#endif

        m_LockedLevel = -1;

        return IZ_TRUE;
    }

    IZ_BOOL CTextureGLES2::Write(
        IZ_UINT level,
        void* data,
        IZ_UINT x, IZ_UINT y,
        IZ_UINT width, IZ_UINT height)
    {
        IZ_ASSERT(m_Texture != 0);

        // Check not locked.
        IZ_ASSERT(m_LockedSize == 0);

        IZ_ASSERT(x < width);
        IZ_ASSERT(y < height);

        CTextureOperator texOp(m_Device, m_Texture);

        if (texOp != this) {
            Initialize();
        }

        IZ_UINT w = GetWidth(level);
        IZ_UINT h = GetHeight(level);

        VRETURN(width <= w);
        VRETURN(height <= h);

        CALL_GL_API(::glTexSubImage2D(
            GL_TEXTURE_2D,
            level,
            x, y,
            width, height,
            m_GLFormat,
            m_GLType,
            data));

        return IZ_TRUE;
    }

    IZ_BOOL CTextureGLES2::IsPrepared() const
    {
        return (m_Texture > 0);
    }

    // 本体解放
    IZ_BOOL CTextureGLES2::Disable()
    {
        CALL_GL_API(::glDeleteTextures(1, &m_Texture));
        m_Texture = 0;

        return IZ_TRUE;
    }

    // リセット
    IZ_BOOL CTextureGLES2::Restore()
    {
        // Nothing is done...

        return IZ_TRUE;
    }

    /////////////////////////////////////////////////

    CTextureOperator::CTextureOperator(CGraphicsDeviceGLES2* device, GLuint targetTexHandle)
    {
        m_CurTex = device->GetTexture(0);
        m_TargetTexHandle = targetTexHandle;

        if (m_CurTex == IZ_NULL
            || m_CurTex->GetTexHandle() != targetTexHandle)
        {
            CALL_GL_API(::glBindTexture(GL_TEXTURE_2D, targetTexHandle));
        }
    }

    CTextureOperator::~CTextureOperator()
    {
        if (m_CurTex == IZ_NULL) {
            CALL_GL_API(::glBindTexture(GL_TEXTURE_2D, 0));
        }
        else {
            CALL_GL_API(::glBindTexture(
                m_CurTex->GetTexType() == E_GRAPH_TEX_TYPE_PLANE ? GL_TEXTURE_2D : GL_TEXTURE_CUBE_MAP,
                m_CurTex->GetTexHandle()));
        }
    }

}   // namespace graph
}   // namespace izanagi
