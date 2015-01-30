#pragma once

#include "require.h"
#include "Mesh.h"

NS_WE_BEGAN

class DLL_EXPORT Renderer
{
public:
    Renderer();
    ~Renderer();

    void setMesh(Mesh* mesh)
    {
        _object = mesh;
    }

    void onFrameMove();
    void onFrameRender();

private:
    Mesh* _object = nullptr;
};

NS_WE_END