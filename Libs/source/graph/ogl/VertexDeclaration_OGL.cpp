#include "graph/ogl/VertexDeclaration_OGL.h"
#include "graph/ogl/VertexBuffer_OGL.h"
#include "graph/gles2/ShaderProgram_GLES2.h"

// NOTE
// OpenGL で インスタンシング描画.
// http://blog.techlab-xe.net/archives/2870

// NOTE
// Vertex Array Object
// http://googledevjp.blogspot.jp/2015/11/blog-post.html

// NOTE
// glDrawElements() は glBindBuffer( GL_ARRAY_BUFFER ) を参照しないしVAO も glBindBuffer(GL_ARRAY_BUFFER) に影響を与えません.
// ですが、glDrawElements() は glBindBuffer(GL_ELEMENT_ARRAY_BUFFER) を参照し、VAO は glBindBuffer(GL_ELEMENT_ARRAY_BUFFER) を置き換えます.
// http://wlog.flatlib.jp/item/1629

// NOTE
// https://shikihuiku.wordpress.com/2014/02/10/opengl%E3%81%AEdrawcall%E3%81%AB%E3%81%A4%E3%81%84%E3%81%A6/

namespace izanagi
{
namespace graph
{
    // インスタンス作成
    CVertexDeclaration* CVertexDeclarationOGL::CreateVertexDeclaration(
        IMemoryAllocator* allocator,
        const SVertexElement* elem,
        IZ_UINT elemNum)
    {
        size_t size = sizeof(CVertexDeclarationOGL);
        size += sizeof(SVertexElement) * elemNum;

        IZ_UINT8* buf = (IZ_UINT8*)ALLOC(allocator, size);
        VRETURN_NULL(buf != IZ_NULL);

        CVertexDeclarationOGL* instance = new (buf)CVertexDeclarationOGL;
        {
            instance->AddRef();

            instance->m_Allocator = allocator;

            buf += sizeof(CVertexDeclarationOGL);

            instance->m_Elements = reinterpret_cast<SVertexElement*>(buf);
            memcpy(
                instance->m_Elements,
                elem,
                sizeof(SVertexElement) * elemNum);

            instance->m_ElemNum = elemNum;

#ifdef USE_VAO
            CALL_GL_API(::glGenVertexArrays(1, &instance->m_vao));
#endif
        }

        return instance;
    }

    CVertexDeclarationOGL::CVertexDeclarationOGL()
    {
        FILL_ZERO(m_vbs, sizeof(m_vbs));
    }

    CVertexDeclarationOGL::~CVertexDeclarationOGL()
    {
        for (IZ_UINT i = 0; i < COUNTOF(m_vbs); i++) {
            SAFE_RELEASE(m_vbs[i]);
        }
    }

    namespace {
        using ParamsByElemetType = std::tuple < IZ_BOOL, IZ_UINT, GLenum, size_t > ;

        ParamsByElemetType getParamsByElemetType(izanagi::graph::E_GRAPH_VTX_DECL_TYPE elemType)
        {
            IZ_BOOL needNormalized = IZ_FALSE;
            IZ_UINT num = 4;
            GLenum type = GL_BYTE;
            size_t size = 4;

            switch (elemType)
            {
            case E_GRAPH_VTX_DECL_TYPE_FLOAT1:
                size = sizeof(GLfloat);
                type = GL_FLOAT;
                num = 1;
                break;
            case E_GRAPH_VTX_DECL_TYPE_FLOAT2:
                size = sizeof(GLfloat);
                type = GL_FLOAT;
                num = 2;
                break;
            case E_GRAPH_VTX_DECL_TYPE_FLOAT3:
                size = sizeof(GLfloat);
                type = GL_FLOAT;
                num = 3;
                break;
            case E_GRAPH_VTX_DECL_TYPE_FLOAT4:
                size = sizeof(GLfloat);
                type = GL_FLOAT;
                num = 4;
                break;
            case E_GRAPH_VTX_DECL_TYPE_COLOR:
                size = sizeof(GLbyte);
                type = GL_UNSIGNED_BYTE;
                num = 4;
                needNormalized = IZ_TRUE;
                break;
            case E_GRAPH_VTX_DECL_TYPE_UBYTE4:
                size = sizeof(GLbyte);
                type = GL_UNSIGNED_BYTE;
                num = 4;
                break;
            case E_GRAPH_VTX_DECL_TYPE_SHORT2:
                size = sizeof(GLshort);
                type = GL_SHORT;
                num = 2;
                break;
            case E_GRAPH_VTX_DECL_TYPE_SHORT4:
                size = sizeof(GLshort);
                type = GL_SHORT;
                num = 2;
                break;
            case E_GRAPH_VTX_DECL_TYPE_UBYTE4N:
                size = sizeof(GLbyte);
                type = GL_UNSIGNED_BYTE;
                num = 4;
                needNormalized = IZ_TRUE;
                break;
            case E_GRAPH_VTX_DECL_TYPE_SHORT2N:
                size = sizeof(GLshort);
                type = GL_SHORT;
                num = 2;
                needNormalized = IZ_TRUE;
                break;
            case E_GRAPH_VTX_DECL_TYPE_SHORT4N:
                size = sizeof(GLshort);
                type = GL_SHORT;
                num = 4;
                needNormalized = IZ_TRUE;
                break;
            case E_GRAPH_VTX_DECL_TYPE_USHORT2N:
                size = sizeof(GLshort);
                type = GL_UNSIGNED_SHORT;
                num = 2;
                needNormalized = IZ_TRUE;
                break;
            case E_GRAPH_VTX_DECL_TYPE_USHORT4N:
                size = sizeof(GLshort);
                type = GL_UNSIGNED_SHORT;
                num = 4;
                needNormalized = IZ_TRUE;
                break;
            case E_GRAPH_VTX_DECL_TYPE_FLOAT16_4:
                size = sizeof(GLshort);
                type = GL_HALF_FLOAT_ARB;
                num = 4;
                break;
            default:
                IZ_ASSERT(IZ_FALSE);
                size = 0;
                break;
            }

            ParamsByElemetType ret(needNormalized, num, type, size);

            return ret;
        }
    }

#ifdef USE_VAO
    IZ_BOOL CVertexDeclarationOGL::begin(
        CGraphicsDevice* device,
        CShaderProgramGLES2* program,
        IZ_UINT vtxOffset,
        CVertexBufferOGL* vb,
        IZ_UINT vtxStride)
    {
        if (!m_isForceUpdate) {
            IZ_ASSERT(m_vao > 0);
            CALL_GL_API(::glBindVertexArray(m_vao));
        }

        if (vb) {
            if (m_vbs[0] != vb) {
                m_isApplied = IZ_FALSE;
                SAFE_REPLACE(m_vbs[0], vb);
            }
            CALL_GL_API(::glBindBuffer(GL_ARRAY_BUFFER, vb->GetRawInterface()));
            vb->Initialize(device);
        }

        if (m_isApplied) {
            if (m_prevVtxOffset != vtxOffset) {
                m_isApplied = IZ_FALSE;
            }
			else if (program != m_prevProgram) {
				m_isApplied = IZ_FALSE;
			}
        }

		m_prevProgram = program;

        if (!m_isApplied || m_isForceUpdate) {
            for (IZ_UINT i = 0; i < m_ElemNum; i++) {
                const char* attribName = GetAttribName(i);
                IZ_INT attribIndex = program->GetAttribIndex(attribName);

                if (attribIndex >= 0) {
                    const SVertexElement& element = m_Elements[i];

                    // NOTE
                    // VBOを使う場合は常に１ストリームのみ
                    IZ_ASSERT(element.Stream == 0);

                    // NOTE
                    // Semanticによる頂点データの位置は無視するm_Elementsの並び順通りに設定する

                    E_GRAPH_VTX_DECL_TYPE elemType = element.Type;
                    if (element.Usage == E_GRAPH_VTX_DECL_USAGE_POSITION
                        && elemType != E_GRAPH_VTX_DECL_TYPE_FLOAT16_4)
                    {
                        elemType = E_GRAPH_VTX_DECL_TYPE_FLOAT3;
                    }

                    auto params = getParamsByElemetType(elemType);

                    IZ_BOOL needNormalized = std::get<0>(params);
                    IZ_UINT num = std::get<1>(params);
                    GLenum type = std::get<2>(params);
                    size_t size = std::get<3>(params);

                    if (size == 0) {
                        IZ_ASSERT(IZ_FALSE);
                        continue;
                    }

                    // NOTE
                    // glVertexAttribPointer に渡すオフセットはバイト数
                    // しかし、ここにわたってくるオフセットは頂点位置のオフセット
                    // そのため、バイトオフセットに変換する
                    IZ_UINT offset = vtxOffset * vtxStride;

                    CALL_GL_API(::glEnableVertexAttribArray(attribIndex));

                    CALL_GL_API(::glVertexAttribPointer(
                        attribIndex,
                        num,
                        type,
                        needNormalized ? GL_TRUE : GL_FALSE,
                        vtxStride,
                        (void*)(offset + element.Offset)));
                }
            }

            m_isApplied = IZ_TRUE;
        }

        m_prevVtxOffset = vtxOffset;

        return IZ_TRUE;
    }

    IZ_BOOL CVertexDeclarationOGL::beginInstancing(
        CShaderProgramGLES2* program,
        InstancingParam* params,
        CVertexBuffer** vbs)
    {
        if (!m_isForceUpdate) {
            IZ_ASSERT(m_vao > 0);
            CALL_GL_API(::glBindVertexArray(m_vao));
        }

        IZ_UINT curStream = IZ_UINT32_MAX;

        for (IZ_UINT i = 0; i < m_ElemNum; i++) {
            const char* attribName = GetAttribName(i);
            IZ_INT attribIndex = program->GetAttribIndex(attribName);

            if (attribIndex >= 0) {
                const SVertexElement& element = m_Elements[i];

                auto stream = element.Stream;

                auto& param = params[stream];

                if (vbs[stream]) {
                    if (m_vbs[stream] != vbs[stream]) {
                        m_isApplied = IZ_FALSE;
                        SAFE_REPLACE(m_vbs[stream], vbs[stream]);
                    }
                    auto handleVB = ((CVertexBufferOGL*)vbs[stream])->GetRawInterface();

                    if (curStream != stream) {
                        CALL_GL_API(::glBindBuffer(GL_ARRAY_BUFFER, handleVB));
                    }
                }

                curStream = stream;

                if ((!m_isApplied || m_isForceUpdate)
                    && vbs[stream])
                {
                    // NOTE
                    // Semanticによる頂点データの位置は無視するm_Elementsの並び順通りに設定する

                    E_GRAPH_VTX_DECL_TYPE elemType = element.Type;
                    if (element.Usage == E_GRAPH_VTX_DECL_USAGE_POSITION) {
                        elemType = E_GRAPH_VTX_DECL_TYPE_FLOAT3;
                    }

                    auto params = getParamsByElemetType(elemType);

                    IZ_BOOL needNormalized = std::get<0>(params);
                    IZ_UINT num = std::get<1>(params);
                    GLenum type = std::get<2>(params);
                    size_t size = std::get<3>(params);

                    if (size == 0) {
                        IZ_ASSERT(IZ_FALSE);
                        continue;
                    }

                    // NOTE
                    // glVertexAttribPointer に渡すオフセットはバイト数
                    // しかし、ここにわたってくるオフセットは頂点位置のオフセット
                    // そのため、バイトオフセットに変換する
                    IZ_UINT offset = param.offset * param.stride;

                    CALL_GL_API(::glVertexAttribPointer(
                        attribIndex,
                        num,
                        type,
                        needNormalized ? GL_TRUE : GL_FALSE,
                        param.stride,
                        (void*)(offset + element.Offset)));

                    CALL_GL_API(::glEnableVertexAttribArray(attribIndex));

                    // For instancing.
                    auto divisor = param.divisor;
                    CALL_GL_API(::glVertexAttribDivisor(attribIndex, divisor));
                }
            }
        }

        m_isApplied = IZ_TRUE;

        return IZ_TRUE;
    }
#else   // #ifdef USE_VAO
    IZ_BOOL CVertexDeclarationOGL::begin(
        CGraphicsDevice* device,
        CShaderProgramGLES2* program,
        IZ_UINT vtxOffset,
        CVertexBufferOGL* vb,
        IZ_UINT vtxStride)
    {
        if (vb) {
            CALL_GL_API(::glBindBuffer(GL_ARRAY_BUFFER, vb->GetRawInterface()));
            vb->Initialize(device);
        }

        for (IZ_UINT i = 0; i < m_ElemNum; i++) {
            const char* attribName = GetAttribName(i);
            IZ_INT attribIndex = program->GetAttribIndex(attribName);

            if (attribIndex >= 0) {
                const SVertexElement& element = m_Elements[i];

                // NOTE
                // VBOを使う場合は常に１ストリームのみ
                IZ_ASSERT(element.Stream == 0);

                // NOTE
                // Semanticによる頂点データの位置は無視するm_Elementsの並び順通りに設定する

                E_GRAPH_VTX_DECL_TYPE elemType = element.Type;
                if (element.Usage == E_GRAPH_VTX_DECL_USAGE_POSITION) {
                    elemType = E_GRAPH_VTX_DECL_TYPE_FLOAT3;
                }

                auto params = getParamsByElemetType(elemType);

                IZ_BOOL needNormalized = std::get<0>(params);
                IZ_UINT num = std::get<1>(params);
                GLenum type = std::get<2>(params);
                size_t size = std::get<3>(params);

                if (size == 0) {
                    IZ_ASSERT(IZ_FALSE);
                    continue;
                }

                // NOTE
                // glVertexAttribPointer に渡すオフセットはバイト数
                // しかし、ここにわたってくるオフセットは頂点位置のオフセット
                // そのため、バイトオフセットに変換する
                IZ_UINT offset = vtxOffset * vtxStride;

                CALL_GL_API(::glEnableVertexAttribArray(attribIndex));

                CALL_GL_API(::glVertexAttribPointer(
                    attribIndex,
                    num,
                    type,
                    needNormalized ? GL_TRUE : GL_FALSE,
                    vtxStride,
                    (void*)(offset + element.Offset)));
            }
        }

        return IZ_TRUE;
    }

    IZ_BOOL CVertexDeclarationOGL::beginInstancing(
        CShaderProgramGLES2* program,
        InstancingParam* params,
        CVertexBuffer** vbs)
    {
        IZ_UINT curStream = IZ_UINT32_MAX;

        for (IZ_UINT i = 0; i < m_ElemNum; i++) {
            const char* attribName = GetAttribName(i);
            IZ_INT attribIndex = program->GetAttribIndex(attribName);

            if (attribIndex >= 0) {
                const SVertexElement& element = m_Elements[i];

                auto stream = element.Stream;

                auto& param = params[stream];

                if (vbs[stream]) {
                    auto handleVB = ((CVertexBufferOGL*)vbs[stream])->GetRawInterface();

                    if (curStream != stream) {
                        CALL_GL_API(::glBindBuffer(GL_ARRAY_BUFFER, handleVB));
                    }
                }

                curStream = stream;

                if (vbs[stream]) {
                    // NOTE
                    // Semanticによる頂点データの位置は無視するm_Elementsの並び順通りに設定する

                    E_GRAPH_VTX_DECL_TYPE elemType = element.Type;
                    if (element.Usage == E_GRAPH_VTX_DECL_USAGE_POSITION) {
                        elemType = E_GRAPH_VTX_DECL_TYPE_FLOAT3;
                    }

                    auto params = getParamsByElemetType(elemType);

                    IZ_BOOL needNormalized = std::get<0>(params);
                    IZ_UINT num = std::get<1>(params);
                    GLenum type = std::get<2>(params);
                    size_t size = std::get<3>(params);

                    if (size == 0) {
                        IZ_ASSERT(IZ_FALSE);
                        continue;
                    }

                    // NOTE
                    // glVertexAttribPointer に渡すオフセットはバイト数
                    // しかし、ここにわたってくるオフセットは頂点位置のオフセット
                    // そのため、バイトオフセットに変換する
                    IZ_UINT offset = param.offset * param.stride;

                    CALL_GL_API(::glVertexAttribPointer(
                        attribIndex,
                        num,
                        type,
                        needNormalized ? GL_TRUE : GL_FALSE,
                        param.stride,
                        (void*)(offset + element.Offset)));

                    CALL_GL_API(::glEnableVertexAttribArray(attribIndex));

                    // For instancing.
                    auto divisor = param.divisor;
                    CALL_GL_API(::glVertexAttribDivisor(attribIndex, divisor));
                }
            }
        }

        return IZ_TRUE;
    }
#endif  // #ifdef USE_VAO

    void CVertexDeclarationOGL::end()
    {
#ifdef USE_VAO
        CALL_GL_API(::glBindVertexArray(0));
#endif
    }
}   // namespace graph
}   // namespace izanagi
