/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RsmConfig_H___
#define ___C3D_RsmConfig_H___

#include "ReflectiveShadowMapsModule.hpp"

#include "Castor3D/Limits.hpp"

#include "Castor3D/Miscellaneous/MiscellaneousModule.hpp"

#include <CastorUtils/Data/TextWriter.hpp>
#include <CastorUtils/Design/ChangeTracked.hpp>
#include <CastorUtils/Math/RangedValue.hpp>

namespace castor3d
{
	struct RsmConfig
	{
		C3D_API void accept( ConfigurationVisitorBase & visitor );
		C3D_API static void addParsers( castor::AttributeParsers & result );

		castor::ChangeTracked< float > intensity;
		castor::ChangeTracked< float > maxRadius;
		castor::ChangeTracked< castor::RangedValue< uint32_t > > sampleCount{ castor::makeRangedValue( 100u, 20u, MaxRsmRange ) };
	};

	inline bool operator==( RsmConfig const & lhs, RsmConfig const & rhs )noexcept
	{
		return lhs.intensity == rhs.intensity
			&& lhs.maxRadius == rhs.maxRadius
			&& lhs.sampleCount == rhs.sampleCount;
	}
}

#endif
