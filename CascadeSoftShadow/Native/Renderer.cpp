#include "Renderer.h"
#include "Shader.h"

USING_WE

static Shaders* s_getShaders()
{
	static Shaders s_shaders; 

	return &s_shaders;
}


void Renderer::onFrameRender()
{
    if (_object)
    {
		s_getShaders()->prepareToCascade();
        _object->drawSubset();

		s_getShaders()->prepareToMesh();
        _object->drawSubset();

		s_getShaders()->prepareToCascade();
    }
}
