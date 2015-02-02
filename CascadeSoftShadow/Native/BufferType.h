#pragma once

#include "Renderer.h"
#include "clover.h"

NS_WE_BEGAN

template<class type>
class BufferType
{
public:
	BufferType(function<void(type*)> func)
		: _func(func)
	{
		// Create the constant buffers
		D3D11_BUFFER_DESC Desc;
		Desc.Usage = D3D11_USAGE_DYNAMIC;
		Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		Desc.MiscFlags = 0;
		Desc.ByteWidth = sizeof(type);
		V(gDevice()->CreateBuffer(&Desc, NULL, &_constantBuffer));
	}

	~BufferType()
	{
		SAFE_RELEASE(_constantBuffer);
	}

	void map()
	{
		auto context = gContext();
		D3D11_MAPPED_SUBRESOURCE mappedResource;

		// set PS buffer.
		V(context->Map(_constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));
		type* data = (type*)mappedResource.pData;
		_func(data);
		context->Unmap(_constantBuffer, 0);
	}

	void setPS(int start, int num)
	{
		gContext()->PSSetConstantBuffers(start, num, &_constantBuffer);
	}
	void setVS(int start, int num)
	{
		gContext()->VSSetConstantBuffers(start, num, &_constantBuffer);
	}
private:
	function<void(type*)> _func;
	ID3D11Buffer* _constantBuffer;
};

NS_WE_END