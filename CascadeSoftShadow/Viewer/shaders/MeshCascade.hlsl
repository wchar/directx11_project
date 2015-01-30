//--------------------------------------------------------------------------------------
// Globals
//--------------------------------------------------------------------------------------
cbuffer cbPerObject : register(b0)
{
	matrix g_mWorldViewProjection;
	matrix g_mWorldView;
	matrix g_mWorld;
	matrix g_mShadow;
}


//--------------------------------------------------------------------------------------
// Input / Output structures
//--------------------------------------------------------------------------------------
struct VS_INPUT
{
	float4 position		: POSITION;
	float3 normal		: NORMAL;
	float2 texcoord		: TEXCOORD0;	
	float3 tangent		: TANGENT;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
};


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VSMain( VS_INPUT input )
{
    VS_OUTPUT output;
    
    output.position = mul(input.position, g_mWorldViewProjection);

    return output;
}

VS_OUTPUT VSMain_SKIN( VS_INPUT input )
{
    VS_OUTPUT output;
    
    output.position = mul(input.position, g_mWorldViewProjection);

    return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float2 PSMain (VS_OUTPUT input) : SV_TARGET 
{
    float2 rt;
    rt.x = input.position.z;
    rt.y = rt.x * rt.x;
    return rt;
}