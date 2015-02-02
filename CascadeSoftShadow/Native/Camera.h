#pragma once

#include "require.h"

NS_WE_BEGAN

class DLL_EXPORT Camera
{
public:
    Camera();

    void setViewParams(
        XMFLOAT3 *eye,
        XMFLOAT3 *lookat);

    void setProjParams(
        UINT width,
        UINT height,
        FLOAT nearPlane,
        FLOAT farPlane,
        FLOAT fov = XM_PIDIV4);

    const XMFLOAT3 &getEye() const;
    const XMFLOAT3 &getLookat() const;

    XMVECTOR getEyeV() const;
    XMVECTOR getLookatV() const;
    XMVECTOR getDirectV() const;

    const XMMATRIX getViewM() const;
    const XMMATRIX getProjM() const;
    const XMMATRIX getViewProjM() const;
    const XMMATRIX getViewInvM() const;

    float getNearClip() const;
    float getFarClip() const;

protected:
    XMFLOAT4X4 _view;
    XMFLOAT4X4 _proj;
    XMFLOAT4X4 _viewInv;

    XMFLOAT3 _eye;
    XMFLOAT3 _lookat;

    FLOAT _nearPlane;
    FLOAT _farPlane;
    FLOAT _fov;
    FLOAT _aspect;

    UINT _width;
    UINT _height;
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

    void closer();
    void further();

    // static XMVECTOR QuatFromBallPoints(const XMVECTOR vFrom, const XMVECTOR vTo);
protected:
    void update();

    const XMMATRIX getRotationMatrix()
    {
        return XMMatrixRotationQuaternion(XMLoadFloat4(&_now));
    }
    bool isBeingDragged() const
    {
        return _drag;
    }
    const XMFLOAT4 getQuatNow() const
    {
        return _now;
    }
    void setQuatNow(const XMFLOAT4 *q)
    {
        _now = *q;
    }

    XMFLOAT4X4 _rotation;

    XMFLOAT4 _down;
    XMFLOAT4 _now;

    XMFLOAT3 _downPt;
    XMFLOAT3 _currentPt;
    XMFLOAT3 ScreenToVector(float screenPtX, float screenPtY);

    FLOAT _radius;
    FLOAT _eyeRadius;

    bool _drag;
};


// inlines
inline const XMFLOAT3 &Camera::getEye() const
{
    return _eye;
}

inline const XMFLOAT3 &Camera::getLookat() const
{
    return _lookat;
}

inline XMVECTOR Camera::getEyeV() const
{
    return XMLoadFloat3(&_eye);
}

inline XMVECTOR Camera::getLookatV() const
{
    return XMLoadFloat3(&_lookat);
}

inline XMVECTOR Camera::getDirectV() const
{
    return (getLookatV() - getEyeV());
}

inline const XMMATRIX Camera::getViewM() const
{
    return XMLoadFloat4x4(&_view);
}

inline const XMMATRIX Camera::getProjM() const
{
    return XMLoadFloat4x4(&_proj);
}

inline const XMMATRIX Camera::getViewProjM() const
{
    return getViewM() * getProjM();
}

inline const XMMATRIX Camera::getViewInvM() const
{
    return XMLoadFloat4x4(&_viewInv);
}

inline float Camera::getNearClip() const
{
    return _nearPlane;
}

inline float Camera::getFarClip() const
{
    return _farPlane;
}

NS_WE_END
