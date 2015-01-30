#include "Shader.h"
#include "clover.h"
#include "Content.h"
#include "BufferType.h"

USING_WE

VertexShader::VertexShader(wchar_t* file)
{
    _blob = gContent()->getBlob(file, "VS");
    V(gDevice()->CreateVertexShader(_blob->GetBufferPointer(), _blob->GetBufferSize(), NULL, &_VS));
}

VertexShader::~VertexShader()
{
    SAFE_RELEASE(_VS);
}

void VertexShader::prepare()
{
    gContext()->VSSetShader(_VS, nullptr, 0);
}

PixelShader::PixelShader(wchar_t* file)
{
    _blob = gContent()->getBlob(file, "PS");
    V(gDevice()->CreatePixelShader(_blob->GetBufferPointer(), _blob->GetBufferSize(), NULL, &_PS));
}

PixelShader::~PixelShader()
{
    SAFE_RELEASE(_PS);
}

void PixelShader::prepare()
{
    gContext()->PSSetShader(_PS, nullptr, 0);
}

MeshVertexShader::MeshVertexShader() :
VertexShader(L"shaders/MeshVS.hlsl")
{

}

MeshVertexShader::~MeshVertexShader()
{
}

void MeshVertexShader::prepare()
{

}

CascadeShader::CascadeShader() :
PixelShader(L"shaders/MeshCascade.hlsl")
{
}

CascadeShader::~CascadeShader()
{
    SAFE_RELEASE(_PS);
}

void CascadeShader::prepare()
{
    PixelShader::prepare();
}

MeshPixelShader::MeshPixelShader() :
PixelShader(L"shaders/MeshPS.hlsl")
{
}

MeshPixelShader::~MeshPixelShader()
{
}

void MeshPixelShader::prepare()
{
    PixelShader::prepare();
}

FullScreenVertexShader::FullScreenVertexShader() :
VertexShader(L"shaders/FullScreenQuadVS.hlsl")
{
}

FullScreenVertexShader::~FullScreenVertexShader()
{
}

void FullScreenVertexShader::prepare()
{
    VertexShader::prepare();
}

FullScreenPixelShader::FullScreenPixelShader() :
PixelShader(L"shaders/TestSRV.hlsl")
{
}

FullScreenPixelShader::~FullScreenPixelShader()
{
}

void FullScreenPixelShader::prepare()
{
    PixelShader::prepare();
}