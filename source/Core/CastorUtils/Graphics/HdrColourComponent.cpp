#include "CastorUtils/Graphics/HdrColourComponent.hpp"

#include "CastorUtils/Graphics/ColourComponent.hpp"

#include <cmath>

namespace c3d
{
	namespace hdrcol
	{
		static float removeGamma( float srgb, float gamma )
		{
			return powf( std::max( srgb, 0.0f ), gamma );
		}
	}

	HdrColourComponentValue::HdrColourComponentValue( ColourComponentValue const & rhs
		, float gamma )
		: m_component{ hdrcol::removeGamma( rhs.value(), gamma ) }
	{
	}

	bool operator==( HdrColourComponentValue const & lhs, HdrColourComponentValue const & rhs )
	{
		float uiA;
		float uiB;
		lhs.convertTo( uiA );
		rhs.convertTo( uiB );
		return uiA == uiB;
	}

	bool operator!=( HdrColourComponentValue const & lhs, HdrColourComponentValue const & rhs )
	{
		return !( lhs == rhs );
	}

	float operator-( HdrColourComponentValue const & lhs, HdrColourComponentValue const & rhs )
	{
		float fValue;
		lhs.convertTo( fValue );
		HdrColourComponentValue cpnt( fValue );
		cpnt -= rhs;
		return cpnt;
	}

	float operator+( HdrColourComponentValue const & lhs, HdrColourComponentValue const & rhs )
	{
		float fValue;
		lhs.convertTo( fValue );
		HdrColourComponentValue cpnt( fValue );
		cpnt += rhs;
		return cpnt;
	}

	float operator*( HdrColourComponentValue const & lhs, HdrColourComponentValue const & rhs )
	{
		float fValue;
		lhs.convertTo( fValue );
		HdrColourComponentValue cpnt( fValue );
		cpnt *= rhs;
		return cpnt;
	}

	float operator/( HdrColourComponentValue const & lhs, HdrColourComponentValue const & rhs )
	{
		float fValue;
		lhs.convertTo( fValue );
		HdrColourComponentValue cpnt( fValue );
		cpnt /= rhs;
		return cpnt;
	}
}
