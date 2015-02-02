#include "Sampler.h"
#include <memory>

NS_WE_BEGAN;

template<class type>
class auto_release_ptr
{
public:
	auto_release_ptr(type* com)
		: _com(com)
	{}
	~auto_release_ptr()
	{
		SAFE_RELEASE(_com);
	}
private:
	type* _com;
};

#define STATIC_AUTO_RELEASE(x) static auto_release_ptr<IUnknown> s_auto_ptr(s_sampler);

namespace Sampler
{

void setLinear(int start, int num)
{
    static ID3D11SamplerState *s_sampler = nullptr;

    if (!s_sampler)
    {
        D3D11_SAMPLER_DESC SamDesc;
        SamDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        SamDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        SamDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        SamDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        SamDesc.MipLODBias = 0.0f;
        SamDesc.MaxAnisotropy = 1;
        SamDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
        SamDesc.BorderColor[0] = SamDesc.BorderColor[1] = SamDesc.BorderColor[2] = SamDesc.BorderColor[3] = 0;
        SamDesc.MinLOD = 0;
        SamDesc.MaxLOD = D3D11_FLOAT32_MAX;

        V(gDevice()->CreateSamplerState(&SamDesc, &s_sampler));
		STATIC_AUTO_RELEASE(s_sampler);
    }

    gContext()->PSSetSamplers(start, num, &s_sampler);
}

void setShadowPoint(int start, int num)
{
    static ID3D11SamplerState *s_sampler = nullptr;

    if (!s_sampler)
    {
        D3D11_SAMPLER_DESC SamDescShad =
        {
            D3D11_FILTER_MIN_MAG_MIP_POINT,// D3D11_FILTER Filter;
            D3D11_TEXTURE_ADDRESS_CLAMP, //D3D11_TEXTURE_ADDRESS_MODE AddressU;
            D3D11_TEXTURE_ADDRESS_CLAMP, //D3D11_TEXTURE_ADDRESS_MODE AddressV;
            D3D11_TEXTURE_ADDRESS_BORDER, //D3D11_TEXTURE_ADDRESS_MODE AddressW;
            0,//FLOAT MipLODBias;
            0,//UINT MaxAnisotropy;
            D3D11_COMPARISON_NEVER, //D3D11_COMPARISON_FUNC ComparisonFunc;
            0.0, 0.0, 0.0, 0.0,//FLOAT BorderColor[ 4 ];
            0,//FLOAT MinLOD;
            0//FLOAT MaxLOD;
        };

        V(gDevice()->CreateSamplerState(&SamDescShad, &s_sampler));
		STATIC_AUTO_RELEASE(s_sampler);
    }

    gContext()->PSSetSamplers(start, num, &s_sampler);
}

void setShadowAnisotropic16(int start, int num)
{
    static ID3D11SamplerState *s_sampler = nullptr;

    if (!s_sampler)
    {
        D3D11_SAMPLER_DESC SamDescShad =
        {
            D3D11_FILTER_ANISOTROPIC,// D3D11_FILTER Filter;
            D3D11_TEXTURE_ADDRESS_CLAMP, //D3D11_TEXTURE_ADDRESS_MODE AddressU;
            D3D11_TEXTURE_ADDRESS_CLAMP, //D3D11_TEXTURE_ADDRESS_MODE AddressV;
            D3D11_TEXTURE_ADDRESS_BORDER, //D3D11_TEXTURE_ADDRESS_MODE AddressW;
            0,//FLOAT MipLODBias;
            16,//UINT MaxAnisotropy;
            D3D11_COMPARISON_NEVER, //D3D11_COMPARISON_FUNC ComparisonFunc;
            0.0, 0.0, 0.0, 0.0,//FLOAT BorderColor[ 4 ];
            0,//FLOAT MinLOD;
            0//FLOAT MaxLOD;
        };

        V(WE::gDevice()->CreateSamplerState(&SamDescShad, &s_sampler));
		STATIC_AUTO_RELEASE(s_sampler);
    }

    gContext()->PSSetSamplers(start, num, &s_sampler);
}

}

NS_WE_END;