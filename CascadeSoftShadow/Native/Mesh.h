#pragma once

#include "require.h"
#include "Material.h"

NS_WE_BEGAN

class DLL_EXPORT Mesh
{
public:
    static Mesh* createFromFbx(wchar_t* file);

	void drawSubset();
private:    
    Mesh();
    ~Mesh();
    
    bool loadFbx(wchar_t* file);

    ID3D11Buffer* _vertexBuffer;
    ID3D11Buffer* _indexBuffer;

    vector<Material*> _materials;
    vector<Subset> _subsets;
};

NS_WE_END

