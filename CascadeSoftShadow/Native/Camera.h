#pragma once

#include "require.h"

NS_WE_BEGAN

class DLL_EXPORT Camera
{
public:
	Camera();

	void setViewParams(
		XMFLOAT3* eye, 
		XMFLOAT3* lookat);

	void setProjParams(
		UINT width, 
		UINT height,
		FLOAT nearPlane, 
		FLOAT farPlane, 
		FLOAT fov = XM_PIDIV4);

	const XMFLOAT3& getEye() const
	{
		return _eye;
	}

	const XMFLOAT3& getLookat() const
	{
		return _lookat;
	}

	XMVECTOR getEyeV() const
	{
		return XMLoadFloat3(&_eye);
	}
		
	XMVECTOR getLookatV() const
	{
		return XMLoadFloat3(&_lookat);
    }

    XMVECTOR getDirectV() const
    {
        return (getLookatV() - getEyeV());
    }

    const XMMATRIX GetViewM() const
    {
        return XMLoadFloat4x4(&_view);
    }

	const XMMATRIX GetProjM() const
	{
		return XMLoadFloat4x4(&_proj);
	}

	const XMMATRIX GetViewProjM() const
	{
		return GetViewM()*GetProjM();
	}

	const XMMATRIX GetViewInvMtx() const
	{
		return XMLoadFloat4x4(&_viewInv);
	}

	float getNearClip() const
	{
		return _nearPlane;
	}

	float getFarClip() const
	{
		return _farPlane;
	}
private:
	XMFLOAT4X4 _view;
	XMFLOAT4X4 _proj;
	XMFLOAT4X4 _viewInv;

	XMFLOAT3 _eye;
	XMFLOAT3 _lookat;

	UINT _width;
	UINT _height;

	FLOAT _nearPlane;
	FLOAT _farPlane;
	FLOAT _fov;
	FLOAT _aspect;
};

class DLL_EXPORT FPSCamera : public Camera
{
public:
	FPSCamera();

	void update(float t);
private:
};

class DLL_EXPORT ModelViewerCamera : public Camera
{
public:
	ModelViewerCamera();

	void began(int nX, int nY);
	void moved(int nX, int nY);
    void ended();
private:
};

NS_WE_END
