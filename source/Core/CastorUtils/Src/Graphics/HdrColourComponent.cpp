#include "HdrColourComponent.hpp"

namespace Castor
{
	bool operator==( HdrColourComponent const & p_cpnA, HdrColourComponent const & p_cpnB )
	{
		float uiA;
		float uiB;
		p_cpnA.convert_to( uiA );
		p_cpnB.convert_to( uiB );
		return uiA == uiB;
	}

	bool operator!=( HdrColourComponent const & p_cpnA, HdrColourComponent const & p_cpnB )
	{
		return !( p_cpnA == p_cpnB );
	}

	float operator-( HdrColourComponent const & p_cpnt, HdrColourComponent const & p_scalar )
	{
		float fValue;
		p_cpnt.convert_to( fValue );
		HdrColourComponent cpnt( &fValue );
		cpnt -= p_scalar;
		return fValue;
	}

	float operator+( HdrColourComponent const & p_cpnt, HdrColourComponent const & p_scalar )
	{
		float fValue;
		p_cpnt.convert_to( fValue );
		HdrColourComponent cpnt( &fValue );
		cpnt += p_scalar;
		return fValue;
	}

	float operator*( HdrColourComponent const & p_cpnt, HdrColourComponent const & p_scalar )
	{
		float fValue;
		p_cpnt.convert_to( fValue );
		HdrColourComponent cpnt( &fValue );
		cpnt *= p_scalar;
		return fValue;
	}

	float operator/( HdrColourComponent const & p_cpnt, HdrColourComponent const & p_scalar )
	{
		float fValue;
		p_cpnt.convert_to( fValue );
		HdrColourComponent cpnt( &fValue );
		cpnt /= p_scalar;
		return fValue;
	}
}
