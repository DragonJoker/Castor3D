#include "CastorUtils/Graphics/ColourComponent.hpp"

#include "CastorUtils/Graphics/HdrColourComponent.hpp"

#include <cmath>

namespace castor
{
	namespace srgbcol
	{
		static float applyGamma( float hdr, float gamma )
		{
			return powf( std::max( hdr, 0.0f ), 1.0f / gamma );
		}
	}

	ColourComponent::ColourComponent( HdrColourComponent const & rhs
		, float gamma )
		: m_component{ srgbcol::applyGamma( rhs.value(), gamma ) }
	{
	}

	bool operator ==( ColourComponent const & lhs, ColourComponent const & rhs )
	{
		uint8_t uiA;
		uint8_t uiB;
		lhs.convertTo( uiA );
		rhs.convertTo( uiB );
		return uiA == uiB;
	}

	bool operator !=( ColourComponent const & lhs, ColourComponent const & rhs )
	{
		return !operator ==( lhs, rhs );
	}

	float operator -( ColourComponent const & lhs, ColourComponent const & rhs )
	{
		float fValue;
		lhs.convertTo( fValue );
		ColourComponent cpnt( fValue );
		cpnt -= rhs;
		return fValue;
	}

	float operator +( ColourComponent const & lhs, ColourComponent const & rhs )
	{
		float fValue;
		lhs.convertTo( fValue );
		ColourComponent cpnt( fValue );
		cpnt += rhs;
		return fValue;
	}

	float operator *( ColourComponent const & lhs, ColourComponent const & rhs )
	{
		float fValue;
		lhs.convertTo( fValue );
		ColourComponent cpnt( fValue );
		cpnt *= rhs;
		return fValue;
	}

	float operator /( ColourComponent const & lhs, ColourComponent const & rhs )
	{
		float fValue;
		lhs.convertTo( fValue );
		ColourComponent cpnt( fValue );
		cpnt /= rhs;
		return fValue;
	}
}
