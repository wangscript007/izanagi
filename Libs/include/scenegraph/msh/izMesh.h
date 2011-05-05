﻿#if !defined(__IZANAGI_SCENEGRAPH_IZ_MESH_H__)
#define __IZANAGI_SCENEGRAPH_IZ_MESH_H__

#include "MSHFormat.h"

namespace izanagi {
	class CGraphicsDevice;
	class CVertexBuffer;
	class IInputStream;
	class CMeshGroup;
	class CSkeletonInstance;
	class IMshRenderHandler;

	/**
	 * メッシュデータ
	 */
	class CMesh : public CObject {
		friend class CModel;

	public:
		static CMesh* CreateMesh(
			IMemoryAllocator* pAllocator,
			CGraphicsDevice* pDevice,
			IInputStream* pIn);

	private:
		CMesh();
		~CMesh();

		NO_COPIABLE(CMesh);

		IZ_DEFINE_INTERNAL_RELEASE();

	public:
		/**
		 * メッシュを描画
		 */
		IZ_BOOL Render(
			CSkeletonInstance* pSkl,
			IMshRenderHandler* pRenderHandler);

	public:
		/**
		 * メッシュグループ（= メッシュセットの集まり）を取得
		 */
		CMeshGroup* GetMeshGroup(IZ_UINT idx);

		const S_MSH_HEADER& GetHeader() const { return m_Header; }

		/**
		 * メッシュグループ総数を取得
		 */
		IZ_UINT GetMeshGroupNum() const { return m_Header.numMeshGroup; }

		CGraphicsDevice* GetGraphicsDevice() { return m_pDevice; }

	private:
		IMemoryAllocator* m_pAllocator;
		CGraphicsDevice* m_pDevice;

		S_MSH_HEADER m_Header;

		CMeshGroup** m_pMeshGroup;
	};
}	// namespace izanagi

#endif	// #if !defined(__IZANAGI_SCENEGRAPH_IZ_MESH_H__)
