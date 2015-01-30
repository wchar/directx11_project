#pragma once

#include "require.h"

#define NS_MATH_BEGAN	namespace math{
#define NS_MATH_END		}

NS_WE_BEGAN NS_MATH_BEGAN

XMVECTOR quatFromBallPoints(const XMVECTOR from, const XMVECTOR to);

NS_MATH_END NS_WE_END