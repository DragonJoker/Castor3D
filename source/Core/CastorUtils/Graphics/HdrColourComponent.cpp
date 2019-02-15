#include "HdrColourComponent.hpp"

namespace castor
{
	bool operator==( HdrColourComponent const & p_cpnA, HdrColourComponent const & p_cpnB )
	{
		float uiA;
		float uiB;
		p_cpnA.convertTo( uiA );
		p_cpnB.convertTo( uiB );
		return uiA == uiB;
	}

	bool operator!=( HdrColourComponent const & p_cpnA, HdrColourComponent const & p_cpnB )
	{
		return !( p_cpnA == p_cpnB );
	}

	float operator-( HdrColourComponent const & p_cpnt, HdrColourComponent const & p_scalar )
	{
		float fValue;
		p_cpnt.convertTo( fValue );
		HdrColourComponent cpnt( &fValue );
		cpnt -= p_scalar;
		return fValue;
	}

	float operator+( HdrColourComponent const & p_cpnt, HdrColourComponent const & p_scalar )
	{
		float fValue;
		p_cpnt.convertTo( fValue );
		HdrColourComponent cpnt( &fValue );
		cpnt += p_scalar;
		return fValue;
	}

	float operator*( HdrColourComponent const & p_cpnt, HdrColourComponent const & p_scalar )
	{
		float fValue;
		p_cpnt.convertTo( fValue );
		HdrColourComponent cpnt( &fValue );
		cpnt *= p_scalar;
		return fValue;
	}

	float operator/( HdrColourComponent const & p_cpnt, HdrColourComponent const & p_scalar )
	{
		float fValue;
		p_cpnt.convertTo( fValue );
		HdrColourComponent cpnt( &fValue );
		cpnt /= p_scalar;
		return fValue;
	}
}
