//--------------------------------------------------------------------------------------
// Globals
//--------------------------------------------------------------------------------------


cbuffer cbAllShadowData : register(b0)
{
    float4          g_vCascadeOffset[8];
    float4          g_vCascadeScale[8];

    int             g_nCascadeLevels; 
    int             g_iVisualizeCascades; 

    float           g_fMinBorderPadding;     
    float           g_fMaxBorderPadding;
		                                   
    float           g_fCascadeBlendArea; 
    float           g_fTexelSize; 
    float           g_fNativeTexelSizeInX;
	//float			g_fPadding;	

    float4          g_fCascadeFrustumsEyeSpaceDepthsData[2];  
	
	static float    g_fCascadeFrustumsEyeSpaceDepths[8] = (float[8])g_fCascadeFrustumsEyeSpaceDepthsData;
    
	float3          g_vLightDir;
    float3          g_vCameraPosition;
	
	float2			g_fPadding2;	
};

cbuffer cbAllShadowData : register(b3)
{
    int CASCADE_COUNT_FLAG;
    int SELECT_CASCADE_BY_INTERVAL_FLAG;
    int BLEND_BETWEEN_CASCADE_LAYERS_FLAG;
    int g_iPadding;
};


//--------------------------------------------------------------------------------------
// Textures and Samplers
//--------------------------------------------------------------------------------------
Texture2D g_texDiffuse      : register(t0);
// Texture2D g_texNormal       : register(t1);
// Texture2D g_texEmissive     : register(t2);

Texture2DArray g_texShadow  : register(t5);

SamplerState g_samLinear    : register(s0);
SamplerState g_samShadow    : register(s5);


//--------------------------------------------------------------------------------------
// Input / Output structures
//--------------------------------------------------------------------------------------
struct PS_INPUT
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


static const float4 vCascadeColorsMultiplier[8] = 
{
    float4 ( 1.5f, 0.0f, 0.0f, 1.0f ),
    float4 ( 0.0f, 1.5f, 0.0f, 1.0f ),
    float4 ( 0.0f, 0.0f, 5.5f, 1.0f ),
    float4 ( 1.5f, 0.0f, 5.5f, 1.0f ),
    float4 ( 1.5f, 1.5f, 0.0f, 1.0f ),
    float4 ( 1.0f, 1.0f, 1.0f, 1.0f ),
    float4 ( 0.0f, 1.0f, 5.5f, 1.0f ),
    float4 ( 0.5f, 3.5f, 0.75f, 1.0f )
};

void ComputeCoordinatesTransform(in int iCascadeIndex,
                                  in float4 InterpolatedPosition, 
                                  in out float4 vShadowTexCoord,
                                  in out float4 vShadowTexCoordViewSpace) 
{
    // Now that we know the correct map, we can transform the world space position of the current fragment                
    if(SELECT_CASCADE_BY_INTERVAL_FLAG) 
    {
        vShadowTexCoord = vShadowTexCoordViewSpace * g_vCascadeScale[iCascadeIndex];
        vShadowTexCoord += g_vCascadeOffset[iCascadeIndex];
    }  
    vShadowTexCoord.w = vShadowTexCoord.z; // We put the z value in w so that we can index the texture array with Z.
    vShadowTexCoord.z = iCascadeIndex;
    
} 

//--------------------------------------------------------------------------------------
// Use PCF to sample the depth map and return a percent lit value.
//--------------------------------------------------------------------------------------
void CalculateVarianceShadow (in float4 vShadowTexCoord, in float4 vShadowMapTextureCoordViewSpace, int iCascade, out float fPercentLit) 
{
    fPercentLit = 0.0f;
    // This loop could be unrolled, and texture immediate offsets could be used if the kernel size were fixed.
    // This would be a performance improvment.
	        
    float2 mapDepth = 0;


    // In orderto pull the derivative out of divergent flow control we calculate the 
    // derivative off of the view space coordinates an then scale the deriviative.
    
    float3 vShadowTexCoordDDX = 
		ddx(vShadowMapTextureCoordViewSpace);
    vShadowTexCoordDDX *= g_vCascadeScale[iCascade].xyz; 
    float3 vShadowTexCoordDDY = 
		ddy(vShadowMapTextureCoordViewSpace);
    vShadowTexCoordDDY *= g_vCascadeScale[iCascade].xyz; 
    
    mapDepth += g_texShadow.SampleGrad(g_samShadow, vShadowTexCoord.xyz, 
									   vShadowTexCoordDDX,
									   vShadowTexCoordDDY);
    // The sample instruction uses gradients for some filters.
		        
    float  fAvgZ  = mapDepth.x; // Filtered z
    float  fAvgZ2 = mapDepth.y; // Filtered z-squared
    
    if (vShadowTexCoord.w <= fAvgZ) // We put the z value in w so that we can index the texture array with Z.
    {
        fPercentLit = 1;
	}
	else 
	{
	    float variance = (fAvgZ2) - (fAvgZ * fAvgZ);
        variance       = min(1.0f, max(0.0f, variance + 0.00001f));
    
        float mean     = fAvgZ;
        float d        = vShadowTexCoord.w - mean; // We put the z value in w so that we can index the texture array with Z.
        float p_max    = variance / (variance + d*d);

        // To combat light-bleeding, experiment with raising p_max to some power
        // (Try values from 0.1 to 100.0, if you like.)
        fPercentLit = pow(p_max, 18);
	    
	}
}

//--------------------------------------------------------------------------------------
// Calculate amount to blend between two cascades and the band where blending will occure.
//--------------------------------------------------------------------------------------
void CalculateBlendAmountForInterval (in int iNextCascadeIndex, 
                                       in out float fPixelDepth, 
                                       in out float fCurrentPixelsBlendBandLocation,
                                       out float fBlendBetweenCascadesAmount
                                      ) 
{

    // We need to calculate the band of the current shadow map where it will fade into the next cascade.
    // We can then early out of the expensive PCF for loop.
    // 
    float fBlendInterval = g_fCascadeFrustumsEyeSpaceDepths[ iNextCascadeIndex - 1 ];
    if(iNextCascadeIndex > 1) 
    {
        fPixelDepth -= g_fCascadeFrustumsEyeSpaceDepths[ iNextCascadeIndex-2 ];
        fBlendInterval -= g_fCascadeFrustumsEyeSpaceDepths[ iNextCascadeIndex-2 ];
    } 
    // The current pixel's blend band location will be used to determine when we need to blend and by how much.
    fCurrentPixelsBlendBandLocation = fPixelDepth / fBlendInterval;
    fCurrentPixelsBlendBandLocation = 1.0f - fCurrentPixelsBlendBandLocation;
    // The fBlendBetweenCascadesAmount is our location in the blend band.
    fBlendBetweenCascadesAmount = fCurrentPixelsBlendBandLocation / g_fCascadeBlendArea;
}


//--------------------------------------------------------------------------------------
// Calculate amount to blend between two cascades and the band where blending will occure.
//--------------------------------------------------------------------------------------
void CalculateBlendAmountForMap (in float4 vShadowMapTextureCoord, 
                                  in out float fCurrentPixelsBlendBandLocation,
                                  out float fBlendBetweenCascadesAmount) 
{
    // Calcaulte the blend band for the map based selection.
    float2 distanceToOne = float2 (1.0f - vShadowMapTextureCoord.x, 1.0f - vShadowMapTextureCoord.y);
    fCurrentPixelsBlendBandLocation = min(vShadowMapTextureCoord.x, vShadowMapTextureCoord.y);
    float fCurrentPixelsBlendBandLocation2 = min(distanceToOne.x, distanceToOne.y);
    fCurrentPixelsBlendBandLocation = min(fCurrentPixelsBlendBandLocation, fCurrentPixelsBlendBandLocation2);
    fBlendBetweenCascadesAmount = fCurrentPixelsBlendBandLocation / g_fCascadeBlendArea;
}

//--------------------------------------------------------------------------------------
// Calculate the shadow based on several options and rende the scene.
//--------------------------------------------------------------------------------------

float4 PSMain(PS_INPUT input) : SV_TARGET
{
    float depth = input.position.z / input.position.w * 100;
    return float4(depth, depth, depth, 1);
    // float4 vDiffuse = g_vDiffuse; 
    // if (bDiffuseTexture)
    // { 
    //     vDiffuse = g_texDiffuse.Sample(g_samLinear, input.texcoord);
    // }
    float4 vVisualizeCascadeColor = float4(0.0f,0.0f,0.0f,1.0f);

    float4 vShadowMapTextureCoordViewSpace = 0.0f;
    float4 vShadowMapTextureCoord = 0.0f;
    float4 vShadowMapTextureCoord_blend = 0.0f;
    
    float fPercentLit = 0.0f;
    float fPercentLit_blend = 0.0f;

    int iCascadeFound = 0;
    int iCurrentCascadeIndex=1;
    int iNextCascadeIndex = 0;

    float fCurrentPixelDepth;

    // The interval based selection technique compares the pixel's depth against the frustum's cascade divisions.
    fCurrentPixelDepth = input.depth;
    
    // This for loop is not necessary when the frustum is uniformaly divided and interval based selection is used.
    // In this case fCurrentPixelDepth could be used as an array lookup into the correct frustum. 
    vShadowMapTextureCoordViewSpace = input.texShadow;
    
    
    if(SELECT_CASCADE_BY_INTERVAL_FLAG) 
    {
        iCurrentCascadeIndex = 0;
        if (CASCADE_COUNT_FLAG > 1) 
        {
            float4 vCurrentPixelDepth = input.depth;
            float4 fComparison = (vCurrentPixelDepth > g_fCascadeFrustumsEyeSpaceDepthsData[0]);
            float4 fComparison2 = (vCurrentPixelDepth > g_fCascadeFrustumsEyeSpaceDepthsData[1]);
            float fIndex = dot(
                            float4(CASCADE_COUNT_FLAG > 0,
                                    CASCADE_COUNT_FLAG > 1, 
                                    CASCADE_COUNT_FLAG > 2, 
                                    CASCADE_COUNT_FLAG > 3)
                            , fComparison)
                         + dot(
                            float4(
                                    CASCADE_COUNT_FLAG > 4,
                                    CASCADE_COUNT_FLAG > 5,
                                    CASCADE_COUNT_FLAG > 6,
                                    CASCADE_COUNT_FLAG > 7)
                            , fComparison2) ;
                                    
            fIndex = min(fIndex, CASCADE_COUNT_FLAG - 1);
            iCurrentCascadeIndex = (int)fIndex;
        }
    }
    
    if (!SELECT_CASCADE_BY_INTERVAL_FLAG) 
    {
        iCurrentCascadeIndex = 0;
        if (CASCADE_COUNT_FLAG == 1) 
        {
            vShadowMapTextureCoord = vShadowMapTextureCoordViewSpace * g_vCascadeScale[0];
            vShadowMapTextureCoord += g_vCascadeOffset[0];
        }
        if (CASCADE_COUNT_FLAG > 1) {
            for(int iCascadeIndex = 0; iCascadeIndex < CASCADE_COUNT_FLAG && iCascadeFound == 0; ++iCascadeIndex) 
            {
                vShadowMapTextureCoord = vShadowMapTextureCoordViewSpace * g_vCascadeScale[iCascadeIndex];
                vShadowMapTextureCoord += g_vCascadeOffset[iCascadeIndex];

                if (min(vShadowMapTextureCoord.x, vShadowMapTextureCoord.y) > g_fMinBorderPadding
                  && max(vShadowMapTextureCoord.x, vShadowMapTextureCoord.y) < g_fMaxBorderPadding)
                { 
                    iCurrentCascadeIndex = iCascadeIndex;   
                    iCascadeFound = 1; 
                }
            }
        }
    }    

    vVisualizeCascadeColor = vCascadeColorsMultiplier[iCurrentCascadeIndex];

    ComputeCoordinatesTransform(iCurrentCascadeIndex, input.interpPos, vShadowMapTextureCoord, vShadowMapTextureCoordViewSpace );    
                                             
    if(BLEND_BETWEEN_CASCADE_LAYERS_FLAG && CASCADE_COUNT_FLAG > 1) 
    {
        // Repeat text coord calculations for the next cascade. 
        // The next cascade index is used for blurring between maps.
        iNextCascadeIndex = min (CASCADE_COUNT_FLAG - 1, iCurrentCascadeIndex + 1); 
        if(!SELECT_CASCADE_BY_INTERVAL_FLAG) 
        {
            vShadowMapTextureCoord_blend = vShadowMapTextureCoordViewSpace * g_vCascadeScale[iNextCascadeIndex];
            vShadowMapTextureCoord_blend += g_vCascadeOffset[iNextCascadeIndex];
        }
        ComputeCoordinatesTransform(iNextCascadeIndex, input.interpPos, vShadowMapTextureCoord_blend, vShadowMapTextureCoordViewSpace);  
    }            
    float fBlendBetweenCascadesAmount = 1.0f;
    float fCurrentPixelsBlendBandLocation = 1.0f;
    
    if(SELECT_CASCADE_BY_INTERVAL_FLAG) 
    {
        if(CASCADE_COUNT_FLAG > 1 && BLEND_BETWEEN_CASCADE_LAYERS_FLAG) 
        {
            CalculateBlendAmountForInterval (iNextCascadeIndex, fCurrentPixelDepth, 
                fCurrentPixelsBlendBandLocation, fBlendBetweenCascadesAmount);          
        }   
    }
    else 
    {
        if(CASCADE_COUNT_FLAG > 1 && BLEND_BETWEEN_CASCADE_LAYERS_FLAG) 
        {
            CalculateBlendAmountForMap (vShadowMapTextureCoord, 
                fCurrentPixelsBlendBandLocation, fBlendBetweenCascadesAmount);
        }   
    }
    
    // Because the Z coordinate specifies the texture array,
    // the derivative will be 0 when there is no divergence
    //float fDivergence = abs(ddy(vShadowMapTextureCoord.z)) +  abs(ddx(vShadowMapTextureCoord.z));
    CalculateVarianceShadow (vShadowMapTextureCoord, vShadowMapTextureCoordViewSpace, 
								iCurrentCascadeIndex, fPercentLit);
								
    // We repeat the calcuation for the next cascade layer, when blending between maps.
    if(BLEND_BETWEEN_CASCADE_LAYERS_FLAG  && CASCADE_COUNT_FLAG > 1) 
    {
        if(fCurrentPixelsBlendBandLocation < g_fCascadeBlendArea) 
        {  // the current pixel is within the blend band.

			// Because the Z coordinate species the texture array,
			// the derivative will be 0 when there is no divergence
			float fDivergence = abs(ddy(vShadowMapTextureCoord_blend.z)) +  
				abs(ddx(vShadowMapTextureCoord_blend.z));
            CalculateVarianceShadow (vShadowMapTextureCoord_blend, vShadowMapTextureCoordViewSpace, 
										iNextCascadeIndex, fPercentLit_blend);

            // Blend the two calculated shadows by the blend amount.
            fPercentLit = lerp(fPercentLit_blend, fPercentLit, fBlendBetweenCascadesAmount); 

        }   
    }    
    
    if( !g_iVisualizeCascades ) vVisualizeCascadeColor = float4( 1.0f, 1.0f, 1.0f, 1.0f );

    float3 vLightDir1 = float3(-1.0f, 1.0f, -1.0f); 
    float3 vLightDir2 = float3(1.0f, 1.0f, -1.0f); 
    float3 vLightDir3 = float3(0.0f, -1.0f, 0.0f);
    float3 vLightDir4 = float3(1.0f, 1.0f, 1.0f);     
    // Some ambient-like lighting.
    float fLighting = saturate(dot(vLightDir1 , input.normal))*0.05f +
                      saturate(dot(vLightDir2 , input.normal))*0.05f +
                      saturate(dot(vLightDir3 , input.normal))*0.05f +
                      saturate(dot(vLightDir4 , input.normal))*0.05f ;
    
    float4 vShadowLighting = fLighting * 0.5f;
    fLighting += saturate(dot(g_vLightDir , input.normal));
    fLighting = lerp(0.1f, fLighting, fPercentLit);
    
    float4 color = vVisualizeCascadeColor;//vDiffuse;
    color *= fLighting;
    return max(color, float4(0.06f, 0.06f, 0.06f, 1.0f));
}
