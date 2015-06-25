struct VS_INPUT
{
    float3 position : POSITION;
    float2 texcoord : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
};

VS_OUTPUT VSMain(VS_INPUT input)
{
    VS_OUTPUT output;
     
	output.position = float4(input.position, 1.0f);
    output.texcoord = input.texcoord;
    
    return output;
}
