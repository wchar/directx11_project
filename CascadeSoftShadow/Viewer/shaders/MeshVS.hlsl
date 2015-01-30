//--------------------------------------------------------------------------------------
// Globals
//--------------------------------------------------------------------------------------

cbuffer cbPerObject : register(b0)
{
	matrix g_mWorldViewProjection;
	matrix g_mWorldView;
	matrix g_mWorld;
	matrix g_mShadow;
};

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
	float4 position		: SV_POSITION;
	float3 normal		: NORMAL;
	float2 texcoord		: TEXCOORD0;	
	float3 tangent		: TANGENT;	
	    
	float4 texShadow	: TEXCOORD1;
    float4 interpPos	: TEXCOORD2; 
    float  depth		: TEXCOORD3;    
	float3 worldPos     : TEXCOORD4;
};



//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VSMain(VS_INPUT input)
{
	VS_OUTPUT output;

    output.position = mul( input.position, g_mWorldViewProjection );
    output.normal = mul( input.normal, (float3x3)g_mWorld );
	output.tangent = mul( input.tangent, (float3x3)g_mWorld );
    output.texcoord = input.texcoord;
	    
	// Transform the shadow texture coordinates for all the cascades.
    output.texShadow = mul( input.position, g_mShadow );
    output.interpPos = input.position;   
    output.depth = mul( input.position, g_mWorldView ).z ;
    output.worldPos = mul( input.position, g_mWorld ).xyz;
	return output;
}
