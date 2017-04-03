#include "ComAngle.hpp"

namespace CastorCom
{
	CAngle::CAngle()
	{
	}

	CAngle::~CAngle()
	{
	}

	STDMETHODIMP CAngle::Cos( /* [out, retval] */ float * pVal )
	{
		return RetrieveValue( pVal, [this]()
		{
			return float( Castor::Angle::cos() );
		} );
	}

	STDMETHODIMP CAngle::Sin( /* [out, retval] */ float * pVal )
	{
		return RetrieveValue( pVal, [this]()
		{
			return float( Castor::Angle::sin() );
		} );
	}

	STDMETHODIMP CAngle::Tan( /* [out, retval] */ float * pVal )
	{
		return RetrieveValue( pVal, [this]()
		{
			return float( Castor::Angle::tan() );
		} );
	}

	STDMETHODIMP CAngle::Cosh( /* [out, retval] */ float * pVal )
	{
		return RetrieveValue( pVal, [this]()
		{
			return float( Castor::Angle::cosh() );
		} );
	}

	STDMETHODIMP CAngle::Sinh( /* [out, retval] */ float * pVal )
	{
		return RetrieveValue( pVal, [this]()
		{
			return float( Castor::Angle::sinh() );
		} );
	}

	STDMETHODIMP CAngle::Tanh( /* [out, retval] */ float * pVal )
	{
		return RetrieveValue( pVal, [this]()
		{
			return float( Castor::Angle::tanh() );
		} );
	}

	STDMETHODIMP CAngle::ACos( /* [in] */ float val )
	{
		Castor::Angle::acos( val );
		return S_OK;
	}

	STDMETHODIMP CAngle::ASin( /* [in] */ float val )
	{
		Castor::Angle::asin( val );
		return S_OK;
	}

	STDMETHODIMP CAngle::ATan( /* [in] */ float val )
	{
		Castor::Angle::atan( val );
		return S_OK;
	}
}
