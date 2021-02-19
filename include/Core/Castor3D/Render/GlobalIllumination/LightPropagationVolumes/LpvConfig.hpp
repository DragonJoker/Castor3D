/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LpvConfig_H___
#define ___C3D_LpvConfig_H___

#include "LightPropagationVolumesModule.hpp"

#include "Castor3D/Miscellaneous/MiscellaneousModule.hpp"

#include <CastorUtils/Design/ChangeTracked.hpp>

namespace castor3d
{
	struct LpvConfig
	{
		C3D_API void accept( castor::String const & name
			, PipelineVisitorBase & visitor );

		castor::ChangeTracked< float > indirectAttenuation;
		castor::ChangeTracked< float > texelAreaModifier;
	};
}

#endif
