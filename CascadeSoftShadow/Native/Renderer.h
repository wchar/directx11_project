#pragma once

#include "require.h"
#include "Mesh.h"
#include "CascadeManager.h"

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

	void resizeWindow();
    void onFrameMove();
    void onFrameRender();
	CascadeManager* getManager();
private:
    Mesh* _object = nullptr;
	bool _resize;
};

NS_WE_END