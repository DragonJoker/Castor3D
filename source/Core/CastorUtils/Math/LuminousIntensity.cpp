#include "CastorUtils/Math/LuminousIntensity.hpp"

#include "CastorUtils/Math/Illumination.hpp"

namespace castor
{
	LuminousIntensity::LuminousIntensity( Illumination const & illumination, float metres )noexcept
		: LuminousIntensity{ illumination.lux() * metres * metres }
	{
	}
}
