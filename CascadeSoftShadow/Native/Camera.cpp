#include "Camera.h"
#include "wemath.h"

USING_WE

// Camera
Camera::Camera()
{
    _eye = XMFLOAT3(0.0f, 0.0f, 10.0f);
    _lookat = XMFLOAT3(0.0f, 0.0f, 0.0f);

    // Setup the view matrix
    setViewParams(&_eye, &_lookat);

    // Setup the projection matrix
    setProjParams(100, 100, 1.0f, 1000.0f);
}

void Camera::setProjParams(UINT width, UINT height, FLOAT nearPlane, FLOAT farPlane, FLOAT fov)
{
    // Set attributes for the projection matrix
    _width = width;
    _height = height;

    _fov = fov;
    _aspect = _width / (FLOAT)_height;
    _nearPlane = nearPlane;
    _farPlane = farPlane;

    XMMATRIX mProjXM = XMMatrixPerspectiveFovLH(fov, _aspect, nearPlane, farPlane);
    XMStoreFloat4x4(&_proj, mProjXM);
}

void Camera::setViewParams(XMFLOAT3 *pvEyePt, XMFLOAT3 *pvLookatPt)
{
    if (NULL == pvEyePt || NULL == pvLookatPt)
        return;

    _eye = *pvEyePt;
    _lookat = *pvLookatPt;

    // Calculate the view matrix.
    XMVECTOR vEye = XMLoadFloat3(pvEyePt);
    XMVECTOR vAt = XMLoadFloat3(pvLookatPt);
    XMVECTOR vUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    XMMATRIX mView = XMMatrixLookAtLH(vEye, vAt, vUp);
    XMMATRIX mViewInv = XMMatrixInverse(&vUp, mView);

    // Store the matrices.
    XMStoreFloat4x4(&_view, mView);
    XMStoreFloat4x4(&_viewInv, mViewInv);
}

// FPSCamera
FPSCamera::FPSCamera()
{

}

// ModelViewerCamera
ModelViewerCamera::ModelViewerCamera()
{
    XMStoreFloat4(&_now, XMQuaternionIdentity());
    XMStoreFloat4(&_down, XMQuaternionIdentity());
    XMStoreFloat4x4(&_rotation, XMMatrixIdentity());

    _drag = FALSE;

    _radius = 1.215f;

    _downPt = XMFLOAT3(0.0f, 0.0f, 1.0f);
    _currentPt = XMFLOAT3(0.0f, 0.0f, 1.0f);
    _eyeRadius = 6.0;
    began(0, 0);
    moved(1, 0);
    ended();
}

XMFLOAT3 ModelViewerCamera::ScreenToVector(float fScreenPtX, float fScreenPtY)
{
    // Scale to screen
    FLOAT x = -(fScreenPtX - _width / 2) / (_radius * _width / 2);
    FLOAT y = -(fScreenPtY - _height / 2) / (_radius * _height / 2);

    FLOAT z = 0.0f;
    FLOAT mag = x * x + y * y;

    if (mag > 1.0f)
    {
        FLOAT scale = 1.0f / sqrtf(mag);
        x *= scale;
        y *= scale;
    }
    else
        z = sqrtf(1.0f - mag);

    // Return vector
    return XMFLOAT3(x, y, z);
}

void ModelViewerCamera::began(int nX, int nY)
{
    // Only enter the drag state if the click falls
	// inside the click rectangle.
    if (nX >= 0 &&
            nX < (int)(_width) &&
            nY >= 0 &&
            nY <  (int)(_height))
    {
        _drag = true;
        _down = _now;
        _downPt = ScreenToVector((float)nX, (float)nY);
    }
}

void ModelViewerCamera::moved(int nX, int nY)
{
    if (_drag)
    {
        _currentPt = ScreenToVector((float)nX, (float)nY);

        XMVECTOR qDown = XMLoadFloat4(&_down);
        XMVECTOR vDownPt = XMLoadFloat3(&_downPt);
        XMVECTOR vCurrentPt = XMLoadFloat3(&_currentPt);
        qDown = XMQuaternionMultiply(qDown, math::quatFromBallPoints(vDownPt, vCurrentPt));
        XMStoreFloat4(&_now, qDown);
        // Simply update
        update();
    }
}

void ModelViewerCamera::ended()
{
    _drag = false;
}

void ModelViewerCamera::update()
{
    XMVECTOR vDeterminant;
    XMMATRIX mCameraRot = getRotationMatrix();

    mCameraRot = XMMatrixInverse(&vDeterminant, mCameraRot);

    XMVECTOR Eye = XMVectorSet(0.0f, 0.0f, _eyeRadius, 0.0f);
    XMVECTOR At = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
    XMVECTOR Up = XMVectorSet(0.0f, 1000.0f, 0.0f, 0.0f);
    Eye = XMVector3Transform(Eye, mCameraRot);
    Up = XMVector3Transform(Up, mCameraRot);

    XMMATRIX mView = XMMatrixLookAtLH(Eye, At, Up);
    XMStoreFloat4x4(&_view, mView);

    // Store EyePos
    XMStoreFloat3(&_eye, Eye);

    // mtxViewInv
    XMMATRIX mViewInv = XMMatrixInverse(&vDeterminant, mView);
    XMStoreFloat4x4(&_viewInv, mViewInv);
}

void ModelViewerCamera::closer()
{
    _eyeRadius *= 1.1f;
    update();
}

void ModelViewerCamera::further()
{
    _eyeRadius *= 0.90909f;
    update();
}