#pragma once

#include "require.h"

#define MAX_SUBSET 30

typedef struct Vertex
{
    XMFLOAT3 position;
    XMFLOAT3 normal;
    XMFLOAT2 texcoord;
    XMFLOAT3 tangent;
} FbxVertex;

struct FbxMaterial
{
    XMFLOAT3 ambient;
    XMFLOAT3 diffuse;
    XMFLOAT3 emissive;
    XMFLOAT3 specular;

    float opacity;
    float shininess;
    float reflectivity;

    // false Lambert and true Phong.
    WCHAR strDiffuseTexture[MAX_PATH];
    WCHAR strEmissiveTexture[MAX_PATH];
    WCHAR strNormalMap[MAX_PATH];
    WCHAR strMaterialName[MAX_PATH];
};

struct Subset
{
    UINT indexCount;
    UINT indexStart;
    UINT vertexStart;
    UINT materialID;
};
