/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PbrPass_H___
#define ___C3D_PbrPass_H___

#include "Castor3D/Material/Pass/Pass.hpp"

namespace castor3d
{
	class PbrPass
	{
	public:
		C3D_API static PassUPtr create( LightingModelID lightingModelId
			, Material & parent );

		C3D_API static castor::String const Type;
		C3D_API static castor::String const LightingModel;
	};
}

#endif
