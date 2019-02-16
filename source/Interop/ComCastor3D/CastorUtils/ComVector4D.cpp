#include "ComCastor3D/CastorUtils/ComVector4D.hpp"
#include "ComCastor3D/ComUtils.hpp"

namespace CastorCom
{
	CVector4D::CVector4D()
	{
	}

	CVector4D::~CVector4D()
	{
	}

	STDMETHODIMP CVector4D::Negate()
	{
		castor::point::negate( *this );
		return S_OK;
	}

	STDMETHODIMP CVector4D::Normalise()
	{
		castor::point::normalise( *this );
		return S_OK;
	}

	STDMETHODIMP CVector4D::Dot( IVector4D * pVal, FLOAT * pRet )
	{
		HRESULT hr = E_POINTER;

		if ( pVal && pRet )
		{
			*pRet = castor::point::dot( *this, *reinterpret_cast< CVector4D * >( pVal ) );
			hr = S_OK;
		}

		return hr;
	}

	STDMETHODIMP CVector4D::Length( FLOAT * pVal )
	{
		HRESULT hr = E_POINTER;

		if ( pVal && pVal )
		{
			*pVal = FLOAT( castor::point::length( *this ) );
			hr = S_OK;
		}

		return hr;
	}
}
