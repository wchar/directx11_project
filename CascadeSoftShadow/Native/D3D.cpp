#include "D3D.h"
#include "clover.h"
#include "Camera.h"

USING_WE

D3D::D3D()
{
    _device = nullptr;
    _context = nullptr;
    _renderTargetView = nullptr;
    _depthStencil = nullptr;
    _depthStencilView = nullptr;
    _swapChain = nullptr;
    _hWnd = nullptr;
}

D3D::~D3D()
{
    SAFE_RELEASE(_device);
    SAFE_RELEASE(_context);
    SAFE_RELEASE(_renderTargetView);
    SAFE_RELEASE(_depthStencil);
    SAFE_RELEASE(_depthStencilView);
    SAFE_RELEASE(_swapChain);
}

HRESULT D3D::create(HWND hwnd)
{
    HRESULT hr = S_OK;

    if (hwnd == NULL)
        return S_FALSE;

    _hWnd = hwnd;

    RECT rc;
    GetClientRect(hwnd, &rc);
    UINT width = rc.right - rc.left;
    UINT height = rc.bottom - rc.top;
    UINT createDeviceFlags = 0;

    D3D_DRIVER_TYPE driverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };
    UINT numDriverTypes = ARRAYSIZE(driverTypes);

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_0
    };
    UINT numFeatureLevels = ARRAYSIZE(featureLevels);

    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = width;
    sd.BufferDesc.Height = height;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hwnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;

    for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
    {
        _driverType = driverTypes[driverTypeIndex];
        hr = D3D11CreateDeviceAndSwapChain(NULL, _driverType, NULL, createDeviceFlags, featureLevels, numFeatureLevels,
                                           D3D11_SDK_VERSION, &sd, &_swapChain, &_device, &_featureLevel, &_context);
        if (SUCCEEDED(hr))
            break;
    }
    if (FAILED(hr))
        return hr;

    V(_createViews(width, height));

    return S_OK;
}

size D3D::getScreen()
{
    RECT rc;
    GetClientRect(_hWnd, &rc);
    return size(rc.right - rc.left, rc.bottom - rc.top);
}

HRESULT D3D::setWindow(HWND hWnd)
{
    _hWnd = hWnd;

    IDXGIDevice *pDXGIDevice;
    _device->QueryInterface(__uuidof(IDXGIDevice), (void **)&pDXGIDevice);

    IDXGIAdapter *pDXGIAdapter;
    pDXGIDevice->GetParent(__uuidof(IDXGIAdapter), (void **)&pDXGIAdapter);

    IDXGIFactory *pIDXGIFactory;
    pDXGIAdapter->GetParent(__uuidof(IDXGIFactory), (void **)&pIDXGIFactory);

    RECT rc;
    GetClientRect(hWnd, &rc);
    UINT width = rc.right - rc.left;
    UINT height = rc.bottom - rc.top;

    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 1;
    sd.BufferDesc.Width = width;
    sd.BufferDesc.Height = height;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;

    SAFE_RELEASE(_swapChain);
    pIDXGIFactory->CreateSwapChain(_device, &sd, &_swapChain);

    V(onResizeWindow());

    return S_OK;
}

HRESULT D3D::onResizeWindow()
{
    // _context->OMSetRenderTargets(1, NULL, NULL);

    SAFE_RELEASE(_renderTargetView);
    SAFE_RELEASE(_depthStencil);
    SAFE_RELEASE(_depthStencilView);

    RECT rc;
    GetClientRect(_hWnd, &rc);
    UINT width = rc.right - rc.left;
    UINT height = rc.bottom - rc.top;

    ID3D11Texture2D *pBackBuffer = NULL;
    V(_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID *)&pBackBuffer));
    pBackBuffer->Release();

    V(_swapChain->ResizeBuffers(2, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0));
    V(_createViews(width, height));

    _context->OMSetRenderTargets(1, &_renderTargetView, _depthStencilView);
    _context->RSSetViewports(1, &_viewport);

     //Update camera params.
    auto camera = gCamera();
	camera->setProjParams(width, height, camera->getNearClip(), camera->getFarClip());

	auto light = gLight();
	light->setProjParams(width, height, light->getNearClip(), light->getFarClip());

    return S_OK;
}

HRESULT D3D::_createViews(UINT width, UINT height)
{
    // Create a render target view
    ID3D11Texture2D *pBackBuffer = NULL;
    V(_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID *)&pBackBuffer));

    HRESULT hr = _device->CreateRenderTargetView(pBackBuffer, NULL, &_renderTargetView);
    pBackBuffer->Release();
    if (FAILED(hr))
        return hr;

    // Create depth stencil texture
    D3D11_TEXTURE2D_DESC descDepth;
    ZeroMemory(&descDepth, sizeof(descDepth));
    descDepth.Width = width;
    descDepth.Height = height;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDepth.SampleDesc.Count = 1;
    descDepth.SampleDesc.Quality = 0;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    descDepth.CPUAccessFlags = 0;
    descDepth.MiscFlags = 0;
    V(_device->CreateTexture2D(&descDepth, NULL, &_depthStencil));

    // Create the depth stencil view
    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
    ZeroMemory(&descDSV, sizeof(descDSV));
    descDSV.Format = descDepth.Format;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;
    V(_device->CreateDepthStencilView(_depthStencil, &descDSV, &_depthStencilView));

    // Setup the view port
    _viewport.Width = (FLOAT)width;
    _viewport.Height = (FLOAT)height;
    _viewport.MinDepth = 0.0f;
    _viewport.MaxDepth = 1.0f;
    _viewport.TopLeftX = 0;
    _viewport.TopLeftY = 0;

    return S_OK;
}

void D3D::clear()
{
    float ClearColor[4] = { 0.4f, 0.7f, 1.0f, 0.0f };
    _context->ClearRenderTargetView(_renderTargetView, ClearColor);
    _context->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH, 1.0, 0);
}

void D3D::present()
{
    _swapChain->Present(0, 0);
}

void D3D::resetViews()
{
    _context->OMSetRenderTargets(1, &_renderTargetView, _depthStencilView);
    _context->RSSetViewports(1, &_viewport);
}

