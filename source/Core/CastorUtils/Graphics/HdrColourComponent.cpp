#include "CastorUtils/Graphics/HdrColourComponent.hpp"

#include "CastorUtils/Graphics/ColourComponent.hpp"

#include <cmath>

namespace c3d
{
	namespace hdrcol
	{
		static float removeGamma( float srgb, float gamma )noexcept
		{
			return powf( std::max( srgb, 0.0f ), gamma );
		}
	}

	HdrColourComponentValue::HdrColourComponentValue( ColourComponentValue const & rhs
		, float gamma )noexcept
		: m_component{ hdrcol::removeGamma( rhs.value(), gamma ) }
	{
	}
}
