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

	bool operator ==( ColourComponentValue const & lhs, ColourComponentValue const & rhs )
	{
		uint8_t uiA;
		uint8_t uiB;
		lhs.convertTo( uiA );
		rhs.convertTo( uiB );
		return uiA == uiB;
	}

	bool operator !=( ColourComponentValue const & lhs, ColourComponentValue const & rhs )
	{
		return !operator ==( lhs, rhs );
	}

	float operator -( ColourComponentValue const & lhs, ColourComponentValue const & rhs )
	{
		float fValue;
		lhs.convertTo( fValue );
		ColourComponentValue cpnt( fValue );
		cpnt -= rhs;
		return cpnt;
	}

	float operator +( ColourComponentValue const & lhs, ColourComponentValue const & rhs )
	{
		float fValue;
		lhs.convertTo( fValue );
		ColourComponentValue cpnt( fValue );
		cpnt += rhs;
		return cpnt;
	}

	float operator *( ColourComponentValue const & lhs, ColourComponentValue const & rhs )
	{
		float fValue;
		lhs.convertTo( fValue );
		ColourComponentValue cpnt( fValue );
		cpnt *= rhs;
		return cpnt;
	}

	float operator /( ColourComponentValue const & lhs, ColourComponentValue const & rhs )
	{
		float fValue;
		lhs.convertTo( fValue );
		ColourComponentValue cpnt( fValue );
		cpnt /= rhs;
		return cpnt;
	}
}
