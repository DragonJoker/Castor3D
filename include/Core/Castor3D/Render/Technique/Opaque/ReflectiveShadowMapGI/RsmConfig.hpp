/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RsmConfig_H___
#define ___C3D_RsmConfig_H___

#include "ReflectiveShadowMapGIModule.hpp"

#include "Castor3D/Miscellaneous/MiscellaneousModule.hpp"

#include <CastorUtils/Data/TextWriter.hpp>
#include <CastorUtils/Design/ChangeTracked.hpp>
#include <CastorUtils/Math/RangedValue.hpp>

namespace castor3d
{
	struct RsmConfig
	{
		C3D_API void accept( castor::String const & name
			, PipelineVisitorBase & visitor );

		static constexpr uint32_t MaxRange = 800u;

		castor::ChangeTracked< float > intensity;
		castor::ChangeTracked< float > maxRadius;
		castor::ChangeTracked< castor::RangedValue< uint32_t > > sampleCount{ castor::makeRangedValue( 100u, 20u, MaxRange ) };
	};
}

#endif
