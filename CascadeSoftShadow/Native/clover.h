#pragma once

#include "require.h"

NS_WE_BEGAN

class D3D;

DLL_EXPORT D3D* gD3D();

// Get d3ddevice.
DLL_EXPORT ID3D11Device* gDevice();

// Get immediate context.
DLL_EXPORT ID3D11DeviceContext* gContext();

DLL_EXPORT HRESULT compileShaderFromFile(WCHAR* szFileName,
	D3D_SHADER_MACRO* pDefines,
	LPCSTR szEntryPoint,
	LPCSTR szShaderModel,
	ID3DBlob** ppBlobOut);

DLL_EXPORT HRESULT compileShaderFromMemory(LPCSTR pSrcData,
	SIZE_T SrcDataLen,
	D3D_SHADER_MACRO* pDefines,
	LPCSTR szEntryPoint,
	LPCSTR szShaderModel,
	ID3DBlob** ppBlobOut);

DLL_EXPORT HRESULT createBuffer(UINT uElementSize, 
	UINT uCount, 
	VOID* pInitData, 
	ID3D11Buffer** ppBufOut);

DLL_EXPORT HRESULT createBufferSRV(
	ID3D11Buffer* pBuffer, 
	ID3D11ShaderResourceView** ppSRVOut);

DLL_EXPORT HRESULT createBufferUAV(
	ID3D11Buffer* pBuffer, 
	ID3D11UnorderedAccessView** ppUAVOut);

DLL_EXPORT HRESULT createSRVFromFile(
	const WCHAR* strFile, 
	ID3D11ShaderResourceView** ppSRVout);

NS_WE_END


NS_WE_BEGAN 
// camera
class Camera;
DLL_EXPORT Camera* gCamera();
DLL_EXPORT Camera* gLight();

NS_WE_END

