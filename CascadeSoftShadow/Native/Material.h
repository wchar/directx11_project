#pragma once

#include "require.h"
#include "MeshMisc.h"

NS_WE_BEGAN

class Material
{    
public:
    Material();
    static Material* create(FbxMaterial&);
    void prepare();

    FLOAT alpha;
    XMFLOAT3 diffuse;
    ID3D11ShaderResourceView* getTex()
    {
        return _diffuseTex;
    }
private:
    ID3D11ShaderResourceView* _diffuseTex;
};

NS_WE_END