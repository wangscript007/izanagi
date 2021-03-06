#if !defined(__STATE_STATE_0_H__)
#define __STATE_STATE_0_H__

#include "izDefs.h"
#include "izStd.h"
#include "izGraph.h"
#include "izSceneGraph.h"

class CStateTest0 : public izanagi::CSceneStateBase {
public:
	CStateTest0();
	~CStateTest0();

public:
	IZ_BOOL Init();
	IZ_BOOL Render(izanagi::graph::CGraphicsDevice* device);
	IZ_BOOL Update();
	IZ_BOOL Destroy();
	IZ_BOOL Enter(izanagi::IMemoryAllocator* allocator, void* val);
	IZ_BOOL Leave();

	IZ_BOOL OnKeyDown(IZ_UINT nChar) { return IZ_TRUE; }
	IZ_BOOL OnMouseMove(IZ_INT x, IZ_INT y) { return IZ_TRUE; }
	IZ_BOOL OnMouseWheel(IZ_SHORT zDelta) { return IZ_TRUE; }
};

#endif	// #if !defined(__STATE_STATE_0_H__)
