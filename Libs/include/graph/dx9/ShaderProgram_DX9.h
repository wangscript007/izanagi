#if !defined(__IZANAGI_GRAPH_SHADER_PROGRAM_DX9_H__)
#define __IZANAGI_GRAPH_SHADER_PROGRAM_DX9_H__

#include "graph/ShaderProgram.h"

namespace izanagi
{
namespace graph
{
    class CVertexShaderDX9;
    class CPixelShaderDX9;

    /**
     */
    class CShaderProgramDX9 : public CShaderProgram
    {
        friend class CGraphicsDeviceDX9;

        static CShaderProgram* CreateShaderProgram(IMemoryAllocator* allocator);

    private:
        CShaderProgramDX9();
        virtual ~CShaderProgramDX9() {}

    public:
        virtual IZ_BOOL AttachVertexShader(CVertexShader* vs);

        virtual IZ_BOOL AttachPixelShader(CPixelShader* ps);

        virtual IZ_BOOL Link();

        virtual IZ_BOOL IsValid();

        virtual IZ_BOOL IsDirty();

        virtual void ClearDirty();

    public:
        virtual SHADER_PARAM_HANDLE GetHandleByName(IZ_PCSTR name);

        virtual IZ_UINT GetSamplerIndex(const SHADER_PARAM_HANDLE& handle);

    public:
        virtual IZ_BOOL SetBool(CGraphicsDevice* device, const SHADER_PARAM_HANDLE& handle, IZ_BOOL b);
        virtual IZ_BOOL SetBoolArray(CGraphicsDevice* device, const SHADER_PARAM_HANDLE& handle, const IZ_BOOL* b, IZ_UINT num);

        virtual IZ_BOOL SetFloat(CGraphicsDevice* device, const SHADER_PARAM_HANDLE& handle, IZ_FLOAT f);
        virtual IZ_BOOL SetFloatArray(CGraphicsDevice* device, const SHADER_PARAM_HANDLE& handle, const IZ_FLOAT* f, IZ_UINT num);

        virtual IZ_BOOL SetInt(CGraphicsDevice* device, const SHADER_PARAM_HANDLE& handle, IZ_INT n);
        virtual IZ_BOOL SetIntArray(CGraphicsDevice* device, const SHADER_PARAM_HANDLE& handle, const IZ_INT* n, IZ_UINT num);

        virtual IZ_BOOL SetMatrix(CGraphicsDevice* device, const SHADER_PARAM_HANDLE& handle, const math::SMatrix& m);
        virtual IZ_BOOL SetMatrixArray(CGraphicsDevice* device, const SHADER_PARAM_HANDLE& handle, const math::SMatrix* m, IZ_UINT num);

        virtual IZ_BOOL SetVector(CGraphicsDevice* device, const SHADER_PARAM_HANDLE& handle, const math::SVector& v);
        virtual IZ_BOOL SetVectorArray(CGraphicsDevice* device, const SHADER_PARAM_HANDLE& handle, const math::SVector* v, IZ_UINT num);

        virtual IZ_BOOL SetValue(CGraphicsDevice* device, const SHADER_PARAM_HANDLE& handle, const void* p, IZ_UINT size);

    private:
        CVertexShaderDX9* VertexShader();
        CPixelShaderDX9* PixelShader();

        IZ_BOOL IsDirtyVS() { return m_IsDirtyVS; }
        IZ_BOOL IsDirtyPS() { return m_IsDirtyPS; }

    private:
        IZ_BOOL m_IsDirtyVS;
        IZ_BOOL m_IsDirtyPS;
    };
}   // namespace graph
}   // namespace izanagi

#endif  // #if !defined(__IZANAGI_GRAPH_SHADER_PROGRAM_DX9_H__)