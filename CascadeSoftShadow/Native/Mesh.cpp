#include "Mesh.h"
#include "FbxImporter.h"
#include "clover.h"
#include "Content.h"

USING_WE

Mesh* Mesh::createFromFbx(wchar_t* file)
{
    Mesh* mesh = new Mesh();
    if (mesh->loadFbx(file))
    {
        return mesh;
    }
    else
    {
        SAFE_DELETE(mesh);
        return nullptr;
    }
}

Mesh::Mesh()
{
    _vertexBuffer = nullptr;
    _indexBuffer = nullptr;
}

Mesh::~Mesh()
{
    SAFE_RELEASE(_vertexBuffer);
    SAFE_RELEASE(_indexBuffer);

    for (auto mtl : _materials)
    {
        SAFE_DELETE(mtl);
    }
}

bool Mesh::loadFbx(wchar_t* file)
{
    FbxMeshImporter* imp = FbxMeshImporter::create(file);
    if (!imp)
    {
        return false;
    }

    // Create VB
    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));
    bd.ByteWidth = (UINT)(sizeof(Vertex) * imp->vertices.size());
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;
    bd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA initData;
    ZeroMemory(&initData, sizeof(initData));
    initData.pSysMem = imp->vertices.data();
    V(gDevice()->CreateBuffer(&bd, &initData, &_vertexBuffer));

    // Create IB
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = (UINT)(sizeof(DWORD) * imp->indices.size());
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;
    initData.pSysMem = imp->indices.data();
    V(gDevice()->CreateBuffer(&bd, &initData, &_indexBuffer));

    // Craete Materials
    for (FbxMaterial& fbxMtl : imp->materials)
    {
        _materials.push_back(Material::create(fbxMtl));
    }
    
    // Create Subsets
    for (Subset& subset : imp->subsets)
    {
        _subsets.push_back(subset);
    }

    SAFE_DELETE(imp);
    return true;
}

void s_setInputLayout()
{
	static ID3D11InputLayout* s_layout = nullptr;

	if (!s_layout)
	{
		// Vertex layout
		D3D11_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		auto blob = gContent()->getBlob("MeshVertexShader");
		V(gDevice()->CreateInputLayout(
			layout, ARRAYSIZE(layout),
			blob->GetBufferPointer(),
			blob->GetBufferSize(),
			&s_layout));
	}

	gContext()->IASetInputLayout(s_layout);
}

void Mesh::drawSubset()
{
    UINT strides[] = { sizeof(Vertex) };
    UINT offsets[] = { 0 };
    gContext()->IASetVertexBuffers(0, 1, &_vertexBuffer, strides, offsets);
    gContext()->IASetIndexBuffer(_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	s_setInputLayout();

    for (auto& subset : _subsets)
    {
        Material* mtl = _materials[subset.materialID];
        mtl->prepare();

        // Draw subset.
        gContext()->DrawIndexed(subset.indexCount, subset.indexStart, subset.vertexStart);
    }
}
