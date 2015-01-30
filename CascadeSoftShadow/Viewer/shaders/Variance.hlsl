//--------------------------------------------------------------------------------------
// Globals
//--------------------------------------------------------------------------------------

cbuffer cbblurVS : register( b2 )
{
	int2		g_iWidthHeight			: packoffset( c0 );
	int		    g_iKernelStart  		: packoffset( c0.z );
	int		    g_iKernelEnd	        : packoffset( c0.w );
};

//--------------------------------------------------------------------------------------
// defines
//--------------------------------------------------------------------------------------

Texture2DArray g_txShadow : register( t5 );
SamplerState g_samShadow : register( s5 );

//--------------------------------------------------------------------------------------
// Input/Output structures
//--------------------------------------------------------------------------------------

struct PSIn
{
    float4      Pos	    : SV_Position;		//Position
    float2      Tex	    : TEXCOORD;		    //Texture coordinate
    float2      ITex    : TEXCOORD2;
};

struct VSIn
{
    uint Pos	: SV_VertexID ;
};


PSIn VSMain(VSIn inn)
{
    PSIn output;

    output.Pos.y  = -1.0f + (inn.Pos%2) * 2.0f ;
    output.Pos.x  = -1.0f + (inn.Pos/2) * 2.0f;
    output.Pos.z = .5;
    output.Pos.w = 1;
    output.Tex.x = inn.Pos/2;
    output.Tex.y = 1.0f - inn.Pos%2;
    output.ITex.x = (float)(g_iWidthHeight.x * output.Tex.x);
    output.ITex.y = (float)(g_iWidthHeight.y * output.Tex.y);
    return output;
}

//float PSDepth

//------------------------------------------------------------------------------
// Logarithmic filtering
//------------------------------------------------------------------------------

float log_conv ( float x0, float X, float y0, float Y )
{
    return (X + log(x0 + (y0 * exp(Y - X))));
}


//--------------------------------------------------------------------------------------
// Pixel shader that performs bump mapping on the final vertex
//--------------------------------------------------------------------------------------
float2 PSBlurX(PSIn input) : SV_Target
{	 
    float2 dep=0;
	float offset = 1.0f/g_iWidthHeight.x;
    for ( int x = g_iKernelStart; x < g_iKernelEnd; ++x ) {
		dep += g_txShadow.Sample( g_samShadow,  float3( input.Tex.x + offset * x, input.Tex.y, 0 ) ).rg;
    }
    dep /= g_iKernelEnd - g_iKernelStart;
    return dep;   
}

//--------------------------------------------------------------------------------------
// Pixel shader that performs bump mapping on the final vertex
//--------------------------------------------------------------------------------------
float2 PSBlurY(PSIn input) : SV_Target
{	 
    float2 dep=0;
	float offset = 1.0f/g_iWidthHeight.y;
    for ( int y = g_iKernelStart; y < g_iKernelEnd; ++y ) {
		dep += g_txShadow.Sample( g_samShadow,  float3( input.Tex.x, input.Tex.y + offset * y, 0 ) ).rg;
    }
    dep /= g_iKernelEnd - g_iKernelStart;
    return dep;  
}



