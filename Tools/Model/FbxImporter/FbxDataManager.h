#if !defined(__MODEL_LIB_FBX_DATA_MANAGER_H__)
#define __MODEL_LIB_FBX_DATA_MANAGER_H__

#include "izToolKit.h"
#include "fbxsdk.h"

#include <map>
#include <set>

struct Face {
    IZ_UINT vtx[3];
};

struct UVData {
    IZ_UINT idxInMesh;
    FbxVector2 uv;

    FbxMesh* fbxMesh{ nullptr };
    FbxSurfaceMaterial* mtrl{ nullptr };
};

struct PosData {
    IZ_UINT idxInMesh;
    FbxVector4 pos;

    FbxMesh* fbxMesh{ nullptr };
    FbxSurfaceMaterial* mtrl{ nullptr };
};    

struct NormalData {
    IZ_UINT idxInMesh;
    FbxVector4 nml;

    FbxMesh* fbxMesh{ nullptr };
    FbxSurfaceMaterial* mtrl{ nullptr };
};

struct ColorData {
    IZ_UINT idxInMesh;
    FbxColor clr;

    FbxMesh* fbxMesh{ nullptr };
    FbxSurfaceMaterial* mtrl{ nullptr };
};

struct VertexData {
    IZ_UINT idxInMesh;

    FbxVector2 uv;      // UV.
    FbxVector4 pos;     // 位置.
    FbxVector4 nml;     // 法線.
    FbxColor clr;       // 頂点カラー.

    std::vector<IZ_FLOAT> weight;
    std::vector<IZ_UINT> joint;

    FbxMesh* fbxMesh{ nullptr };
    FbxSurfaceMaterial* mtrl{ nullptr };

    bool operator==(const VertexData& rhs)
    {
        IZ_BOOL isPos = (this->pos.mData[0] == rhs.pos.mData[0])
                        && (this->pos.mData[1] == rhs.pos.mData[1])
                        && (this->pos.mData[2] == rhs.pos.mData[2]);

        IZ_BOOL isUV = (this->uv.mData[0] == rhs.uv.mData[0])
                        && (this->uv.mData[1] == rhs.uv.mData[1]);

        IZ_BOOL isNml = (this->nml.mData[0] == rhs.uv.mData[0])
                        && (this->nml.mData[1] == rhs.nml.mData[1])
                        && (this->nml.mData[2] == rhs.nml.mData[2]);

        IZ_BOOL isMesh = (this->fbxMesh == rhs.fbxMesh);
        IZ_BOOL isMtrl = (this->mtrl == rhs.mtrl);

        return (isPos && isUV && isNml && isMesh && isMtrl);
    }
};

struct IndexData
{
    IZ_UINT idxInMesh;

    IZ_UINT polygonIdxInMesh;

    FbxMesh* fbxMesh{ nullptr };
    FbxSurfaceMaterial* mtrl{ nullptr };

    IndexData(IZ_UINT idx, IZ_UINT polyIdx, FbxMesh* mesh, FbxSurfaceMaterial* _mtrl)
        : idxInMesh(idx), polygonIdxInMesh(polyIdx), fbxMesh(mesh), mtrl(_mtrl)
    {}
};

struct SkinData
{
    IZ_UINT idxInMesh;

    FbxMesh* fbxMesh{ nullptr };

    std::vector<IZ_FLOAT> weight;
    std::vector<IZ_UINT> joint;

    SkinData() {}

    SkinData(IZ_UINT idx, FbxMesh* mesh)
        : idxInMesh(idx), fbxMesh(mesh)
    {}

    IZ_BOOL isEmpty() const
    {
        return (fbxMesh == nullptr);
    }

    bool operator==(const SkinData& rhs)
    {
        return (idxInMesh == rhs.idxInMesh
            && fbxMesh == rhs.fbxMesh);
    }
};

struct MeshSubset {
    std::vector<Face> faces;

    FbxMesh* fbxMesh{ nullptr };
    FbxSurfaceMaterial* mtrl{ nullptr };

    IZ_UINT vtxNum = 0;

    MeshSubset() {}

    MeshSubset(FbxMesh* _mesh, FbxSurfaceMaterial* _mtrl)
        : fbxMesh(_mesh), mtrl(_mtrl)
    {}

    bool operator==(const MeshSubset& rhs)
    {
        return (fbxMesh == rhs.fbxMesh && mtrl == rhs.mtrl);
    }
};

struct Node {
    fbxsdk::FbxNode* fbxNode{ nullptr };
    IZ_INT targetIdx{ -1 };

    Node() {}

    Node(fbxsdk::FbxNode* node)
        : fbxNode(node)
    {}

    Node(fbxsdk::FbxNode* node, IZ_INT idx)
        : fbxNode(node), targetIdx(idx)
    {}
};

class FbxDataManager {
public:
    FbxDataManager() {}
    ~FbxDataManager() {}

public:
    IZ_BOOL IsValid() const;

    IZ_BOOL Open(const char* path);
    IZ_BOOL OpenForAnm(const char* path, IZ_BOOL nodeOnly = IZ_FALSE);

    void Close();

    void LoadMesh();
    void LoadMaterial();

    IZ_UINT GetFbxMeshNum() const;

    FbxMesh* GetFbxMesh(IZ_UINT idx);

    IZ_UINT GetMeshNum() const;

    MeshSubset& GetMesh(IZ_UINT idx);
    const MeshSubset& GetMesh(IZ_UINT idx) const;

    IZ_UINT GetVtxNum() const;

    const VertexData& GetVertex(IZ_UINT idx) const;

    IZ_UINT GetNodeNum() const;

    FbxNode* GetFbxNode(IZ_UINT idx);
    const Node& GetNode(IZ_UINT idx);

    IZ_UINT GetNodeIndex(const FbxNode* node) const;

    FbxCluster* GetClusterByNode(const FbxNode* node);

    void GetSkinData(
        IZ_UINT idx,
        std::vector<IZ_FLOAT>& weight,
        std::vector<IZ_UINT>& joint) const;

    IZ_UINT GetMaterialNum() const;
    FbxSurfaceMaterial* GetMaterial(IZ_UINT idx);

    IZ_INT GetAnmStartFrame() const { return m_AnmStartFrame; }
    IZ_INT GetAnmStopFrame() const { return m_AnmStopFrame; }

    // ベースモデルデータに基づいてノードの再調整.
    IZ_UINT RearrangeNodeByTargetBaseModel(FbxDataManager* target);

private:
    void LoadAnimation(FbxImporter* importer);

    // ノードを集める.
    void GatherNodes(FbxNode* node);

    void GatherMeshes();

    void GatherClusters();

    void GatherFaces();

    void GatherVertices();

    void GatherPos(std::map<FbxMesh*, std::vector<PosData>>& posList);
    void GatherUV(std::map<FbxMesh*, std::vector<UVData>>& uvList);

    void GatherNormal(std::map<FbxMesh*, std::vector<NormalData>>& nmlList);
    void GatherColor(std::map<FbxMesh*, std::vector<ColorData>>& clrList);

    void GatherSkin(std::vector<SkinData>& skinList);

    fbxsdk::FbxSurfaceMaterial* GetMaterial(FbxMesh* fbxMesh, IZ_UINT index);

private:
    FbxManager* m_manager{ nullptr };
    FbxScene* m_scene{ nullptr };

    std::vector<Node> m_nodes;
    std::vector<FbxMesh*> m_fbxMeshes;

    std::vector<FbxCluster*> m_clusters;

    std::vector<MeshSubset> m_meshes;

    std::map<FbxMesh*, std::vector<IndexData>> m_indices;
    std::vector<VertexData> m_vertices;

    std::vector<fbxsdk::FbxSurfaceMaterial*> m_materials;

    IZ_INT m_AnmStartFrame;
    IZ_INT m_AnmStopFrame;
};

#endif  // #if !defined(__MODEL_LIB_FBX_DATA_MANAGER_H__)
