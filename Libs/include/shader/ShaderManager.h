﻿#if !defined(__IZANAGI_SHADER_SHADER_MANAGER_H__)
#define __IZANAGI_SHADER_SHADER_MANAGER_H__

#include "izStd.h"
#include "base/Shader.h"
#include "base/ShaderDefs.h"

namespace izanagi {
	/**
	*/
	class CShaderManager : public CObject {
	public:
		// Create instance of CShaderManager.
		static CShaderManager* CreateShaderManager(IMemoryAllocator* pAllocator);

	private:
		CShaderManager();
		~CShaderManager();

		NO_COPIABLE(CShaderManager);

		IZ_DEFINE_INTERNAL_RELEASE();

	public:
		IZ_BOOL Add(CShader* pShader);
		void ClearAll();

		IZ_UINT GetShaderNumByName(IZ_PCSTR name);
		IZ_UINT GetShaderNumByKey(IZ_UINT key);

		CShader* GetShaderByName(IZ_PCSTR name);
		CShader* GetShaderByKey(IZ_UINT key);

		CShader* GetShaderByAttr(
			IZ_PCSTR name,
			const CShaderAttr pAttrs[],
			IZ_UINT nAttrNum);
		CShader* GetShaderByAttr(
			IZ_UINT key,
			const CShaderAttr pAttrs[],
			IZ_UINT nAttrNum);

	private:
		IMemoryAllocator* m_pAllocator;

		CStdHash<IZ_UINT, CShader, SHD_HASH_MAX> m_ShaderHash;

		typedef CStdHash<IZ_UINT, CShader, SHD_HASH_MAX>::Item CShaderHashItem;
	};
}	// namespace izanagi

#endif	// #if !defined(__IZANAGI_SHADER_SHADER_MANAGER_H__)
