#pragma once

#include "clover.h"
#include "Mesh.h"
#include "Shader.h"

NS_WE_BEGAN

static const int MAX_CASCADES = 8;

struct CascadeConfig
{
    // Cascade config info
    INT         cascadeLevels;
    INT         bufferSize;

    INT         cascadePartitionsMax;
    INT         cascadePartitionsZeroToOne[MAX_CASCADES]; // Values are 0 to 100 and represent a percent of the frstum
    INT         shadowBlurSize;

    BOOL        moveLightTexelSize;
    INT         visualizeCascades;
    INT         mapSelection;
    INT         blurBetweenCascades;
    FLOAT       blurBetweenCascadesAmount;
};

class DLL_EXPORT CascadeManager
{
public:
    CascadeManager();
    ~CascadeManager();

    bool create();
    void release();

    CascadeConfig cascadeConfig;

    bool recreateCascadeBuffers();
    bool createRenderTargetBuffers();

	void releaseCascadeBuffers();
    void releaseRenderTargetBuffers();

	void setScenesAABB(XMFLOAT3 max, XMFLOAT3 min)
    {
        _sceneAABBMin = max;
        _sceneAABBMin = min;
    }

    void processCascade(Mesh* mesh);
    void processBlur(Shaders* shaders);
	void processScene(Mesh* mesh);

	ID3D11ShaderResourceView* getCascadeResult(int i)
	{
		return _cascadedShadowMapVarianceSRVArrayAll[i];
	}
	ID3D11ShaderResourceView* getSceneResult()
	{
		return _shaderResourceView;
	}
protected:
	XMFLOAT3 _sceneAABBMin;
	XMFLOAT3 _sceneAABBMax;
	
	void computeMatrices();
	bool sceneSetConstantBuffer();

	D3D11_VIEWPORT              _renderVP[MAX_CASCADES];
	D3D11_VIEWPORT              _renderOneTileVP;
	D3D11_VIEWPORT              _viewport;

	XMFLOAT4X4                  _shadowProj[MAX_CASCADES];
	XMFLOAT4X4                  _shadowView;

    ID3D11RasterizerState       *_rsScene;
    ID3D11RasterizerState       *_rsShadow;

	FLOAT                       _cascadePartitionsFrustum[MAX_CASCADES];
    ID3D11Texture2D             *_cascadedShadowMapVarianceTextureArray;
    ID3D11RenderTargetView      *_cascadedShadowMapVarianceRTVArrayAll[MAX_CASCADES];
    ID3D11ShaderResourceView    *_cascadedShadowMapVarianceSRVArrayAll[MAX_CASCADES];
    ID3D11ShaderResourceView    *_cascadedShadowMapVarianceSRVArraySingle;

    ID3D11Texture2D             *_temporaryShadowDepthBufferTexture;
    ID3D11DepthStencilView      *_temporaryShadowDepthBufferDSV;

    ID3D11Texture2D             *_cascadedShadowMapTempBlurTexture;
    ID3D11RenderTargetView      *_cascadedShadowMapTempBlurRTV;
    ID3D11ShaderResourceView    *_cascadedShadowMapTempBlurSRV;

	// render target
    ID3D11Texture2D             *_renderTargetTexture;
    ID3D11RenderTargetView      *_renderTargetView;
    ID3D11ShaderResourceView    *_shaderResourceView;
    ID3D11Texture2D             *_depthBuffer;
    ID3D11DepthStencilView      *_depthStencilView;
    ID3D11ShaderResourceView    *_depthShaderResourceView;
};

NS_WE_END