/*
See LICENSE file in root folder
*/
#ifndef ___C3D_BlinnPhongPass_H___
#define ___C3D_BlinnPhongPass_H___

#include "Castor3D/Material/Pass/Phong/PhongPass.hpp"

namespace castor3d
{
	class BlinnPhongPass
	{
	public:
		C3D_API static PassSPtr create( Material & parent );

		C3D_API static castor::String const Type;
		C3D_API static castor::String const LightingModel;
	};
}

#endif
