#include "CastorUtils/Graphics/ColourComponent.hpp"

#include "CastorUtils/Graphics/HdrColourComponent.hpp"

#include <cmath>

namespace c3d
{
	namespace srgbcol
	{
		static float applyGamma( float hdr, float gamma )
		{
			return powf( std::max( hdr, 0.0f ), 1.0f / gamma );
		}
	}

	ColourComponentValue::ColourComponentValue( HdrColourComponentValue const & rhs
		, float gamma )
		: m_component{ srgbcol::applyGamma( rhs.value(), gamma ) }
	{
	}
}
