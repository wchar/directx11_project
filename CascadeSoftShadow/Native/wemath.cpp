#include "wemath.h"

XMVECTOR WE::math::quatFromBallPoints(const XMVECTOR from, const XMVECTOR to)
{
	if (XMVector3Equal(from, to))
		return  XMQuaternionIdentity();
	float w = XMVectorGetX(XMVector3AngleBetweenNormals(from, to));
	XMVECTOR vPart = XMVector3Cross(from, to);
	return XMQuaternionRotationAxis(vPart, w * 2.0f);
}
