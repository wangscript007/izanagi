#if !defined(__IZANAGI_GRAPH_SHADER_PROXY_GLES2_H__)
#define __IZANAGI_GRAPH_SHADER_PROXY_GLES2_H__

#include "graph/GraphDefs.h"

namespace izanagi
{
namespace graph
{
    // シェーダ定数テーブル
    template <class _SHADER_BASE>
    class CShaderProxy : public _SHADER_BASE {
    protected:
        // コンストラクタ
        CShaderProxy()
        {
            m_Shader = 0;
        }
        virtual ~CShaderProxy()
        {
            if (m_Shader != 0) {
                CALL_GL_API(::glDeleteShader(m_Shader));
                m_Shader = 0;
            }
        }

    protected:
        IZ_BOOL CreateShader(const void* program, GLenum type)
        {
            CALL_GL_API(m_Shader = ::glCreateShader(type));
            VRETURN(m_Shader != 0);
        
            CALL_GL_API(::glShaderSource(
                m_Shader,
                1,
                (const char**)&program,
                IZ_NULL));
            
            CALL_GL_API(::glCompileShader(m_Shader));

#ifdef __IZ_DEBUG__
            m_strProgram = (const char*)program;

            // Error Check
            GLint isCompiled = 0;
            ::glGetShaderiv(m_Shader, GL_COMPILE_STATUS, &isCompiled);

            if (!isCompiled) {
                GLint infoLen = 0;
                CALL_GL_API(::glGetShaderiv(m_Shader, GL_INFO_LOG_LENGTH, &infoLen));

                if (infoLen > 1) {
                    char* log = (char*)ALLOC_ZERO(_SHADER_BASE::m_Allocator, infoLen);
                    CALL_GL_API(::glGetShaderInfoLog(m_Shader, infoLen, NULL, log));
                    IZ_PRINTF("Shader Compile Log **** \n");
                    IZ_PRINTF("%s", log);
                    FREE(_SHADER_BASE::m_Allocator, log);
                }
            }
#endif  // #ifdef __IZ_DEBUG__

            return IZ_TRUE;
        }

    public:
        GLuint GetRawInterface() { return m_Shader; }

    protected:
        GLuint m_Shader;

#if __IZ_DEBUG__
        std::string m_strProgram;
#endif
    };
}   // namespace graph
}   // namespace izanagi

#endif  // #if !defined(__IZANAGI_GRAPH_SHADER_PROXY_GLES2_H__)
