#pragma once

#include "MeshMisc.h"
#include "fbxCommon.h"

USING_WE

class FbxLoader
{
public:
    static FbxLoader* create(wchar_t* file);
private:
    bool load(wchar_t* file);
    bool getTextureFile(FbxProperty* lProperty, WCHAR* wstrFileName);
    void fillMaterialStructure(FbxMaterial* material, FbxSurfaceMaterial *lFbxMaterial);
    UINT parseMaterial(FbxGeometry* geometry);
    void parseMesh(FbxMesh* mesh);
    void loadMesh(FbxNode* node);
private:
    UINT getVertex(FbxVertex* vertex, UINT idx);

    vector <UINT>       _arrvIdxControlPoint;
    vector <XMFLOAT3>   _arrvPosition;
    vector <XMFLOAT3>   _arrvNormal;
    vector <XMFLOAT2>   _arrvTexcoord;

    vector <UINT>       _arrvTriangleMaterialIndex;
    vector <FbxMaterial>   _arrvMaterial;

    friend class FbxMeshImporter;
};

class FbxMeshImporter
{
public:
    FbxMeshImporter();
    static FbxMeshImporter* create(wchar_t* file);

    vector <FbxVertex> vertices;
    vector <DWORD> indices;
    vector <Subset> subsets;
    vector <FbxMaterial> materials;

    UINT vertexCount;
    UINT indexCount;
private:
    bool import(wchar_t* file);

    struct CacheEntry
    {
        UINT index;
        CacheEntry* next;
    };

    vector <CacheEntry*> _vertexCache;
    void releaseCacheEntry();
    DWORD addVertex(UINT hash, FbxVertex* vertices);
};