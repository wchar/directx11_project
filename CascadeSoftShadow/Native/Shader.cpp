#include "Shader.h"
#include "clover.h"
#include "Content.h"
#include "BufferType.h"

USING_WE

static void s_getPathJoint(wchar_t* fullPath, wchar_t* file)
{
	wcscpy_s(fullPath, MAX_PATH, L"..\\shaders\\");
	wcscat_s(fullPath, MAX_PATH, file);
	WLOG("loading:" << file);
}

VertexShader::VertexShader(function<void(ID3DBlob **, ID3D11VertexShader **)> func)
{
	func(&_blob, &_VS);
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

PixelShader::~PixelShader()
{
	SAFE_RELEASE(_PS);
}

void PixelShader::prepare()
{
	gContext()->PSSetShader(_PS, nullptr, 0);
}


//MeshVertexShader::MeshVertexShader() : VertexShader(L"MeshVS.hlsl"){}
MeshVertexShader::MeshVertexShader() : VertexShader([](ID3DBlob **blob, ID3D11VertexShader **vs)
{
	static wchar_t path[MAX_PATH];
	s_getPathJoint(path, L"MeshVS.hlsl");
	
	V(compileShaderFromFile(path, NULL, "VSMain", "vs_5_0", blob));
	V(gDevice()->CreateVertexShader((*blob)->GetBufferPointer(), (*blob)->GetBufferSize(), NULL, vs));

	gContent()->addBlob("MeshVertexShader", *blob);
}){}

//CascadeShader::CascadeShader() : PixelShader(L"MeshCascade.hlsl"){}
CascadeShader::CascadeShader() : PixelShader([](ID3DBlob **blob, ID3D11PixelShader **ps)
{
	wchar_t path[MAX_PATH];
	s_getPathJoint(path, L"MeshCascade.hlsl");
	V(compileShaderFromFile(path, NULL, "PSMain", "ps_5_0", blob));
	V(gDevice()->CreatePixelShader((*blob)->GetBufferPointer(), (*blob)->GetBufferSize(), NULL, ps));
}){}

//MeshPixelShader::MeshPixelShader() : PixelShader(L"MeshPS.hlsl"){}
MeshPixelShader::MeshPixelShader() : PixelShader([](ID3DBlob **blob, ID3D11PixelShader **ps)
{
	wchar_t path[MAX_PATH];
	s_getPathJoint(path, L"MeshPS.hlsl");
	V(compileShaderFromFile(path, NULL, "PSMain", "ps_5_0", blob));
	V(gDevice()->CreatePixelShader((*blob)->GetBufferPointer(), (*blob)->GetBufferSize(), NULL, ps));
}){}


FullScreenVertexShader::FullScreenVertexShader() : VertexShader([](ID3DBlob **blob, ID3D11VertexShader **vs)
{
	static wchar_t path[MAX_PATH];
	s_getPathJoint(path, L"FullScreenQuadVS.hlsl");
	V(compileShaderFromFile(path, NULL, "VSMain", "vs_5_0", blob));
	V(gDevice()->CreateVertexShader((*blob)->GetBufferPointer(), (*blob)->GetBufferSize(), NULL, vs));

	gContent()->addBlob("FullScreenQuadVS.hlsl", *blob);
}){}

FullScreenPixelShader::FullScreenPixelShader() : PixelShader([](ID3DBlob **blob, ID3D11PixelShader **ps)
{
	wchar_t path[MAX_PATH];
	s_getPathJoint(path, L"TestSRV.hlsl");
	V(compileShaderFromFile(path, NULL, "PSMain", "ps_5_0", blob));
	V(gDevice()->CreatePixelShader((*blob)->GetBufferPointer(), (*blob)->GetBufferSize(), NULL, ps));
}){}

VarianceBlurVS::VarianceBlurVS() : VertexShader([](ID3DBlob **blob, ID3D11VertexShader **vs)
{
	static wchar_t path[MAX_PATH];
	s_getPathJoint(path, L"Variance.hlsl");
	V(compileShaderFromFile(path, NULL, "VSMain", "vs_5_0", blob));
	V(gDevice()->CreateVertexShader((*blob)->GetBufferPointer(), (*blob)->GetBufferSize(), NULL, vs));
}){}

VarianceBlurX::VarianceBlurX() : PixelShader([](ID3DBlob **blob, ID3D11PixelShader **ps)
{
	wchar_t path[MAX_PATH];
	s_getPathJoint(path, L"Variance.hlsl");
	V(compileShaderFromFile(path, NULL, "PSBlurX", "ps_5_0", blob));
	V(gDevice()->CreatePixelShader((*blob)->GetBufferPointer(), (*blob)->GetBufferSize(), NULL, ps));
}){}

VarianceBlurY::VarianceBlurY() : PixelShader([](ID3DBlob **blob, ID3D11PixelShader **ps)
{
	wchar_t path[MAX_PATH];
	s_getPathJoint(path, L"Variance.hlsl");
	V(compileShaderFromFile(path, NULL, "PSBlurY", "ps_5_0", blob));
	V(gDevice()->CreatePixelShader((*blob)->GetBufferPointer(), (*blob)->GetBufferSize(), NULL, ps));
}){}


