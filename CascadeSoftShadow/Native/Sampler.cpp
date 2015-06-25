#include "Sampler.h"
#include <memory>

NS_WE_BEGAN;

namespace Sampler
{
#define FUNCTION(funcName)							\
	void funcName##(int start, int num)				\
	{												\
	static ID3D11SamplerState *s_sampler = nullptr;	\
		if (!s_sampler)									\
		{												\
		D3D11_SAMPLER_DESC SamDesc;						\
		ZeroMemory(&SamDesc, sizeof(SamDesc));			

#define END\
		V(gDevice()->CreateSamplerState(&SamDesc, &s_sampler));	\
		STATIC_AUTO_RELEASE(s_sampler);							\
	}														\
	gContext()->PSSetSamplers(start, num, &s_sampler);		\
}


	FUNCTION(setPoint)
	{
		SamDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		SamDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		SamDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		SamDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		SamDesc.MaxAnisotropy = 1;
		SamDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		SamDesc.MaxLOD = D3D11_FLOAT32_MAX;
	}
	END;

	FUNCTION(setLinear)
	{
		SamDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		SamDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		SamDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		SamDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		SamDesc.MaxAnisotropy = 1;
		SamDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		SamDesc.MaxLOD = D3D11_FLOAT32_MAX;
	}
	END;

	FUNCTION(setShadowPoint)
	{
		SamDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		SamDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		SamDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		SamDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
		SamDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	}
	END;

	FUNCTION(setShadowAnisotropic16)
	{
		SamDesc.Filter = D3D11_FILTER_ANISOTROPIC;
		SamDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		SamDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		SamDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
		SamDesc.MaxAnisotropy = 16;
		SamDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	}
	END;
}

NS_WE_END;