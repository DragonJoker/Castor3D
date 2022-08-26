/*
See LICENSE file in root folder
*/
#ifndef ___C3D_DeferredDirectionalLightPass_H___
#define ___C3D_DeferredDirectionalLightPass_H___

#include "LightPass.hpp"

namespace castor3d
{
	class DirectionalLightPass
	{
	public:
		static ShaderPtr getVertexShaderSource();
	};
}

#endif
