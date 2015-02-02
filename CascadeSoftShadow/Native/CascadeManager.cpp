#include "CascadeManager.h"
#include "D3D.h"
#include "Camera.h"
#include "BufferType.h"
#include "Sampler.h"

USING_WE;

static const INT MAX_BUFFER_SIZE = 2048;

static const XMVECTORF32 s_FLTMAX = { FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX };
static const XMVECTORF32 s_FLTMIN = { -FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX };
static const XMVECTORF32 s_halfVector = { 0.5f, 0.5f, 0.5f, 0.5f };

CascadeManager::CascadeManager()
    : _rsScene(nullptr)
    , _rsShadow(nullptr)
    , _cascadedShadowMapVarianceTextureArray(nullptr)
    , _cascadedShadowMapVarianceSRVArraySingle(nullptr)
    , _temporaryShadowDepthBufferTexture(nullptr)
    , _temporaryShadowDepthBufferDSV(nullptr)
    , _cascadedShadowMapTempBlurTexture(nullptr)
    , _cascadedShadowMapTempBlurRTV(nullptr)
    , _cascadedShadowMapTempBlurSRV(nullptr)
    , _renderTargetTexture(nullptr)
    , _renderTargetView(nullptr)
    , _shaderResourceView(nullptr)
    , _depthBuffer(nullptr)
    , _depthStencilView(nullptr)
    , _depthShaderResourceView(nullptr)
{
    for (int i = 0; i < MAX_CASCADES; i++)
    {
        _cascadedShadowMapVarianceRTVArrayAll[i] = nullptr;
        _cascadedShadowMapVarianceSRVArrayAll[i] = nullptr;
    }

    cascadeConfig.bufferSize = 512;
    cascadeConfig.cascadeLevels = 5;
    cascadeConfig.cascadePartitionsZeroToOne[0] = 10;
    cascadeConfig.cascadePartitionsZeroToOne[1] = 20;
    cascadeConfig.cascadePartitionsZeroToOne[2] = 40;
    cascadeConfig.cascadePartitionsZeroToOne[3] = 100;
    cascadeConfig.cascadePartitionsZeroToOne[4] = 100;
    cascadeConfig.cascadePartitionsZeroToOne[5] = 100;
    cascadeConfig.cascadePartitionsZeroToOne[6] = 100;
    cascadeConfig.cascadePartitionsZeroToOne[7] = 100;
    cascadeConfig.cascadePartitionsMax = 100;
    cascadeConfig.visualizeCascades = false;
    cascadeConfig.shadowBlurSize = 15;
    cascadeConfig.blurBetweenCascades = 0;
    cascadeConfig.blurBetweenCascadesAmount = 0.005;
    cascadeConfig.moveLightTexelSize = true;

    for (INT index = 0; index < MAX_CASCADES; ++index)
    {
        _renderVP[index].Height = (FLOAT)cascadeConfig.bufferSize;
        _renderVP[index].Width = (FLOAT)cascadeConfig.bufferSize;
        _renderVP[index].MaxDepth = 1.0f;
        _renderVP[index].MinDepth = 0.0f;
        _renderVP[index].TopLeftX = 0;
        _renderVP[index].TopLeftY = 0;
    }

    _sceneAABBMin = XMFLOAT3(-100.0f, -100.0f, -100.0f);
    _sceneAABBMax = XMFLOAT3(100.0f, 100.0f, 100.0f);
}

CascadeManager::~CascadeManager()
{
    release();
}

bool CascadeManager::create()
{
    // rs
    D3D11_RASTERIZER_DESC drd =
    {
        D3D11_FILL_SOLID,//D3D11_FILL_MODE FillMode;
        D3D11_CULL_NONE,//D3D11_CULL_MODE CullMode;
        FALSE,//BOOL FrontCounterClockwise;
        0,//INT DepthBias;
        0.0,//FLOAT DepthBiasClamp;
        0.0,//FLOAT SlopeScaledDepthBias;
        TRUE,//BOOL DepthClipEnable;
        FALSE,//BOOL ScissorEnable;
        TRUE,//BOOL MultisampleEnable;
        FALSE//BOOL AntialiasedLineEnable;
    };

    gDevice()->CreateRasterizerState(&drd, &_rsScene);

    // Setting the slope scale depth biase greatly decreases surface acne and incorrect self shadowing.
    drd.SlopeScaledDepthBias = 1.0;
    gDevice()->CreateRasterizerState(&drd, &_rsShadow);

    // Create views.
    V(recreateCascadeBuffers());
    V(createRenderTargetBuffers());

    return S_OK;
}

bool CascadeManager::createRenderTargetBuffers()
{
    D3D11_TEXTURE2D_DESC textureDesc;
    D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
    D3D11_TEXTURE2D_DESC depthBufferDesc;
    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;

    // Get the width and height.
    UINT textureWidth = gD3D()->getScreen().width;
    UINT textureHeight = gD3D()->getScreen().height;

    // Initialize the render target texture description.
    ZeroMemory(&textureDesc, sizeof(textureDesc));

    // Setup the render target texture description.
    textureDesc.Width = textureWidth;
    textureDesc.Height = textureHeight;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = 0;

    // Create the render target textures.
    V(gDevice()->CreateTexture2D(&textureDesc, NULL, &_renderTargetTexture));

    // Setup the description of the render target view.
    renderTargetViewDesc.Format = textureDesc.Format;
    renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    renderTargetViewDesc.Texture2D.MipSlice = 0;

    // Create the render target views.
    V(gDevice()->CreateRenderTargetView(_renderTargetTexture, &renderTargetViewDesc, &_renderTargetView));


    // Setup the description of the shader resource view.
    shaderResourceViewDesc.Format = textureDesc.Format;
    shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
    shaderResourceViewDesc.Texture2D.MipLevels = 1;

    // Create the shader resource views.
    V(gDevice()->CreateShaderResourceView(_renderTargetTexture, &shaderResourceViewDesc, &_shaderResourceView));

    // Initialize the description of the depth buffer.
    ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

    // Set up the description of the depth buffer.
    depthBufferDesc.Width = textureWidth;
    depthBufferDesc.Height = textureHeight;
    depthBufferDesc.MipLevels = 1;
    depthBufferDesc.ArraySize = 1;
    depthBufferDesc.Format = DXGI_FORMAT_R32_TYPELESS;
    depthBufferDesc.SampleDesc.Count = 1;
    depthBufferDesc.SampleDesc.Quality = 0;
    depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
    depthBufferDesc.CPUAccessFlags = 0;
    depthBufferDesc.MiscFlags = 0;

    // Create the texture for the depth buffer using the filled out description.
    V(gDevice()->CreateTexture2D(&depthBufferDesc, NULL, &_depthBuffer));

    // Initailze the depth stencil view description.
    ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

    // Set up the depth stencil view description.
    depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depthStencilViewDesc.Texture2D.MipSlice = 0;

    // Create the depth stencil view.
    V(gDevice()->CreateDepthStencilView(_depthBuffer, &depthStencilViewDesc, &_depthStencilView));

    // Craete the deoth shader resource view.
    shaderResourceViewDesc.Format = DXGI_FORMAT_R32_FLOAT;
    V(gDevice()->CreateShaderResourceView(_depthBuffer, &shaderResourceViewDesc, &_depthShaderResourceView));

    // Setup the viewport for rendering.
    _viewport.Width = (float)textureWidth;
    _viewport.Height = (float)textureHeight;
    _viewport.MinDepth = 0.0f;
    _viewport.MaxDepth = 1.0f;
    _viewport.TopLeftX = 0.0f;
    _viewport.TopLeftY = 0.0f;

    return S_OK;
}

bool CascadeManager::recreateCascadeBuffers()
{
    // vp
    for (INT index = 0; index < cascadeConfig.cascadeLevels; ++index)
    {
        _renderVP[index].Height = (FLOAT)cascadeConfig.bufferSize;
        _renderVP[index].Width = (FLOAT)cascadeConfig.bufferSize;
        _renderVP[index].MaxDepth = 1.0f;
        _renderVP[index].MinDepth = 0.0f;
        _renderVP[index].TopLeftX = (FLOAT)(cascadeConfig.bufferSize * index);
        _renderVP[index].TopLeftY = 0;
    }

    _renderOneTileVP.Height = (FLOAT)cascadeConfig.bufferSize;
    _renderOneTileVP.Width = (FLOAT)cascadeConfig.bufferSize;
    _renderOneTileVP.MaxDepth = 1.0f;
    _renderOneTileVP.MinDepth = 0.0f;
    _renderOneTileVP.TopLeftX = 0.0f;
    _renderOneTileVP.TopLeftY = 0.0f;


    // depth
    releaseCascadeBuffers();

    D3D11_TEXTURE2D_DESC dtd =
    {
        cascadeConfig.bufferSize,//UINT Width;
        cascadeConfig.bufferSize,//UINT Height;
        1,//UINT MipLevels;
        cascadeConfig.cascadeLevels,//UINT ArraySize;
        DXGI_FORMAT_R32G32_FLOAT,//DXGI_FORMAT Format;
        1,//DXGI_SAMPLE_DESC SampleDesc;
        0,
        D3D11_USAGE_DEFAULT,//D3D11_USAGE Usage;
        D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,//UINT BindFlags;
        0,//UINT CPUAccessFlags;
        0//UINT MiscFlags;
    };

    V(gDevice()->CreateTexture2D(&dtd, NULL, &_cascadedShadowMapVarianceTextureArray));

    dtd.ArraySize = 1;
    V(gDevice()->CreateTexture2D(&dtd, NULL, &_cascadedShadowMapTempBlurTexture));

    dtd.Format = DXGI_FORMAT_R32_TYPELESS;
    dtd.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    V(gDevice()->CreateTexture2D(&dtd, NULL, &_temporaryShadowDepthBufferTexture));

    D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
    dsvd.Flags = 0;
    dsvd.Format = DXGI_FORMAT_D32_FLOAT;
    dsvd.Texture2D.MipSlice = 0;
    dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

    V(gDevice()->CreateDepthStencilView(_temporaryShadowDepthBufferTexture,
                                        &dsvd,
                                        &_temporaryShadowDepthBufferDSV));

    D3D11_RENDER_TARGET_VIEW_DESC drtvd =
    {
        DXGI_FORMAT_R32G32_FLOAT,
        D3D11_RTV_DIMENSION_TEXTURE2DARRAY
    };

    drtvd.Texture2DArray.MipSlice = 0;

    for (int index = 0; index < cascadeConfig.cascadeLevels; ++index)
    {
        drtvd.Texture2DArray.FirstArraySlice = index;
        drtvd.Texture2DArray.ArraySize = 1;
        V(gDevice()->CreateRenderTargetView(_cascadedShadowMapVarianceTextureArray,
                                            &drtvd,
                                            &_cascadedShadowMapVarianceRTVArrayAll[index]));
    }

    drtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    drtvd.Texture2D.MipSlice = 0;

    V(gDevice()->CreateRenderTargetView(_cascadedShadowMapTempBlurTexture,
                                        &drtvd,
                                        &_cascadedShadowMapTempBlurRTV));

    D3D11_SHADER_RESOURCE_VIEW_DESC dsrvd =
    {
        DXGI_FORMAT_R32G32_FLOAT,
        D3D11_SRV_DIMENSION_TEXTURE2DARRAY,
    };

    dsrvd.Texture2DArray.ArraySize = cascadeConfig.cascadeLevels;
    dsrvd.Texture2DArray.FirstArraySlice = 0;
    dsrvd.Texture2DArray.MipLevels = 1;
    dsrvd.Texture2DArray.MostDetailedMip = 0;

    V(gDevice()->CreateShaderResourceView(_cascadedShadowMapVarianceTextureArray,
                                          &dsrvd,
                                          &_cascadedShadowMapVarianceSRVArraySingle));

    for (int index = 0; index < cascadeConfig.cascadeLevels; ++index)
    {
        dsrvd.Texture2DArray.ArraySize = 1;
        dsrvd.Texture2DArray.FirstArraySlice = index;
        dsrvd.Texture2DArray.MipLevels = 1;
        dsrvd.Texture2DArray.MostDetailedMip = 0;
        V(gDevice()->CreateShaderResourceView(_cascadedShadowMapVarianceTextureArray,
                                              &dsrvd,
                                              &_cascadedShadowMapVarianceSRVArrayAll[index]));
    }

    dsrvd.Texture2DArray.ArraySize = 1;
    dsrvd.Texture2DArray.FirstArraySlice = 0;
    dsrvd.Texture2DArray.MipLevels = 1;
    dsrvd.Texture2DArray.MostDetailedMip = 0;
    V(gDevice()->CreateShaderResourceView(_cascadedShadowMapTempBlurTexture,
                                          &dsrvd,
                                          &_cascadedShadowMapTempBlurSRV));

    return S_OK;
}

void CascadeManager::release()
{
    SAFE_RELEASE(_rsShadow);
    SAFE_RELEASE(_rsScene);

    releaseCascadeBuffers();
    releaseRenderTargetBuffers();
}

void CascadeManager::releaseCascadeBuffers()
{
    SAFE_RELEASE(_cascadedShadowMapVarianceTextureArray);
    for (int index = 0; index < MAX_CASCADES; ++index)
    {
        SAFE_RELEASE(_cascadedShadowMapVarianceRTVArrayAll[index]);
        SAFE_RELEASE(_cascadedShadowMapVarianceSRVArrayAll[index]);
    }
    SAFE_RELEASE(_cascadedShadowMapVarianceSRVArraySingle);

    SAFE_RELEASE(_temporaryShadowDepthBufferTexture);
    SAFE_RELEASE(_temporaryShadowDepthBufferDSV);

    SAFE_RELEASE(_cascadedShadowMapTempBlurTexture);
    SAFE_RELEASE(_cascadedShadowMapTempBlurRTV);
    SAFE_RELEASE(_cascadedShadowMapTempBlurSRV);
}

void CascadeManager::releaseRenderTargetBuffers()
{
    SAFE_RELEASE(_renderTargetTexture);
    SAFE_RELEASE(_renderTargetView)
    SAFE_RELEASE(_shaderResourceView);

    SAFE_RELEASE(_depthBuffer);
    SAFE_RELEASE(_depthStencilView);
    SAFE_RELEASE(_depthShaderResourceView);
}

namespace XNA
{

_DECLSPEC_ALIGN_16_ struct Frustum
{
    XMFLOAT3 Origin;        // Origin of the frustum (and projection).
    XMFLOAT4 Orientation;   // Unit quaternion representing rotation.

    FLOAT RightSlope;       // Positive X slope (X/Z).
    FLOAT LeftSlope;        // Negative X slope.
    FLOAT TopSlope;         // Positive Y slope (Y/Z).
    FLOAT BottomSlope;      // Negative Y slope.
    FLOAT Near, Far;        // Z of the near plane and far plane.
};

VOID ComputeFrustumFromProjection(Frustum *pOut, XMMATRIX *pProjection)
{
    XMASSERT(pOut);
    XMASSERT(pProjection);

    // Corners of the projection frustum in homogenous space.
    static XMVECTOR HomogenousPoints[6] =
    {
        { 1.0f, 0.0f, 1.0f, 1.0f },   // right (at far plane)
        { -1.0f, 0.0f, 1.0f, 1.0f },   // left
        { 0.0f, 1.0f, 1.0f, 1.0f },   // top
        { 0.0f, -1.0f, 1.0f, 1.0f },   // bottom

        { 0.0f, 0.0f, 0.0f, 1.0f },     // near
        { 0.0f, 0.0f, 1.0f, 1.0f }      // far
    };

    XMVECTOR Determinant;
    XMMATRIX matInverse = XMMatrixInverse(&Determinant, *pProjection);

    // Compute the frustum corners in world space.
    XMVECTOR Points[6];

    for (INT i = 0; i < 6; i++)
    {
        // Transform point.
        Points[i] = XMVector4Transform(HomogenousPoints[i], matInverse);
    }

    pOut->Origin = XMFLOAT3(0.0f, 0.0f, 0.0f);
    pOut->Orientation = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

    // Compute the slopes.
    Points[0] = Points[0] * XMVectorReciprocal(XMVectorSplatZ(Points[0]));
    Points[1] = Points[1] * XMVectorReciprocal(XMVectorSplatZ(Points[1]));
    Points[2] = Points[2] * XMVectorReciprocal(XMVectorSplatZ(Points[2]));
    Points[3] = Points[3] * XMVectorReciprocal(XMVectorSplatZ(Points[3]));

    pOut->RightSlope = XMVectorGetX(Points[0]);
    pOut->LeftSlope = XMVectorGetX(Points[1]);
    pOut->TopSlope = XMVectorGetY(Points[2]);
    pOut->BottomSlope = XMVectorGetY(Points[3]);

    // Compute near and far.
    Points[4] = Points[4] * XMVectorReciprocal(XMVectorSplatW(Points[4]));
    Points[5] = Points[5] * XMVectorReciprocal(XMVectorSplatW(Points[5]));

    pOut->Near = XMVectorGetZ(Points[4]);
    pOut->Far = XMVectorGetZ(Points[5]);

    return;
}

}


//--------------------------------------------------------------------------------------
// This function takes the camera's projection matrix and returns the 8
// points that make up a view frustum.
// The frustum is scaled to fit within the Begin and End interval paramaters.
//--------------------------------------------------------------------------------------
static void s_createFrustumPointsFromCascadeInterval(float fCascadeIntervalBegin,
        FLOAT fCascadeIntervalEnd,
        XMMATRIX &vProjection,
        XMVECTOR *pvCornerPointsWorld)
{

    XNA::Frustum vViewFrust;
    ComputeFrustumFromProjection(&vViewFrust, &vProjection);
    vViewFrust.Near = fCascadeIntervalBegin;
    vViewFrust.Far = fCascadeIntervalEnd;

    static const XMVECTORU32 vGrabY = { 0x00000000, 0xFFFFFFFF, 0x00000000, 0x00000000 };
    static const XMVECTORU32 vGrabX = { 0xFFFFFFFF, 0x00000000, 0x00000000, 0x00000000 };

    XMVECTORF32 vRightTop = { vViewFrust.RightSlope, vViewFrust.TopSlope, 1.0f, 1.0f };
    XMVECTORF32 vLeftBottom = { vViewFrust.LeftSlope, vViewFrust.BottomSlope, 1.0f, 1.0f };
    XMVECTORF32 vNear = { vViewFrust.Near, vViewFrust.Near, vViewFrust.Near, 1.0f };
    XMVECTORF32 vFar = { vViewFrust.Far, vViewFrust.Far, vViewFrust.Far, 1.0f };
    XMVECTOR vRightTopNear = XMVectorMultiply(vRightTop, vNear);
    XMVECTOR vRightTopFar = XMVectorMultiply(vRightTop, vFar);
    XMVECTOR vLeftBottomNear = XMVectorMultiply(vLeftBottom, vNear);
    XMVECTOR vLeftBottomFar = XMVectorMultiply(vLeftBottom, vFar);

    pvCornerPointsWorld[0] = vRightTopNear;
    pvCornerPointsWorld[1] = XMVectorSelect(vRightTopNear, vLeftBottomNear, vGrabX);
    pvCornerPointsWorld[2] = vLeftBottomNear;
    pvCornerPointsWorld[3] = XMVectorSelect(vRightTopNear, vLeftBottomNear, vGrabY);

    pvCornerPointsWorld[4] = vRightTopFar;
    pvCornerPointsWorld[5] = XMVectorSelect(vRightTopFar, vLeftBottomFar, vGrabX);
    pvCornerPointsWorld[6] = vLeftBottomFar;
    pvCornerPointsWorld[7] = XMVectorSelect(vRightTopFar, vLeftBottomFar, vGrabY);

}

//--------------------------------------------------------------------------------------
// Used to compute an intersection of the orthographic projection and the Scene AABB
//--------------------------------------------------------------------------------------
struct Triangle
{
    XMVECTOR pt[3];
    BOOL culled;
};

//--------------------------------------------------------------------------------------
// Computing an accurate near and flar plane will decrease surface acne and Peter-panning.
// Surface acne is the term for erroneous self shadowing.  Peter-panning is the effect where
// shadows disappear near the base of an object.
// As offsets are generally used with PCF filtering due self shadowing issues, computing the
// correct near and far planes becomes even more important.
// This concept is not complicated, but the intersection code is.
//--------------------------------------------------------------------------------------
static void s_computeNearAndFar(FLOAT &fNearPlane,
                                FLOAT &fFarPlane,
                                FXMVECTOR vLightCameraOrthographicMin,
                                FXMVECTOR vLightCameraOrthographicMax,
                                XMVECTOR *pvPointsInCameraView)
{

    // Initialize the near and far planes
    fNearPlane = FLT_MAX;
    fFarPlane = -FLT_MAX;

    Triangle triangleList[16];
    INT iTriangleCnt = 1;

    triangleList[0].pt[0] = pvPointsInCameraView[0];
    triangleList[0].pt[1] = pvPointsInCameraView[1];
    triangleList[0].pt[2] = pvPointsInCameraView[2];
    triangleList[0].culled = false;

    // These are the indices used to tesselate an AABB into a list of triangles.
    static const INT iAABBTriIndexes[] =
    {
        0, 1, 2, 1, 2, 3,
        4, 5, 6, 5, 6, 7,
        0, 2, 4, 2, 4, 6,
        1, 3, 5, 3, 5, 7,
        0, 1, 4, 1, 4, 5,
        2, 3, 6, 3, 6, 7
    };

    INT iPointPassesCollision[3];

    // At a high level:
    // 1. Iterate over all 12 triangles of the AABB.
    // 2. Clip the triangles against each plane. Create new triangles as needed.
    // 3. Find the min and max z values as the near and far plane.

    //This is easier because the triangles are in camera spacing making the collisions tests simple comparisions.

    float fLightCameraOrthographicMinX = XMVectorGetX(vLightCameraOrthographicMin);
    float fLightCameraOrthographicMaxX = XMVectorGetX(vLightCameraOrthographicMax);
    float fLightCameraOrthographicMinY = XMVectorGetY(vLightCameraOrthographicMin);
    float fLightCameraOrthographicMaxY = XMVectorGetY(vLightCameraOrthographicMax);

    for (INT AABBTriIter = 0; AABBTriIter < 12; ++AABBTriIter)
    {

        triangleList[0].pt[0] = pvPointsInCameraView[iAABBTriIndexes[AABBTriIter * 3 + 0]];
        triangleList[0].pt[1] = pvPointsInCameraView[iAABBTriIndexes[AABBTriIter * 3 + 1]];
        triangleList[0].pt[2] = pvPointsInCameraView[iAABBTriIndexes[AABBTriIter * 3 + 2]];
        iTriangleCnt = 1;
        triangleList[0].culled = FALSE;

        // Clip each invidual triangle against the 4 frustums.  When ever a triangle is clipped into new triangles,
        //add them to the list.
        for (INT frustumPlaneIter = 0; frustumPlaneIter < 4; ++frustumPlaneIter)
        {

            FLOAT fEdge;
            INT iComponent;

            if (frustumPlaneIter == 0)
            {
                fEdge = fLightCameraOrthographicMinX; // todo make float temp
                iComponent = 0;
            }
            else if (frustumPlaneIter == 1)
            {
                fEdge = fLightCameraOrthographicMaxX;
                iComponent = 0;
            }
            else if (frustumPlaneIter == 2)
            {
                fEdge = fLightCameraOrthographicMinY;
                iComponent = 1;
            }
            else
            {
                fEdge = fLightCameraOrthographicMaxY;
                iComponent = 1;
            }

            for (INT triIter = 0; triIter < iTriangleCnt; ++triIter)
            {
                // We don't delete triangles, so we skip those that have been culled.
                if (!triangleList[triIter].culled)
                {
                    INT iInsideVertCount = 0;
                    XMVECTOR tempOrder;
                    // Test against the correct frustum plane.
                    // This could be written more compactly, but it would be harder to understand.

                    if (frustumPlaneIter == 0)
                    {
                        for (INT triPtIter = 0; triPtIter < 3; ++triPtIter)
                        {
                            if (XMVectorGetX(triangleList[triIter].pt[triPtIter]) >
                                    XMVectorGetX(vLightCameraOrthographicMin))
                            {
                                iPointPassesCollision[triPtIter] = 1;
                            }
                            else
                            {
                                iPointPassesCollision[triPtIter] = 0;
                            }
                            iInsideVertCount += iPointPassesCollision[triPtIter];
                        }
                    }
                    else if (frustumPlaneIter == 1)
                    {
                        for (INT triPtIter = 0; triPtIter < 3; ++triPtIter)
                        {
                            if (XMVectorGetX(triangleList[triIter].pt[triPtIter]) <
                                    XMVectorGetX(vLightCameraOrthographicMax))
                            {
                                iPointPassesCollision[triPtIter] = 1;
                            }
                            else
                            {
                                iPointPassesCollision[triPtIter] = 0;
                            }
                            iInsideVertCount += iPointPassesCollision[triPtIter];
                        }
                    }
                    else if (frustumPlaneIter == 2)
                    {
                        for (INT triPtIter = 0; triPtIter < 3; ++triPtIter)
                        {
                            if (XMVectorGetY(triangleList[triIter].pt[triPtIter]) >
                                    XMVectorGetY(vLightCameraOrthographicMin))
                            {
                                iPointPassesCollision[triPtIter] = 1;
                            }
                            else
                            {
                                iPointPassesCollision[triPtIter] = 0;
                            }
                            iInsideVertCount += iPointPassesCollision[triPtIter];
                        }
                    }
                    else
                    {
                        for (INT triPtIter = 0; triPtIter < 3; ++triPtIter)
                        {
                            if (XMVectorGetY(triangleList[triIter].pt[triPtIter]) <
                                    XMVectorGetY(vLightCameraOrthographicMax))
                            {
                                iPointPassesCollision[triPtIter] = 1;
                            }
                            else
                            {
                                iPointPassesCollision[triPtIter] = 0;
                            }
                            iInsideVertCount += iPointPassesCollision[triPtIter];
                        }
                    }

                    // Move the points that pass the frustum test to the begining of the array.
                    if (iPointPassesCollision[1] && !iPointPassesCollision[0])
                    {
                        tempOrder = triangleList[triIter].pt[0];
                        triangleList[triIter].pt[0] = triangleList[triIter].pt[1];
                        triangleList[triIter].pt[1] = tempOrder;
                        iPointPassesCollision[0] = TRUE;
                        iPointPassesCollision[1] = FALSE;
                    }
                    if (iPointPassesCollision[2] && !iPointPassesCollision[1])
                    {
                        tempOrder = triangleList[triIter].pt[1];
                        triangleList[triIter].pt[1] = triangleList[triIter].pt[2];
                        triangleList[triIter].pt[2] = tempOrder;
                        iPointPassesCollision[1] = TRUE;
                        iPointPassesCollision[2] = FALSE;
                    }
                    if (iPointPassesCollision[1] && !iPointPassesCollision[0])
                    {
                        tempOrder = triangleList[triIter].pt[0];
                        triangleList[triIter].pt[0] = triangleList[triIter].pt[1];
                        triangleList[triIter].pt[1] = tempOrder;
                        iPointPassesCollision[0] = TRUE;
                        iPointPassesCollision[1] = FALSE;
                    }

                    if (iInsideVertCount == 0)
                    {
                        // All points failed. We're done,
                        triangleList[triIter].culled = true;
                    }
                    else if (iInsideVertCount == 1)
                    {
                        // One point passed. Clip the triangle against the Frustum plane
                        triangleList[triIter].culled = false;

                        //
                        XMVECTOR vVert0ToVert1 = triangleList[triIter].pt[1] - triangleList[triIter].pt[0];
                        XMVECTOR vVert0ToVert2 = triangleList[triIter].pt[2] - triangleList[triIter].pt[0];

                        // Find the collision ratio.
                        FLOAT fHitPointTimeRatio = fEdge - XMVectorGetByIndex(triangleList[triIter].pt[0], iComponent);
                        // Calculate the distance along the vector as ratio of the hit ratio to the component.
                        FLOAT fDistanceAlongVector01 = fHitPointTimeRatio / XMVectorGetByIndex(vVert0ToVert1, iComponent);
                        FLOAT fDistanceAlongVector02 = fHitPointTimeRatio / XMVectorGetByIndex(vVert0ToVert2, iComponent);
                        // Add the point plus a percentage of the vector.
                        vVert0ToVert1 *= fDistanceAlongVector01;
                        vVert0ToVert1 += triangleList[triIter].pt[0];
                        vVert0ToVert2 *= fDistanceAlongVector02;
                        vVert0ToVert2 += triangleList[triIter].pt[0];

                        triangleList[triIter].pt[1] = vVert0ToVert2;
                        triangleList[triIter].pt[2] = vVert0ToVert1;

                    }
                    else if (iInsideVertCount == 2)
                    {
                        // 2 in  // tesselate into 2 triangles


                        // Copy the triangle\(if it exists) after the current triangle out of
                        // the way so we can override it with the new triangle we're inserting.
                        triangleList[iTriangleCnt] = triangleList[triIter + 1];

                        triangleList[triIter].culled = false;
                        triangleList[triIter + 1].culled = false;

                        // Get the vector from the outside point into the 2 inside points.
                        XMVECTOR vVert2ToVert0 = triangleList[triIter].pt[0] - triangleList[triIter].pt[2];
                        XMVECTOR vVert2ToVert1 = triangleList[triIter].pt[1] - triangleList[triIter].pt[2];

                        // Get the hit point ratio.
                        FLOAT fHitPointTime_2_0 = fEdge - XMVectorGetByIndex(triangleList[triIter].pt[2], iComponent);
                        FLOAT fDistanceAlongVector_2_0 = fHitPointTime_2_0 / XMVectorGetByIndex(vVert2ToVert0, iComponent);
                        // Calcaulte the new vert by adding the percentage of the vector plus point 2.
                        vVert2ToVert0 *= fDistanceAlongVector_2_0;
                        vVert2ToVert0 += triangleList[triIter].pt[2];

                        // Add a new triangle.
                        triangleList[triIter + 1].pt[0] = triangleList[triIter].pt[0];
                        triangleList[triIter + 1].pt[1] = triangleList[triIter].pt[1];
                        triangleList[triIter + 1].pt[2] = vVert2ToVert0;

                        //Get the hit point ratio.
                        FLOAT fHitPointTime_2_1 = fEdge - XMVectorGetByIndex(triangleList[triIter].pt[2], iComponent);
                        FLOAT fDistanceAlongVector_2_1 = fHitPointTime_2_1 / XMVectorGetByIndex(vVert2ToVert1, iComponent);
                        vVert2ToVert1 *= fDistanceAlongVector_2_1;
                        vVert2ToVert1 += triangleList[triIter].pt[2];
                        triangleList[triIter].pt[0] = triangleList[triIter + 1].pt[1];
                        triangleList[triIter].pt[1] = triangleList[triIter + 1].pt[2];
                        triangleList[triIter].pt[2] = vVert2ToVert1;
                        // Cncrement triangle count and skip the triangle we just inserted.
                        ++iTriangleCnt;
                        ++triIter;


                    }
                    else
                    {
                        // all in
                        triangleList[triIter].culled = false;

                    }
                }// end if !culled loop
            }
        }
        for (INT index = 0; index < iTriangleCnt; ++index)
        {
            if (!triangleList[index].culled)
            {
                // Set the near and far plan and the min and max z values respectivly.
                for (int vertind = 0; vertind < 3; ++vertind)
                {
                    float fTriangleCoordZ = XMVectorGetZ(triangleList[index].pt[vertind]);
                    if (fNearPlane > fTriangleCoordZ)
                    {
                        fNearPlane = fTriangleCoordZ;
                    }
                    if (fFarPlane  < fTriangleCoordZ)
                    {
                        fFarPlane = fTriangleCoordZ;
                    }
                }
            }
        }
    }

}

static void s_createAABBPoints(XMVECTOR *vAABBPoints, FXMVECTOR vCenter, FXMVECTOR vExtents)
{
    //This map enables us to use a for loop and do vector math.
    static const XMVECTORF32 vExtentsMap[] =
    {
        { 1.0f, 1.0f, -1.0f, 1.0f },
        { -1.0f, 1.0f, -1.0f, 1.0f },
        { 1.0f, -1.0f, -1.0f, 1.0f },
        { -1.0f, -1.0f, -1.0f, 1.0f },
        { 1.0f, 1.0f, 1.0f, 1.0f },
        { -1.0f, 1.0f, 1.0f, 1.0f },
        { 1.0f, -1.0f, 1.0f, 1.0f },
        { -1.0f, -1.0f, 1.0f, 1.0f }
    };

    for (INT index = 0; index < 8; ++index)
    {
        vAABBPoints[index] = XMVectorMultiplyAdd(vExtentsMap[index], vExtents, vCenter);
    }

}

void CascadeManager::computeMatrices()
{
    // Copy D3DX matricies into XNA Math matricies.

    XMMATRIX matViewCameraProjection = gCamera()->getProjM();
    XMMATRIX matViewCameraView = gCamera()->getViewM();
    XMMATRIX matLightCameraView = gLight()->getViewM();

    XMVECTOR det;
    XMMATRIX matInverseViewCamera = XMMatrixInverse(&det, matViewCameraView);

    // Convert from min max representation to center extents represnetation.
    // This will make it easier to pull the points out of the transformation.
    XMVECTOR vSceneAABBMin = XMLoadFloat3(&_sceneAABBMin);
    XMVECTOR vSceneAABBMax = XMLoadFloat3(&_sceneAABBMax);
    XMVECTOR vSceneCenter = vSceneAABBMin + vSceneAABBMax;
    vSceneCenter *= s_halfVector;
    XMVECTOR vSceneExtents = vSceneAABBMax - vSceneAABBMin;
    vSceneExtents *= s_halfVector;

    XMVECTOR vSceneAABBPointsLightSpace[8];
    // This function simply converts the center and extents of an AABB into 8 points
    s_createAABBPoints(vSceneAABBPointsLightSpace, vSceneCenter, vSceneExtents);
    // Transform the scene AABB to Light space.
    for (int index = 0; index < 8; ++index)
    {
        vSceneAABBPointsLightSpace[index] = XMVector4Transform(vSceneAABBPointsLightSpace[index], matLightCameraView);
    }


    FLOAT fFrustumIntervalBegin, fFrustumIntervalEnd;
    XMVECTOR vLightCameraOrthographicMin;  // light space frustrum aabb
    XMVECTOR vLightCameraOrthographicMax;
    FLOAT fCameraNearFarRange = 20.0f;

    XMVECTOR vWorldUnitsPerTexel = { 0.0f, 0.0f, 0.0f, 0.0f };

    // We loop over the cascades to calculate the orthographic projection for each cascade.
    for (INT iCascadeIndex = 0; iCascadeIndex < cascadeConfig.cascadeLevels; ++iCascadeIndex)
    {
        // Calculate the interval of the View Frustum that this cascade covers. We measure the interval
        // the cascade covers as a Min and Max distance along the Z Axis.

        // Because we want to fit the orthogrpahic projection tightly around the Cascade, we set the Mimiumum cascade
        // value to the previous Frustum end Interval
        if (iCascadeIndex == 0)
            fFrustumIntervalBegin = 0.0f;
        else
            fFrustumIntervalBegin = (FLOAT)cascadeConfig.cascadePartitionsZeroToOne[iCascadeIndex - 1];


        // Scale the intervals between 0 and 1. They are now percentages that we can scale with.
        fFrustumIntervalEnd = (FLOAT)cascadeConfig.cascadePartitionsZeroToOne[iCascadeIndex];
        fFrustumIntervalBegin /= (FLOAT)cascadeConfig.cascadePartitionsMax;
        fFrustumIntervalEnd /= (FLOAT)cascadeConfig.cascadePartitionsMax;
        fFrustumIntervalBegin = fFrustumIntervalBegin * fCameraNearFarRange;
        fFrustumIntervalEnd = fFrustumIntervalEnd * fCameraNearFarRange;
        XMVECTOR vFrustumPoints[8];

        // This function takes the began and end intervals along with the projection matrix and returns the 8
        // points that repreresent the cascade Interval
        s_createFrustumPointsFromCascadeInterval(fFrustumIntervalBegin, fFrustumIntervalEnd,
                matViewCameraProjection, vFrustumPoints);

        vLightCameraOrthographicMin = s_FLTMAX;
        vLightCameraOrthographicMax = s_FLTMIN;

        XMVECTOR vTempTranslatedCornerPoint;
        // This next section of code calculates the min and max values for the orthographic projection.
        for (int icpIndex = 0; icpIndex < 8; ++icpIndex)
        {
            // Transform the frustum from camera view space to world space.
            vFrustumPoints[icpIndex] = XMVector4Transform(vFrustumPoints[icpIndex], matInverseViewCamera);
            // Transform the point from world space to Light Camera Space.
            vTempTranslatedCornerPoint = XMVector4Transform(vFrustumPoints[icpIndex], matLightCameraView);
            // Find the closest point.
            vLightCameraOrthographicMin = XMVectorMin(vTempTranslatedCornerPoint, vLightCameraOrthographicMin);
            vLightCameraOrthographicMax = XMVectorMax(vTempTranslatedCornerPoint, vLightCameraOrthographicMax);
        }


        // This code removes the shimmering effect along the edges of shadows due to
        // the light changing to fit the camera.

        // We calculate a looser bound based on the size of the PCF blur.  This ensures us that we're
        // sampling within the correct map.
        float fScaleDuetoBlureAMT = ((float)(cascadeConfig.shadowBlurSize * 2 + 1)
                                     / (float)cascadeConfig.bufferSize);
        XMVECTORF32 vScaleDuetoBlureAMT = { fScaleDuetoBlureAMT, fScaleDuetoBlureAMT, 0.0f, 0.0f };


        float fNormalizeByBufferSize = (1.0f / (float)cascadeConfig.bufferSize);
        XMVECTOR vNormalizeByBufferSize = XMVectorSet(fNormalizeByBufferSize, fNormalizeByBufferSize, 0.0f, 0.0f);

        // We calculate the offsets as a percentage of the bound.
        XMVECTOR vBoarderOffset = vLightCameraOrthographicMax - vLightCameraOrthographicMin;
        vBoarderOffset *= s_halfVector;
        vBoarderOffset *= vScaleDuetoBlureAMT;
        vLightCameraOrthographicMax += vBoarderOffset;
        vLightCameraOrthographicMin -= vBoarderOffset;

        // The world units per texel are used to snap  the orthographic projection
        // to texel sized increments.
        // Because we're fitting tighly to the cascades, the shimmering shadow edges will still be present when the
        // camera rotates.  However, when zooming in or strafing the shadow edge will not shimmer.
        vWorldUnitsPerTexel = vLightCameraOrthographicMax - vLightCameraOrthographicMin;
        vWorldUnitsPerTexel *= vNormalizeByBufferSize;



        if (cascadeConfig.moveLightTexelSize)
        {

            // We snape the camera to 1 pixel increments so that moving the camera does not cause the shadows to jitter.
            // This is a matter of integer dividing by the world space size of a texel
            vLightCameraOrthographicMin /= vWorldUnitsPerTexel;
            vLightCameraOrthographicMin = XMVectorFloor(vLightCameraOrthographicMin);
            vLightCameraOrthographicMin *= vWorldUnitsPerTexel;

            vLightCameraOrthographicMax /= vWorldUnitsPerTexel;
            vLightCameraOrthographicMax = XMVectorFloor(vLightCameraOrthographicMax);
            vLightCameraOrthographicMax *= vWorldUnitsPerTexel;

        }

        //These are the unconfigured near and far plane values.  They are purposly awful to show
        // how important calculating accurate near and far planes is.
        FLOAT fNearPlane = 0.0f;
        FLOAT fFarPlane = 20.0f;

        XMVECTOR vLightSpaceSceneAABBminValue = s_FLTMAX;  // world space scene aabb
        XMVECTOR vLightSpaceSceneAABBmaxValue = s_FLTMIN;
        // We calculate the min and max vectors of the scene in light space. The min and max "Z" values of the
        // light space AABB can be used for the near and far plane. This is easier than intersecting the scene with the AABB
        // and in some cases provides similar results.
        for (int index = 0; index < 8; ++index)
        {
            vLightSpaceSceneAABBminValue = XMVectorMin(vSceneAABBPointsLightSpace[index], vLightSpaceSceneAABBminValue);
            vLightSpaceSceneAABBmaxValue = XMVectorMax(vSceneAABBPointsLightSpace[index], vLightSpaceSceneAABBmaxValue);
        }

        // The min and max z values are the near and far planes.
        fNearPlane = XMVectorGetZ(vLightSpaceSceneAABBminValue);
        fFarPlane = XMVectorGetZ(vLightSpaceSceneAABBmaxValue);

        //// By intersecting the light frustum with the scene AABB we can get a tighter bound on the near and far plane.
        //ComputeNearAndFar(fNearPlane, fFarPlane, vLightCameraOrthographicMin,
        //    vLightCameraOrthographicMax, vSceneAABBPointsLightSpace);


        // Craete the orthographic projection for this cascade.
        XMMATRIX mShadowProj = XMMatrixOrthographicOffCenterLH(
                                   XMVectorGetX(vLightCameraOrthographicMin),
                                   XMVectorGetX(vLightCameraOrthographicMax),
                                   XMVectorGetY(vLightCameraOrthographicMin),
                                   XMVectorGetY(vLightCameraOrthographicMax),
                                   fNearPlane, fFarPlane);
        XMStoreFloat4x4(&_shadowProj[iCascadeIndex], mShadowProj);

        _cascadePartitionsFrustum[iCascadeIndex] = fFrustumIntervalEnd;
    }

    XMStoreFloat4x4(&_shadowView, gLight()->getViewM());
}

bool CascadeManager::sceneSetConstantBuffer()
{
    struct CB
    {
        XMFLOAT4    cascadeOffset[8];
        XMFLOAT4    cascadeScale[8];

        INT         cascadeLevels;
        INT         visualizeCascades;

        FLOAT       minBorderPadding;
        FLOAT       maxBorderPadding;

        FLOAT       cascadeBlendArea;
        FLOAT       texelSize;
        FLOAT       nativeTexelSizeInX;
        FLOAT       paddingForCB3;

        FLOAT       cascadeFrustumsEyeSpaceDepths[8];

        XMFLOAT4    lightDir;
        XMFLOAT3    eyePos;
        FLOAT       padding;
    };
	static BufferType<CB> s_bt([=](CB * pcb)
    {
        // This is a floating point number that is used as the percentage to blur between maps.
        pcb->cascadeBlendArea = cascadeConfig.blurBetweenCascadesAmount;
        pcb->texelSize = 1.0f / (float)cascadeConfig.bufferSize;
        pcb->nativeTexelSizeInX = pcb->texelSize / cascadeConfig.cascadeLevels;

        XMMATRIX mTextureScale = XMMatrixScaling(0.5f, -0.5f, 1.0f);
        XMMATRIX mTextureTranslation = XMMatrixTranslation(0.5f, 0.5f, 0.0f);
        XMMATRIX scaleToTile = XMMatrixScaling(1.0f / (float)cascadeConfig.cascadeLevels, 1.0f, 1.0f);
        for (int index = 0; index < cascadeConfig.cascadeLevels; ++index)
        {
            XMMATRIX mShadowTexture = XMLoadFloat4x4(_shadowProj + index) * mTextureScale * mTextureTranslation;
            pcb->cascadeScale[index].x = mShadowTexture._11;
            pcb->cascadeScale[index].y = mShadowTexture._22;
            pcb->cascadeScale[index].z = mShadowTexture._33;
            pcb->cascadeScale[index].w = 1;

            pcb->cascadeOffset[index].x = mShadowTexture._41;
            pcb->cascadeOffset[index].y = mShadowTexture._42;
            pcb->cascadeOffset[index].z = mShadowTexture._43;
            pcb->cascadeOffset[index].w = 0;
        }

        // Copy intervals for the depth interval selection method.
        memcpy(pcb->cascadeFrustumsEyeSpaceDepths,
               _cascadePartitionsFrustum, MAX_CASCADES * 4);

        // The border padding values keep the pixel shader from reading the borders during PCF filtering.
        pcb->maxBorderPadding = (float)(cascadeConfig.bufferSize - 1.0f) / (float)cascadeConfig.bufferSize;
        pcb->minBorderPadding = (float)(1.0f) / (float)cascadeConfig.bufferSize;

        XMVECTOR ep = gLight()->getEyeV();
        XMVECTOR lp = gLight()->getLookatV();
        XMVECTOR dir = XMVector3Normalize(ep - lp);
        XMStoreFloat4(&pcb->lightDir, dir);
        pcb->eyePos = gCamera()->getEye();
        pcb->cascadeLevels = cascadeConfig.cascadeLevels;
        pcb->visualizeCascades = cascadeConfig.visualizeCascades;
    });
    s_bt.map();
    s_bt.setPS(0, 1);

    return true;
}

void CascadeManager::processCascade(Mesh* mesh)
{
    gContext()->RSSetState(_rsShadow);

    gContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // render cascade shadow map
    for (INT currentCascade = 0; currentCascade < cascadeConfig.cascadeLevels; ++currentCascade)
    {
        gContext()->ClearDepthStencilView(_temporaryShadowDepthBufferDSV, D3D11_CLEAR_DEPTH, 1.0, 0);
        gContext()->OMSetRenderTargets(1, &_cascadedShadowMapVarianceRTVArrayAll[currentCascade], _temporaryShadowDepthBufferDSV);
        gContext()->RSSetViewports(1, &_renderOneTileVP);

        // constant buffer
		{
			struct CB
			{
				XMMATRIX mWorldViewProj;
				XMMATRIX mWorldView;
				XMMATRIX mWorld;
				XMMATRIX mShadow;
			};
			static BufferType<CB> s_bt([=](CB * data)
			{
				XMMATRIX mWorld = XMMatrixIdentity();
				XMMATRIX mView = XMLoadFloat4x4(&_shadowView);
				XMMATRIX mProj = XMLoadFloat4x4(_shadowProj + currentCascade);

				data->mWorldViewProj = XMMatrixTranspose(mWorld * mView * mProj);
				data->mWorldView = XMMatrixTranspose(mWorld * mView);
				data->mWorld = XMMatrixTranspose(mWorld);
				data->mShadow = XMLoadFloat4x4(&_shadowView);
			});
			s_bt.map();

		}
        // draw
		mesh->drawSubset();
    }
}

void CascadeManager::processBlur(Shaders* shaders)
{
    // Process blur.
    ID3D11DepthStencilView *dsNullview = nullptr;
    ID3D11ShaderResourceView *srvNull = nullptr;

    gContext()->IASetInputLayout(nullptr);
    gContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    Sampler::setShadowPoint(5, 1);

    if (cascadeConfig.shadowBlurSize > 1)
    {
        for (int iCurrentCascade = 0; iCurrentCascade < cascadeConfig.cascadeLevels; ++iCurrentCascade)
        {
            // evaluate start end sample
            int start = cascadeConfig.shadowBlurSize / -2;
            int end = cascadeConfig.shadowBlurSize / 2 + 1;

            if (iCurrentCascade != 0)
            {
                float fPCascade = cascadeConfig.cascadePartitionsZeroToOne[0] *
                                  1.0f / cascadeConfig.cascadePartitionsZeroToOne[iCurrentCascade];
                start *= fPCascade;
                end *= fPCascade;
            }

            {
                struct VarianceBlurBufferType
                {
                    int width;
                    int height;
                    int sampleStart;
                    int sampleEnd;
                };
				static BufferType<VarianceBlurBufferType> s_bt([=](VarianceBlurBufferType * data)
				{
					data->width = cascadeConfig.bufferSize;
					data->height = cascadeConfig.bufferSize;
					data->sampleStart = start;
					data->sampleEnd = end;
				});
                s_bt.map();
				s_bt.setPS(2, 1);
            }

            // blur x
			shaders->prepareToBlurX();
            gContext()->PSSetShaderResources(5, 1, &srvNull);
            gContext()->OMSetRenderTargets(1, &_cascadedShadowMapTempBlurRTV, dsNullview);
            gContext()->PSSetShaderResources(5, 1, &_cascadedShadowMapVarianceSRVArrayAll[iCurrentCascade]);
            gContext()->Draw(4, 0);

            // blur y
			shaders->prepareToBlurY();
            gContext()->PSSetShaderResources(5, 1, &srvNull);
            gContext()->OMSetRenderTargets(1, &_cascadedShadowMapVarianceRTVArrayAll[iCurrentCascade], dsNullview);
            gContext()->PSSetShaderResources(5, 1, &_cascadedShadowMapTempBlurSRV);
            gContext()->Draw(4, 0);
        }
    }
}

void CascadeManager::processScene(Mesh* mesh)
{
    float color[4] = { 0, 0, 0, 1 };
    gContext()->ClearRenderTargetView(_renderTargetView, color);

    // prepare
    gContext()->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
    gContext()->OMSetRenderTargets(1, &_renderTargetView, _depthStencilView);
    gContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    gContext()->RSSetViewports(1, &_viewport);
    gContext()->RSSetState(_rsScene);

    // Set samplers and resouce for depth map.
    Sampler::setLinear(0, 1);
    Sampler::setShadowAnisotropic16(5, 1);
    gContext()->PSSetShaderResources(5, 1, &_cascadedShadowMapVarianceSRVArraySingle);

	// constant buffer.
    sceneSetConstantBuffer();
    {
        struct CB
        {
            XMMATRIX mWorldViewProj;
            XMMATRIX mWorldView;
            XMMATRIX mWorld;
            XMMATRIX mShadow;
        };
        static BufferType<CB> s_bt([ = ](CB * data)
        {
			XMMATRIX mWorld = XMMatrixIdentity();
			XMMATRIX mView = gCamera()->getViewM();
			XMMATRIX mProj = gCamera()->getProjM();

			data->mWorldViewProj = XMMatrixTranspose(mWorld * mView * mProj);
			data->mWorldView = XMMatrixTranspose(mWorld * mView);
			data->mWorld = XMMatrixTranspose(mWorld);
            data->mShadow = XMLoadFloat4x4(&_shadowView);
		});
        s_bt.map();
        s_bt.setVS(1, 1);
    }    
	
	// render scenes with shadow    
	mesh->drawSubset();
}
