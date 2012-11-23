#if !defined(__IZANAGI_GRAPH_SHADER_PROXY_H__)
#define __IZANAGI_GRAPH_SHADER_PROXY_H__

#include "izDefs.h"
#include "izStd.h"
#include "izMath.h"
#include "graph/GraphDefs.h"
#include "graph/ShaderBase.h"
#include "graph/GraphicsDevice.h"

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
		    m_ConstTable = IZ_NULL;
	    }
		virtual ~CShaderProxy()
        {
            SAFE_RELEASE(m_ConstTable);
        }

	public:
        // ハンドル取得
	    virtual SHADER_PARAM_HANDLE GetHandleByIdx(IZ_UINT idx)
	    {
		    SHADER_PARAM_HANDLE ret = m_ConstTable->GetConstant(IZ_NULL, idx);
		    return ret;
	    }

        // ハンドル取得
	    virtual SHADER_PARAM_HANDLE GetHandleByName(IZ_PCSTR pName)
	    {
		    SHADER_PARAM_HANDLE ret = m_ConstTable->GetConstantByName(IZ_NULL, pName);
		    return ret;
	    }

        // パラメータの配列数取得
	    virtual IZ_UINT GetArrayLengthOf(CGraphicsDevice* device, SHADER_PARAM_HANDLE handle)
	    {
		    IZ_UINT ret = 0;

		    D3DXCONSTANT_DESC desc;
		    IZ_UINT count = 1;

		    HRESULT hr = m_ConstTable->GetConstantDesc(handle, &desc, &count);
		    if (SUCCEEDED(hr)) {
			    ret = desc.Elements;
		    }

		    return ret;
	    }

        virtual IZ_UINT GetSamplerIndex(SHADER_PARAM_HANDLE handle)
        {
            IZ_UINT ret = m_ConstTable->GetSamplerIndex(handle);
            return ret;
        }

        // 定数をデフォルト値に設定
	    virtual IZ_BOOL SetDefaults(CGraphicsDevice* device)
	    {
		    HRESULT hr = m_ConstTable->SetDefaults(GetDeviceRawInterface(device));
		    IZ_ASSERT(SUCCEEDED(hr));
		    return SUCCEEDED(hr);
	    }

        // ブール値を設定
	    virtual IZ_BOOL SetBool(CGraphicsDevice* device, SHADER_PARAM_HANDLE handle, IZ_BOOL b)
	    {
		    HRESULT hr = m_ConstTable->SetBool(
						    GetDeviceRawInterface(device),
						    handle,
						    b);
		    IZ_ASSERT(SUCCEEDED(hr));
		    return SUCCEEDED(hr);
	    }

        // ブール値を設定
	    virtual IZ_BOOL SetBoolArray(CGraphicsDevice* device, SHADER_PARAM_HANDLE handle, const IZ_BOOL* pB, IZ_UINT num)
	    {
		    IZ_C_ASSERT(sizeof(BOOL) == sizeof(IZ_BOOL));

		    HRESULT hr = m_ConstTable->SetBoolArray(
						    GetDeviceRawInterface(device),
						    handle,
						    (const BOOL*)pB, num);
		    IZ_ASSERT(SUCCEEDED(hr));
		    return SUCCEEDED(hr);
	    }

        // 浮動小数点数を設定
	    virtual IZ_BOOL SetFloat(CGraphicsDevice* device, SHADER_PARAM_HANDLE handle, IZ_FLOAT f)
	    {
		    HRESULT hr = m_ConstTable->SetFloat(
						    GetDeviceRawInterface(device),
						    handle,
						    f);
		    IZ_ASSERT(SUCCEEDED(hr));
		    return SUCCEEDED(hr);
	    }

        // 浮動小数点数を設定
	    virtual IZ_BOOL SetFloatArray(CGraphicsDevice* device, SHADER_PARAM_HANDLE handle, const IZ_FLOAT* pF, IZ_UINT num)
	    {
		    HRESULT hr = m_ConstTable->SetFloatArray(
						    GetDeviceRawInterface(device),
						    handle,
						    pF, num);
		    IZ_ASSERT(SUCCEEDED(hr));
		    return SUCCEEDED(hr);
	    }

        // 整数値を設定
	    virtual IZ_BOOL SetInt(CGraphicsDevice* device, SHADER_PARAM_HANDLE handle, IZ_INT n)
	    {
		    HRESULT hr = m_ConstTable->SetInt(
						    GetDeviceRawInterface(device),
						    handle,
						    n);
		    IZ_ASSERT(SUCCEEDED(hr));
		    return SUCCEEDED(hr);
	    }

        // 整数値を設定
	    virtual IZ_BOOL SetIntArray(CGraphicsDevice* device, SHADER_PARAM_HANDLE handle, const IZ_INT* pN, IZ_UINT num)
	    {
		    HRESULT hr = m_ConstTable->SetIntArray(
						    GetDeviceRawInterface(device),
						    handle,
						    pN, num);
		    IZ_ASSERT(SUCCEEDED(hr));
		    return SUCCEEDED(hr);
	    }

        // 行列を設定
	    virtual IZ_BOOL SetMatrix(CGraphicsDevice* device, SHADER_PARAM_HANDLE handle, const SMatrix& m)
	    {
		    IZ_C_ASSERT(sizeof(D3DXMATRIX) == sizeof(SMatrix));

		    HRESULT hr = m_ConstTable->SetMatrix(
						    GetDeviceRawInterface(device),
						    handle,
						    (const D3DXMATRIX*)&m);
		    IZ_ASSERT(SUCCEEDED(hr));
		    return SUCCEEDED(hr);
	    }

        // 行列を設定
	    virtual IZ_BOOL SetMatrixArray(CGraphicsDevice* device, SHADER_PARAM_HANDLE handle, const SMatrix* pM, IZ_UINT num)
	    {
		    IZ_C_ASSERT(sizeof(D3DXMATRIX) == sizeof(SMatrix));

		    HRESULT hr = m_ConstTable->SetMatrixArray(
						    GetDeviceRawInterface(device),
						    handle,
						    (const D3DXMATRIX*)pM, num);
		    IZ_ASSERT(SUCCEEDED(hr));
		    return SUCCEEDED(hr);
	    }

        // 4D ベクトルを設定
	    virtual IZ_BOOL SetVector(CGraphicsDevice* device, SHADER_PARAM_HANDLE handle, const SVector& v)
	    {
		    IZ_C_ASSERT(sizeof(D3DXVECTOR4) == sizeof(SVector));

		    HRESULT hr = m_ConstTable->SetVector(
						    GetDeviceRawInterface(device),
						    handle,
						    (const D3DXVECTOR4*)&v);
		    IZ_ASSERT(SUCCEEDED(hr));
		    return SUCCEEDED(hr);
	    }

        // 4D ベクトルを設定
	    virtual IZ_BOOL SetVectorArray(CGraphicsDevice* device, SHADER_PARAM_HANDLE handle, const SVector* pV, IZ_UINT num)
	    {
		    IZ_C_ASSERT(sizeof(D3DXVECTOR4) == sizeof(SVector));

		    HRESULT hr = m_ConstTable->SetVectorArray(
						    GetDeviceRawInterface(device),
						    handle,
						    (const D3DXVECTOR4*)pV, num);
		    IZ_ASSERT(SUCCEEDED(hr));
		    return SUCCEEDED(hr);
	    }

        // バッファの内容を定数テーブルに設定
	    virtual IZ_BOOL SetValue(CGraphicsDevice* device, SHADER_PARAM_HANDLE handle, const void* p, IZ_UINT size)
	    {
		    HRESULT hr = m_ConstTable->SetValue(
						    GetDeviceRawInterface(device),
						    handle,
						    p, size);
		    IZ_ASSERT(SUCCEEDED(hr));
		    return SUCCEEDED(hr);
	    }

        D3D_DEVICE* GetDeviceRawInterface(CGraphicsDevice* device)
        {
            return (D3D_DEVICE*)device->GetPlatformInterface();
        }

	public:
		D3D_CONSTTABLE* GetRawInterface() { return m_ConstTable; }

	protected:
		// シェーダ定数テーブル
		D3D_CONSTTABLE* m_ConstTable;
	};
}   // namespace graph
}	// namespace izanagi

#endif	// #if !defined(__IZANAGI_GRAPH_SHADER_PROXY_H__)