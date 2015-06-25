#include "Renderer.h"
#include "Shader.h"
#include "clover.h"
#include "D3D.h"
#include "Content.h"
#include "CascadeManager.h"
#include "Sampler.h"

USING_WE

static Shaders *s_getShaders()
{
    static Shaders s_shaders;

    return &s_shaders;
}

static CascadeManager* getCascadeManager()
{
	static CascadeManager cm;
	return &cm;
}

static void s_drawFullScreenQuad()
{
    struct VERTEX
    {
        XMFLOAT3 position;
        XMFLOAT2 tex;
    };

    static ID3D11InputLayout *s_layout = nullptr;
    static ID3D11Buffer *s_vb = nullptr;

	s_getShaders()->prepareToTest();
	gD3D()->resetViews();

	if (!s_vb || !s_layout)
	{
		// create
		auto blob = gContent()->getBlob("FullScreenQuadVS.hlsl");

		// layout
		D3D11_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		UINT numElements = ARRAYSIZE(layout);
		V(gDevice()->CreateInputLayout(layout, numElements, blob->GetBufferPointer(), blob->GetBufferSize(), &s_layout));

		// Create vertex buffer of the screen space
		VERTEX vertices[6] =
		{
			{ XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) }, // right top
			{ XMFLOAT3(1.0f, -1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) }, // right bottom
			{ XMFLOAT3(-1.0f, -1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) }, // left bottom
			{ XMFLOAT3(-1.0f, -1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) }, // left bottom
			{ XMFLOAT3(-1.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) }, // left top
			{ XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) }  // right top
		};
		V(createBuffer(sizeof(VERTEX), 6, vertices, &s_vb));

		STATIC_AUTO_RELEASE(s_layout);
		STATIC_AUTO_RELEASE(s_vb);
    }

    UINT Strides[] = { sizeof(VERTEX) };
    UINT Offsets[] = { 0 };
    gContext()->IASetVertexBuffers(0, 1, &s_vb, Strides, Offsets);
    gContext()->IASetInputLayout(s_layout);
    gContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    gContext()->Draw(6, 0);
}

Renderer::Renderer()
    : _object(nullptr)
{
	_resize = false;
	getCascadeManager()->create();
}

Renderer::~Renderer()
{

}

CascadeManager* Renderer::getManager()
{
	return getCascadeManager();
}

void Renderer::resizeWindow()
{
	_resize = true;
}

ID3D11ShaderResourceView* s_texTest()
{
	static ID3D11ShaderResourceView* p = nullptr;
	if (!p)
	{
		V(createSRVFromFile(L"bg.jpg", &p));
	}
	return p;  
}

void Renderer::onFrameRender()
{
	if (_resize)
	{
		getCascadeManager()->releaseRenderTargetBuffers();
		gD3D()->onResizeWindow();
		getCascadeManager()->createRenderTargetBuffers();
		_resize = false;
	}

    gD3D()->clear();
	gD3D()->resetViews();

	if (_object)
    {
		s_getShaders()->prepareToCascade();
		getCascadeManager()->processCascade(_object);
		
		if (!GetAsyncKeyState('B'))
		{
			getCascadeManager()->processBlur(s_getShaders());
		}
		s_getShaders()->prepareToMesh();
		getCascadeManager()->processScene(_object);
		auto srv = getCascadeManager()->getSceneResult();

		for (int i = 0; i < 8; i++)
		{
			if (GetAsyncKeyState('1' + i))
			{
				srv = getCascadeManager()->getCascadeResult(i);
			}
		}

		gContext()->PSSetShaderResources(0, 1, &srv);
		Sampler::setPoint(0, 1);
        s_drawFullScreenQuad();
    }

    gD3D()->present();
}
