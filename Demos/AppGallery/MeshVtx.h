#if !defined(__MESH_VTX_H__)
#define __MESH_VTX_H__

#include "izSampleKit.h"

struct SMeshVtx : public izanagi:: SMeshVtx {
    IZ_FLOAT nextNml[4];
    IZ_FLOAT dir[3];

    IZ_FLOAT nextNml2[4];
    IZ_FLOAT dir2[3];
};

#endif    // #if !defined(__MESH_VTX_H__)
