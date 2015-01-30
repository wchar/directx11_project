Texture2D g_texture : register(t0);

SamplerState samplePoint : register(s0);

struct PS_INPUT
{    
	float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
};

float4 PSMain(PS_INPUT input) : SV_TARGET
{	
    return g_texture.Sample(samplePoint, input.texcoord);
}


