#pragma once

#include "require.h"

NS_WE_BEGAN

class DLL_EXPORT D3D
{
public:
    DLL_EXPORT friend D3D *gD3D();

    ID3D11Device *getDevice()
    {
        return _device;
    }

    ID3D11DeviceContext *getContext()
    {
        return _context;
    }

    // config
    HRESULT create(HWND hWnd);
    HRESULT setWindow(HWND hWnd);
    HRESULT onResizeWindow();
    void resetViews();
    size getScreen();

    // draw
    void clear();
    void present();
private:
    D3D();
    ~D3D();

    HWND                        _hWnd;

    ID3D11Device               *_device;
    ID3D11DeviceContext        *_context;

    IDXGISwapChain             *_swapChain;
    ID3D11RenderTargetView     *_renderTargetView;
    ID3D11Texture2D            *_depthStencil;
    ID3D11DepthStencilView     *_depthStencilView;

    HRESULT _createViews(UINT width, UINT height);

    D3D_DRIVER_TYPE             _driverType;
    D3D_FEATURE_LEVEL           _featureLevel;
    D3D11_VIEWPORT              _viewport;

};

NS_WE_END