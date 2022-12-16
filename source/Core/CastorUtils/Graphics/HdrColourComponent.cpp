#include "CastorUtils/Graphics/HdrColourComponent.hpp"

#include "CastorUtils/Graphics/ColourComponent.hpp"

#include <cmath>

namespace castor
{
	namespace hdrcol
	{
		static float removeGamma( float srgb, float gamma )
		{
			return powf( std::max( srgb, 0.0f ), gamma );
		}
	}

	HdrColourComponent::HdrColourComponent( ColourComponent const & rhs
		, float gamma )
		: m_component{ hdrcol::removeGamma( rhs.value(), gamma ) }
	{
	}
	bool operator==( HdrColourComponent const & lhs, HdrColourComponent const & rhs )
	{
		float uiA;
		float uiB;
		lhs.convertTo( uiA );
		rhs.convertTo( uiB );
		return uiA == uiB;
	}

	bool operator!=( HdrColourComponent const & lhs, HdrColourComponent const & rhs )
	{
		return !( lhs == rhs );
	}

	float operator-( HdrColourComponent const & lhs, HdrColourComponent const & rhs )
	{
		float fValue;
		lhs.convertTo( fValue );
		HdrColourComponent cpnt( fValue );
		cpnt -= rhs;
		return cpnt;
	}

	float operator+( HdrColourComponent const & lhs, HdrColourComponent const & rhs )
	{
		float fValue;
		lhs.convertTo( fValue );
		HdrColourComponent cpnt( fValue );
		cpnt += rhs;
		return cpnt;
	}

	float operator*( HdrColourComponent const & lhs, HdrColourComponent const & rhs )
	{
		float fValue;
		lhs.convertTo( fValue );
		HdrColourComponent cpnt( fValue );
		cpnt *= rhs;
		return cpnt;
	}

	float operator/( HdrColourComponent const & lhs, HdrColourComponent const & rhs )
	{
		float fValue;
		lhs.convertTo( fValue );
		HdrColourComponent cpnt( fValue );
		cpnt /= rhs;
		return cpnt;
	}
}
