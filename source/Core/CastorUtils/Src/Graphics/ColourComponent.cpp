#include "ColourComponent.hpp"

namespace Castor
{
	bool operator ==( ColourComponent const & p_cpnA, ColourComponent const & p_cpnB )
	{
		uint8_t l_uiA;
		uint8_t l_uiB;
		p_cpnA.convert_to( l_uiA );
		p_cpnB.convert_to( l_uiB );
		return l_uiA == l_uiB;
	}

	bool operator !=( ColourComponent const & p_cpnA, ColourComponent const & p_cpnB )
	{
		return !operator ==( p_cpnA, p_cpnB );
	}

	float operator -( ColourComponent const & p_cpnt, ColourComponent const & p_scalar )
	{
		float l_fValue;
		p_cpnt.convert_to( l_fValue );
		ColourComponent l_cpnt( &l_fValue );
		l_cpnt -= p_scalar;
		return l_fValue;
	}

	float operator +( ColourComponent const & p_cpnt, ColourComponent const & p_scalar )
	{
		float l_fValue;
		p_cpnt.convert_to( l_fValue );
		ColourComponent l_cpnt( &l_fValue );
		l_cpnt += p_scalar;
		return l_fValue;
	}

	float operator *( ColourComponent const & p_cpnt, ColourComponent const & p_scalar )
	{
		float l_fValue;
		p_cpnt.convert_to( l_fValue );
		ColourComponent l_cpnt( &l_fValue );
		l_cpnt *= p_scalar;
		return l_fValue;
	}

	float operator /( ColourComponent const & p_cpnt, ColourComponent const & p_scalar )
	{
		float l_fValue;
		p_cpnt.convert_to( l_fValue );
		ColourComponent l_cpnt( &l_fValue );
		l_cpnt /= p_scalar;
		return l_fValue;
	}
}
