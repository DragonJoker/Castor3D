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
			*pVal = Castor::Angle::Cos();
			hr = S_OK;
		}

		return hr;
	}

	STDMETHODIMP CAngle::Sin( /* [out, retval] */ float * pVal )
	{
		HRESULT hr = E_POINTER;

		if ( *pVal )
		{
			*pVal = Castor::Angle::Sin();
			hr = S_OK;
		}

		return hr;
	}

	STDMETHODIMP CAngle::Tan( /* [out, retval] */ float * pVal )
	{
		HRESULT hr = E_POINTER;

		if ( *pVal )
		{
			*pVal = Castor::Angle::Tan();
			hr = S_OK;
		}

		return hr;
	}

	STDMETHODIMP CAngle::ACos( /* [in] */ float val )
	{
		Castor::Angle::ACos( val );
		return S_OK;
	}

	STDMETHODIMP CAngle::ASin( /* [in] */ float val )
	{
		Castor::Angle::ASin( val );
		return S_OK;
	}

	STDMETHODIMP CAngle::ATan( /* [in] */ float val )
	{
		Castor::Angle::ATan( val );
		return S_OK;
	}

	STDMETHODIMP CAngle::Cosh( /* [out, retval] */ float * pVal )
	{
		HRESULT hr = E_POINTER;

		if ( *pVal )
		{
			*pVal = Castor::Angle::Cosh();
			hr = S_OK;
		}

		return hr;
	}

	STDMETHODIMP CAngle::Sinh( /* [out, retval] */ float * pVal )
	{
		HRESULT hr = E_POINTER;

		if ( *pVal )
		{
			*pVal = Castor::Angle::Sinh();
			hr = S_OK;
		}

		return hr;
	}

	STDMETHODIMP CAngle::Tanh( /* [out, retval] */ float * pVal )
	{
		HRESULT hr = E_POINTER;

		if ( *pVal )
		{
			*pVal = Castor::Angle::Tanh();
			hr = S_OK;
		}

		return hr;
	}
}
