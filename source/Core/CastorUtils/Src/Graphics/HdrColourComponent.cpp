#include "HdrColourComponent.hpp"

namespace Castor
{
	bool operator==( HdrColourComponent const & p_cpnA, HdrColourComponent const & p_cpnB )
	{
		float l_uiA;
		float l_uiB;
		p_cpnA.convert_to( l_uiA );
		p_cpnB.convert_to( l_uiB );
		return l_uiA == l_uiB;
	}

	bool operator!=( HdrColourComponent const & p_cpnA, HdrColourComponent const & p_cpnB )
	{
		return !( p_cpnA == p_cpnB );
	}

	float operator-( HdrColourComponent const & p_cpnt, HdrColourComponent const & p_scalar )
	{
		float l_fValue;
		p_cpnt.convert_to( l_fValue );
		HdrColourComponent l_cpnt( &l_fValue );
		l_cpnt -= p_scalar;
		return l_fValue;
	}

	float operator+( HdrColourComponent const & p_cpnt, HdrColourComponent const & p_scalar )
	{
		float l_fValue;
		p_cpnt.convert_to( l_fValue );
		HdrColourComponent l_cpnt( &l_fValue );
		l_cpnt += p_scalar;
		return l_fValue;
	}

	float operator*( HdrColourComponent const & p_cpnt, HdrColourComponent const & p_scalar )
	{
		float l_fValue;
		p_cpnt.convert_to( l_fValue );
		HdrColourComponent l_cpnt( &l_fValue );
		l_cpnt *= p_scalar;
		return l_fValue;
	}

	float operator/( HdrColourComponent const & p_cpnt, HdrColourComponent const & p_scalar )
	{
		float l_fValue;
		p_cpnt.convert_to( l_fValue );
		HdrColourComponent l_cpnt( &l_fValue );
		l_cpnt /= p_scalar;
		return l_fValue;
	}
}
