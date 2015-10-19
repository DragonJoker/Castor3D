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
		HRESULT hr = E_POINTER;

		if ( *pVal )
		{
			*pVal = float( Castor::Angle::cos() );
			hr = S_OK;
		}

		return hr;
	}

	STDMETHODIMP CAngle::Sin( /* [out, retval] */ float * pVal )
	{
		HRESULT hr = E_POINTER;

		if ( *pVal )
		{
			*pVal = float( Castor::Angle::sin() );
			hr = S_OK;
		}

		return hr;
	}

	STDMETHODIMP CAngle::Tan( /* [out, retval] */ float * pVal )
	{
		HRESULT hr = E_POINTER;

		if ( *pVal )
		{
			*pVal = float( Castor::Angle::tan() );
			hr = S_OK;
		}

		return hr;
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

	STDMETHODIMP CAngle::Cosh( /* [out, retval] */ float * pVal )
	{
		HRESULT hr = E_POINTER;

		if ( *pVal )
		{
			*pVal = float( Castor::Angle::cosh() );
			hr = S_OK;
		}

		return hr;
	}

	STDMETHODIMP CAngle::Sinh( /* [out, retval] */ float * pVal )
	{
		HRESULT hr = E_POINTER;

		if ( *pVal )
		{
			*pVal = float( Castor::Angle::sinh() );
			hr = S_OK;
		}

		return hr;
	}

	STDMETHODIMP CAngle::Tanh( /* [out, retval] */ float * pVal )
	{
		HRESULT hr = E_POINTER;

		if ( *pVal )
		{
			*pVal = float( Castor::Angle::tanh() );
			hr = S_OK;
		}

		return hr;
	}
}
