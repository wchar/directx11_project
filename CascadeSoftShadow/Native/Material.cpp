#include "Material.h"
#include "Content.h"
#include "clover.h"

USING_WE

static struct BufferType
{
    struct TYPE
    {
        XMFLOAT3 vDiffuse;
        FLOAT fAlpha;
    };
public:
    BufferType()
    {
		_constantBuffer = nullptr;
    }

    ~BufferType()
    {
        SAFE_RELEASE(_constantBuffer);
    }

	void create()
	{
		// Create the constant buffers
		D3D11_BUFFER_DESC Desc;
		Desc.Usage = D3D11_USAGE_DYNAMIC;
		Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		Desc.MiscFlags = 0;
		Desc.ByteWidth = sizeof(TYPE);
		V(gDevice()->CreateBuffer(&Desc, NULL, &_constantBuffer));
	}

    void map(Material* mtl)
    {
		if (!_constantBuffer)
		{
			this->create();
		}

        auto context = gContext();
        D3D11_MAPPED_SUBRESOURCE mappedResource;
        
        // set PS buffer.
        V(context->Map(_constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));
        TYPE* data = (TYPE*)mappedResource.pData;
        data->fAlpha = mtl->alpha;
        data->vDiffuse = mtl->diffuse;
        context->Unmap(_constantBuffer, 0);
        context->PSSetConstantBuffers(1, 1, &_constantBuffer);
        
        // set texture
        auto tex = mtl->getTex();
        if (tex)
        {
            context->PSSetShaderResources(0, 1, &tex);
        }
    }
private:
    ID3D11Buffer* _constantBuffer;

}s_bt;


//----------------------------------------------------------------------
Material::Material()
{
    _diffuseTex = nullptr;
}

Material* Material::create(FbxMaterial& fbxMtl)
{   
    auto mtl = new Material();
    auto file = fbxMtl.strDiffuseTexture;

    if (wcsnlen_s(file, MAX_PATH) > 0)
    {
        if (gContent()->loadTextureFromFile(file))
        {
            mtl->_diffuseTex = gContent()->getTexture(file);
        }
    }

    return mtl;
}

void Material::prepare()
{
    s_bt.map(this);
}