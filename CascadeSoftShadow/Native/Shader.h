#pragma once

#include "require.h"

NS_WE_BEGAN

class Shader
{
public:
    Shader(){}
    virtual ~Shader(){}
    virtual void prepare() = 0;

protected:
    ID3DBlob* _blob;
};

class VertexShader : public Shader
{
public:
	VertexShader(function<void(ID3DBlob**, ID3D11VertexShader**)>);
    ~VertexShader();

    virtual void prepare();
protected:
    ID3D11VertexShader* _VS;
};

class PixelShader : public Shader
{
public:
	PixelShader(function<void(ID3DBlob**, ID3D11PixelShader**)>);
    ~PixelShader();

    virtual void prepare();
protected:
    ID3D11PixelShader* _PS;
};

class MeshVertexShader : public VertexShader
{
public:
    MeshVertexShader();
};

class CascadeShader : public PixelShader
{
public:
    CascadeShader();
};

class MeshPixelShader : public PixelShader
{
public:
    MeshPixelShader();
};

class FullScreenVertexShader : public VertexShader
{
public:
    FullScreenVertexShader();
};

class FullScreenPixelShader : public PixelShader
{
public:
    FullScreenPixelShader();
};

class CascadeBlurPixelShader : public PixelShader
{
public:
	CascadeBlurPixelShader();
};

class VarianceBlurX : public PixelShader
{
public:
	VarianceBlurX();
};

class VarianceBlurY : public PixelShader
{
public:
	VarianceBlurY();
};

class VarianceBlurVS : public VertexShader
{
public:
	VarianceBlurVS();
};

class Shaders
{
public:
    void prepareToMesh()
    {
        _meshVertexShader.prepare();
        _meshPixelShader.prepare();
    }
    void prepareToCascade()
    {
        _meshVertexShader.prepare();
        _cascadeShader.prepare();
    }
    void prepareToTest()
    {
        _fullScreenVertexShader.prepare();
        _fullScreenPixelShader.prepare();
    }
	void prepareToBlurX()
	{
		_varianceBlurVS.prepare();
		_varianceBlurX.prepare();
	}
	void prepareToBlurY()
	{
		_varianceBlurVS.prepare();
		_varianceBlurY.prepare();
	}

private:    
   	VarianceBlurX _varianceBlurX;
	VarianceBlurY _varianceBlurY;
	VarianceBlurVS _varianceBlurVS;
	
	MeshVertexShader _meshVertexShader;    
	MeshPixelShader _meshPixelShader;
    CascadeShader _cascadeShader;

	FullScreenVertexShader _fullScreenVertexShader;
	FullScreenPixelShader _fullScreenPixelShader;
};

NS_WE_END