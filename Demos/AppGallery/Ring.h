#if !defined(__RING_H__)
#define __RING_H__

#include "izSampleKit.h"
#include "MeshVtx.h"

class Ring : public izanagi::CDebugMeshTmpl<SMeshVtx> {
    friend class izanagi::CDebugMeshTmpl<SMeshVtx>;

public:
    // Create an instance.
    static Ring* Create(
        izanagi::IMemoryAllocator* allocator,
        izanagi::graph::CGraphicsDevice* device);

private:
    Ring();
    virtual ~Ring();

    NO_COPIABLE(Ring);

    IZ_DEFINE_INTERNAL_RELEASE();

private:
    IZ_BOOL Init(
        izanagi::IMemoryAllocator* allocator,
        izanagi::graph::CGraphicsDevice* device,
        IZ_UINT flag);

    IZ_BOOL CreateVB(
        izanagi::graph::CGraphicsDevice* device,
        IZ_UINT flag,
        IZ_UINT vtxNum);

    IZ_BOOL CreateVD(
        izanagi::graph::CGraphicsDevice* device,
        IZ_UINT flag);

    virtual IZ_UINT8* SetExtraVtxData(
        const void* sVtx,
        IZ_UINT flag,
        IZ_UINT8* data);

    IZ_BOOL SetData(
        izanagi::graph::CGraphicsDevice* device,
        IZ_UINT flag,
        IZ_UINT slices,
        IZ_COLOR color,
        IZ_FLOAT innerR,
        IZ_FLOAT outerR,
        IZ_FLOAT height);

    SMeshVtx* SetDataUpFace(
        SMeshVtx* vtx,
        IZ_UINT flag,
        IZ_UINT slices,
        IZ_COLOR color,
        IZ_FLOAT innerR,
        IZ_FLOAT outerR,
        IZ_FLOAT height);

     SMeshVtx* SetDataDownFace(
        SMeshVtx* vtx,
        IZ_UINT flag,
        IZ_UINT slices,
        IZ_COLOR color,
        IZ_FLOAT innerR,
        IZ_FLOAT outerR,
        IZ_FLOAT height);

    SMeshVtx* SetDataOuterFace(
        SMeshVtx* vtx,
        IZ_UINT flag,
        IZ_UINT slices,
        IZ_COLOR color,
        IZ_FLOAT innerR,
        IZ_FLOAT outerR,
        IZ_FLOAT height);

    SMeshVtx* SetDataInnerFace(
        SMeshVtx* vtx,
        IZ_UINT flag,
        IZ_UINT slices,
        IZ_COLOR color,
        IZ_FLOAT innerR,
        IZ_FLOAT outerR,
        IZ_FLOAT height);

    IZ_BOOL SetIdx(IZ_UINT slices);

    izanagi::SMeshFace* SetIdxUpFace(
        IZ_UINT slices,
        izanagi::SMeshFace* face,
        IZ_UINT& idx);

    izanagi::SMeshFace* SetIdxDownFace(
        IZ_UINT slices,
        izanagi::SMeshFace* face,
        IZ_UINT& idx);

    izanagi::SMeshFace* SetIdxOuterFace(
        IZ_UINT slices,
        izanagi::SMeshFace* face,
        IZ_UINT& idx);

    izanagi::SMeshFace* SetIdxInnerFace(
        IZ_UINT slices,
        izanagi::SMeshFace* face,
        IZ_UINT& idx);
};

#endif    // #if !defined(__RING_H__)
