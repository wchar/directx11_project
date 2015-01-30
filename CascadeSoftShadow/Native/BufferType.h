#include "Renderer.h"

NS_WE_BEGAN

template<class type>
class BufferType
{
public:
	BufferType(function<void(type)> func) :
		_func(func)
	{
		// Create the constant buffers
		D3D11_BUFFER_DESC Desc;
		Desc.Usage = D3D11_USAGE_DYNAMIC;
		Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		Desc.MiscFlags = 0;
		Desc.ByteWidth = sizeof(type);
		V_RETURN(m_pd3dDevice->CreateBuffer(&Desc, NULL, &_constantBuffer));
	}

	~BufferType()
	{
		auto context = gContext();
		D3D11_MAPPED_SUBRESOURCE mappedResource;

		// set PS buffer.
		V(context->Map(_constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));
		type* data = (TYPE*)mappedResource.pData;
		_func(type);
		context->Unmap(_constantBuffer, 0);
		context->PSSetConstantBuffers(1, 1, &_constantBuffer);
	}

	void map()
	{
		_func()
	}
private:
	function<void(type)> _func;
	ID3D11Buffer* _constantBuffer;
};

NS_WE_END