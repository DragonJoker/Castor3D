#include "CastorUtils/Math/Illumination.hpp"

#include "CastorUtils/Math/LuminousIntensity.hpp"

namespace castor
{
	Illumination::Illumination( LuminousIntensity const & illumination, float metres )noexcept
		: Illumination{ illumination.candela() / ( metres * metres ) }
	{
	}
}
