#include "Shader.h"
#include "clover.h"
#include "Content.h"
#include "BufferType.h"

USING_WE
VertexShader::VertexShader(function<void(ID3DBlob **, ID3D11VertexShader **)> func)
{
    func(&_blob, &_VS);
}

VertexShader::VertexShader(wchar_t *file)
    : VertexShader([file](ID3DBlob **blob, ID3D11VertexShader **vs)
{
    WLOG(L"load vertex shader: " << file);
    *blob = gContent()->getBlob(file, "VS");
    V(gDevice()->CreateVertexShader((*blob)->GetBufferPointer(), (*blob)->GetBufferSize(), NULL, vs));
})
{

}

VertexShader::~VertexShader()
{
    SAFE_RELEASE(_VS);
}

void VertexShader::prepare()
{
    gContext()->VSSetShader(_VS, nullptr, 0);
}

PixelShader::PixelShader(function<void(ID3DBlob **, ID3D11PixelShader **)> func)
{
    func(&_blob, &_PS);
}

PixelShader::PixelShader(wchar_t *file)
    : PixelShader([file](ID3DBlob **blob, ID3D11PixelShader **ps)
{
    WLOG(L"load pixel shader: " << file);
    *blob = gContent()->getBlob(file, "PS");
    V(gDevice()->CreatePixelShader((*blob)->GetBufferPointer(), (*blob)->GetBufferSize(), NULL, ps));
})
{

}

PixelShader::~PixelShader()
{
    SAFE_RELEASE(_PS);
}

void PixelShader::prepare()
{
    gContext()->PSSetShader(_PS, nullptr, 0);
}

MeshVertexShader::MeshVertexShader()
    : VertexShader(L"shaders/MeshVS.hlsl")
{

}

MeshVertexShader::~MeshVertexShader()
{
}

void MeshVertexShader::prepare()
{
	VertexShader::prepare();
}

CascadeShader::CascadeShader()
    : PixelShader(L"shaders/MeshCascade.hlsl")
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

MeshPixelShader::MeshPixelShader()
    : PixelShader(L"shaders/MeshPS.hlsl")
{
}

MeshPixelShader::~MeshPixelShader()
{
}

void MeshPixelShader::prepare()
{
    PixelShader::prepare();
}

FullScreenVertexShader::FullScreenVertexShader()
    : VertexShader(L"shaders/FullScreenQuadVS.hlsl")
{
}

FullScreenVertexShader::~FullScreenVertexShader()
{
}

void FullScreenVertexShader::prepare()
{
    VertexShader::prepare();
}

FullScreenPixelShader::FullScreenPixelShader()
    : PixelShader(L"shaders/TestSRV.hlsl")
{
}

FullScreenPixelShader::~FullScreenPixelShader()
{
}

void FullScreenPixelShader::prepare()
{
    PixelShader::prepare();
}

VarianceBlurX::VarianceBlurX()
    : PixelShader([](ID3DBlob **blob, ID3D11PixelShader **ps)
{
	V(compileShaderFromFile(L"shaders/Variance.hlsl", NULL, "PSBlurX", "ps_5_0", blob));
    V(gDevice()->CreatePixelShader((*blob)->GetBufferPointer(), (*blob)->GetBufferSize(), NULL, ps));
})
{

}

VarianceBlurX::~VarianceBlurX()
{

}

void VarianceBlurX::prepare()
{
	PixelShader::prepare();
}

VarianceBlurY::VarianceBlurY()
    : PixelShader([](ID3DBlob **blob, ID3D11PixelShader **ps)
{
	V(compileShaderFromFile(L"shaders/Variance.hlsl", NULL, "PSBlurY", "ps_5_0", blob));
	V(gDevice()->CreatePixelShader((*blob)->GetBufferPointer(), (*blob)->GetBufferSize(), NULL, ps));
})
{
	PixelShader::prepare();
}

VarianceBlurY::~VarianceBlurY()
{

}

void VarianceBlurY::prepare()
{
	PixelShader::prepare();
}

VarianceBlurVS::VarianceBlurVS()
	: VertexShader(L"shaders/Variance.hlsl")
{

}

VarianceBlurVS::~VarianceBlurVS()
{

}

void VarianceBlurVS::prepare()
{
	VertexShader::prepare();
}
