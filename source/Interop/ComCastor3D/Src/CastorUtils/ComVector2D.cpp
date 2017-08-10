#include "ComVector2D.hpp"
#include "ComUtils.hpp"

namespace CastorCom
{
	CVector2D::CVector2D()
	{
	}

	CVector2D::~CVector2D()
	{
	}

	STDMETHODIMP CVector2D::Negate()
	{
		castor::point::negate( *this );
		return S_OK;
	}

	STDMETHODIMP CVector2D::Normalise()
	{
		castor::point::normalise( *this );
		return S_OK;
	}

	STDMETHODIMP CVector2D::dot( IVector2D * pVal, FLOAT * pRet )
	{
		HRESULT hr = E_POINTER;

		if ( pVal && pRet )
		{
			*pRet = castor::point::dot( *this, *reinterpret_cast< CVector2D * >( pVal ) );
			return S_OK;
		}

		return hr;
	}

	STDMETHODIMP CVector2D::Length( FLOAT * pVal )
	{
		HRESULT hr = E_POINTER;

		if ( pVal && pVal )
		{
			*pVal = FLOAT( castor::point::length( *this ) );
			return S_OK;
		}

		return hr;
	}
}
