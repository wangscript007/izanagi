#if !defined(__IZANAGI_GRAPH_VERTEX_DECLARATION_GLES2_H__)
#define __IZANAGI_GRAPH_VERTEX_DECLARATION_GLES2_H__

#include "graph/VertexDeclaration.h"

namespace izanagi
{
namespace graph
{
    class CGraphicsDeviceGLES2;
    class CShaderProgramGLES2;

    // 頂点宣言
    class CVertexDeclarationGLES2 : public CVertexDeclaration
    {
        friend class CGraphicsDeviceGLES2;

    protected:
        enum State {
            None,
            Enabled,
            Disabled,
        };

        static State s_EnabledAttribIndex[16];

    private:
        // インスタンス作成
        static CVertexDeclaration* CreateVertexDeclaration(
            IMemoryAllocator* allocator,
            const SVertexElement* pElem,
            IZ_UINT nElemNum);

    protected:
        inline CVertexDeclarationGLES2();
        virtual inline ~CVertexDeclarationGLES2();

    protected:
        IZ_UINT GetElemNum() const;

        const SVertexElement* GetElements() const;

        IZ_BOOL Apply(
            CShaderProgramGLES2* program,
            IZ_UINT vtxOffset,
            IZ_UINT vtxStride);

        const char* GetAttribName(IZ_UINT elementIdx);        
    };
}   // namespace graph
}   // namespace izanagi

#endif  // #if !defined(__IZANAGI_GRAPH_VERTEX_DECLARATION_GLES2_H__)
