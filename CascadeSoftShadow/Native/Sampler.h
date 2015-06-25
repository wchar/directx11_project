#pragma once

#include "require.h"
#include "clover.h"

NS_WE_BEGAN

namespace Sampler
{
#define FUNC_DEF(funcName) void funcName(int start, int num);
	FUNC_DEF(setPoint);
	FUNC_DEF(setLinear);
	FUNC_DEF(setShadowPoint);
	FUNC_DEF(setShadowAnisotropic16);
}

NS_WE_END
