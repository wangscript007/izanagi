#if !defined(__IZANAGI_GLES2_DEFS_H__)
#define __IZANAGI_GLES2_DEFS_H__

#include <EGL/egl.h>
#include <GLES2/gl2.h>

namespace izanagi {
    typedef GLuint TEX_HANDLE;
    typedef GLint SHADER_PARAM_HANDLE;

    typedef EGLDisplay Display;
    typedef EGLSurface Surface;
    typedef EGLContext Context;
}

#define IS_VALID_SHADER_PARAM_HANDLE(handle)    ((handle) != 0)

#ifdef __IZ_DEBUG__
    #define CALL_GL_API(func)\
        func;\
        {\
            GLenum __gl_err__ = ::glGetError();\
            if (__gl_err__ != GL_NO_ERROR) { IZ_PRINTF("GL ERROR [0x%x]\n", __gl_err__); IZ_ASSERT(IZ_FALSE); }\
        }
#else
    #define CALL_GL_API(func) func
#endif

namespace izanagi
{
namespace graph
{
    /** 初期化パラメータ
     */
    struct SGraphicsDeviceInitParams {
        EGLNativeWindowType window;
        EGLNativeDisplayType display;

        GLuint screenWidth;
        GLuint screenHeight;

        GLubyte rgba[4];

        GLubyte depth;
        GLubyte stencil;

        GLboolean enableMultiSample;
    };
}   // namespace graph
}   // namespace izanagi

#endif  // #if !defined(__IZANAGI_GLES2_DEFS_H__)
