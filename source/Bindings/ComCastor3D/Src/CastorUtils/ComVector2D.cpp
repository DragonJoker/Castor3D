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
		Castor::point::negate( *this );
		return S_OK;
	}

	STDMETHODIMP CVector2D::Normalise()
	{
		Castor::point::normalise( *this );
		return S_OK;
	}

	STDMETHODIMP CVector2D::Dot( IVector2D * pVal, FLOAT * pRet )
	{
		HRESULT hr = E_POINTER;

		if ( pVal && pRet )
		{
			*pRet = Castor::point::dot( *this, *reinterpret_cast< CVector2D * >( pVal ) );
			return S_OK;
		}

		return hr;
	}

	STDMETHODIMP CVector2D::Length( FLOAT * pVal )
	{
		HRESULT hr = E_POINTER;

		if ( pVal && pVal )
		{
			*pVal = FLOAT( Castor::point::distance( *this ) );
			return S_OK;
		}

		return hr;
	}
}
