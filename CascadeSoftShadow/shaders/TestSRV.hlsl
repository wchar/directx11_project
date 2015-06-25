Texture2D g_texture : register(t0);

SamplerState samplePoint : register(s0);

struct PS_INPUT
{    
	float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
};

float4 PSMain(PS_INPUT input) : SV_TARGET
{	
	//return float4(1,0.5,0.5,1);
	// float r = g_texture.Sample(samplePoint, input.texcoord).r;
 //    return float4(1,1,1,1) * r;
	// float4 result = g_texture.Sample(samplePoint, input.texcoord);
	// return result * result * result * result * result* result* 10;
    return g_texture.Sample(samplePoint, input.texcoord);
}


