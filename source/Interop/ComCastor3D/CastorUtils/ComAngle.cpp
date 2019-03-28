#include "ComCastor3D/CastorUtils/ComAngle.hpp"

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
		return retrieveValue( pVal, [this]()
		{
			return float( castor::Angle::cos() );
		} );
	}

	STDMETHODIMP CAngle::Sin( /* [out, retval] */ float * pVal )
	{
		return retrieveValue( pVal, [this]()
		{
			return float( castor::Angle::sin() );
		} );
	}

	STDMETHODIMP CAngle::Tan( /* [out, retval] */ float * pVal )
	{
		return retrieveValue( pVal, [this]()
		{
			return float( castor::Angle::tan() );
		} );
	}

	STDMETHODIMP CAngle::Cosh( /* [out, retval] */ float * pVal )
	{
		return retrieveValue( pVal, [this]()
		{
			return float( castor::Angle::cosh() );
		} );
	}

	STDMETHODIMP CAngle::Sinh( /* [out, retval] */ float * pVal )
	{
		return retrieveValue( pVal, [this]()
		{
			return float( castor::Angle::sinh() );
		} );
	}

	STDMETHODIMP CAngle::Tanh( /* [out, retval] */ float * pVal )
	{
		return retrieveValue( pVal, [this]()
		{
			return float( castor::Angle::tanh() );
		} );
	}

	STDMETHODIMP CAngle::ACos( /* [in] */ float val )
	{
		castor::Angle::acos( val );
		return S_OK;
	}

	STDMETHODIMP CAngle::ASin( /* [in] */ float val )
	{
		castor::Angle::asin( val );
		return S_OK;
	}

	STDMETHODIMP CAngle::ATan( /* [in] */ float val )
	{
		castor::Angle::atan( val );
		return S_OK;
	}
}
