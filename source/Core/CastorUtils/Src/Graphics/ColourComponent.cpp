#include "ColourComponent.hpp"

namespace Castor
{
	bool operator ==( ColourComponent const & p_cpnA, ColourComponent const & p_cpnB )
	{
		uint8_t uiA;
		uint8_t uiB;
		p_cpnA.convert_to( uiA );
		p_cpnB.convert_to( uiB );
		return uiA == uiB;
	}

	bool operator !=( ColourComponent const & p_cpnA, ColourComponent const & p_cpnB )
	{
		return !operator ==( p_cpnA, p_cpnB );
	}

	float operator -( ColourComponent const & p_cpnt, ColourComponent const & p_scalar )
	{
		float fValue;
		p_cpnt.convert_to( fValue );
		ColourComponent cpnt( &fValue );
		cpnt -= p_scalar;
		return fValue;
	}

	float operator +( ColourComponent const & p_cpnt, ColourComponent const & p_scalar )
	{
		float fValue;
		p_cpnt.convert_to( fValue );
		ColourComponent cpnt( &fValue );
		cpnt += p_scalar;
		return fValue;
	}

	float operator *( ColourComponent const & p_cpnt, ColourComponent const & p_scalar )
	{
		float fValue;
		p_cpnt.convert_to( fValue );
		ColourComponent cpnt( &fValue );
		cpnt *= p_scalar;
		return fValue;
	}

	float operator /( ColourComponent const & p_cpnt, ColourComponent const & p_scalar )
	{
		float fValue;
		p_cpnt.convert_to( fValue );
		ColourComponent cpnt( &fValue );
		cpnt /= p_scalar;
		return fValue;
	}
}
