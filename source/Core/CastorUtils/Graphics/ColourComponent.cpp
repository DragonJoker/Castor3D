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

	bool operator ==( ColourComponent const & p_cpnA, ColourComponent const & p_cpnB )
	{
		uint8_t uiA;
		uint8_t uiB;
		p_cpnA.convertTo( uiA );
		p_cpnB.convertTo( uiB );
		return uiA == uiB;
	}

	bool operator !=( ColourComponent const & p_cpnA, ColourComponent const & p_cpnB )
	{
		return !operator ==( p_cpnA, p_cpnB );
	}

	float operator -( ColourComponent const & p_cpnt, ColourComponent const & p_scalar )
	{
		float fValue;
		p_cpnt.convertTo( fValue );
		ColourComponent cpnt( fValue );
		cpnt -= p_scalar;
		return fValue;
	}

	float operator +( ColourComponent const & p_cpnt, ColourComponent const & p_scalar )
	{
		float fValue;
		p_cpnt.convertTo( fValue );
		ColourComponent cpnt( fValue );
		cpnt += p_scalar;
		return fValue;
	}

	float operator *( ColourComponent const & p_cpnt, ColourComponent const & p_scalar )
	{
		float fValue;
		p_cpnt.convertTo( fValue );
		ColourComponent cpnt( fValue );
		cpnt *= p_scalar;
		return fValue;
	}

	float operator /( ColourComponent const & p_cpnt, ColourComponent const & p_scalar )
	{
		float fValue;
		p_cpnt.convertTo( fValue );
		ColourComponent cpnt( fValue );
		cpnt /= p_scalar;
		return fValue;
	}
}
