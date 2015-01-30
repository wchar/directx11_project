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
    VertexShader(wchar_t* file);
    ~VertexShader();

    virtual void prepare();
protected:
    ID3D11VertexShader* _VS;
};

class PixelShader : public Shader
{
public:
    PixelShader(wchar_t* file);
    ~PixelShader();

    virtual void prepare();
protected:
    ID3D11PixelShader* _PS;
};

class MeshVertexShader : public VertexShader
{
public:
    MeshVertexShader();
    ~MeshVertexShader();
    virtual void prepare();
};

class CascadeShader : public PixelShader
{
public:
    CascadeShader();
    ~CascadeShader();
    virtual void prepare();
};

class MeshPixelShader : public PixelShader
{
public:
    MeshPixelShader();
    ~MeshPixelShader();
    virtual void prepare();
};

class SkeletonVertexShader;
class SkeletonPixelShader;

class FullScreenVertexShader : public VertexShader
{
public:
    FullScreenVertexShader();
    ~FullScreenVertexShader();
    virtual void prepare();
};

class FullScreenPixelShader : public PixelShader
{
public:
    FullScreenPixelShader();
    ~FullScreenPixelShader();
    virtual void prepare();
};

class HDR;
class HDAO;

class Shaders
{
public:
    void prepareToCascade()
    {
        _meshVertexShader.prepare();
        _meshPixelShader.prepare();
    }
    void prepareToMesh()
    {
        _meshVertexShader.prepare();
        _cascadeShader.prepare();
    }
    void prepareToTest()
    {
        _fullScreenVertexShader.prepare();
        _fullScreenPixelShader.prepare();
    }
private:
    MeshVertexShader _meshVertexShader;
    MeshPixelShader _meshPixelShader;
    CascadeShader _cascadeShader;
    FullScreenVertexShader _fullScreenVertexShader;
    FullScreenPixelShader _fullScreenPixelShader;
};

NS_WE_END