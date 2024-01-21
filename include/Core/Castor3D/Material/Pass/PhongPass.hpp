/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PhongPass_H___
#define ___C3D_PhongPass_H___

#include "Castor3D/Material/Pass/Pass.hpp"

namespace castor3d
{
	class PhongPass
	{
	public:
		C3D_API static PassUPtr create( LightingModelID lightingModelId
			, Material & parent );

		C3D_API static castor::StringView const Type;
		C3D_API static castor::StringView const LightingModel;
	};
}

#endif
