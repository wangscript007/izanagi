#if !defined(__TEST_MOUSE_HIT_H__)
#define __TEST_MOUSE_HIT_H__

#include "izSampleKit.h"

static const IZ_UINT SCREEN_WIDTH = 1280;
static const IZ_UINT SCREEN_HEIGHT = 720;

class CMouseHit : public izanagi::sample::CSampleApp {
public:
	CMouseHit();
	virtual ~CMouseHit();

protected:
	// 初期化.
	virtual IZ_BOOL InitInternal(
		izanagi::IMemoryAllocator* allocator,
		izanagi::CGraphicsDevice* device,
		izanagi::sample::CSampleCamera& camera);

	// 解放.
	virtual void ReleaseInternal();

	// 更新.
	virtual void UpdateInternal(
		izanagi::CCamera& camera,
		izanagi::CGraphicsDevice* device);

	// 描画.
	virtual void RenderInternal(izanagi::CGraphicsDevice* device);

	virtual IZ_BOOL OnKeyDown(IZ_UINT nChar);
	virtual void OnKeyUp(IZ_UINT nChar);
	virtual IZ_BOOL OnMouseLBtnDown(const izanagi::CIntPoint& point);

private:
	struct {
		izanagi::CTriangle tri;
		izanagi::SMatrix mtx;
		izanagi::CVertexBuffer* vb;
	} m_Triangles;

	izanagi::CVertexDeclaration* m_VD;

	izanagi::CShaderBasic* m_Shader;

	IZ_BOOL m_IsPressKey;
	izanagi::CIntPoint m_MousePoint;
	IZ_BOOL m_IsCross;
};

#endif	// #if !defined(__TEST_MOUSE_HIT_H__)