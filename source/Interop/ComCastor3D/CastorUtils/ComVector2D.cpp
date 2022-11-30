#include "ComCastor3D/CastorUtils/ComVector2D.hpp"
#include "ComCastor3D/ComUtils.hpp"

namespace CastorCom
{
	STDMETHODIMP CVector2D::Negate()noexcept
	{
		castor::point::negate( m_internal );
		return S_OK;
	}

	STDMETHODIMP CVector2D::Normalise()noexcept
	{
		castor::point::normalise( m_internal );
		return S_OK;
	}

	STDMETHODIMP CVector2D::Dot( IVector2D * pVal, FLOAT * pRet )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( pVal && pRet )
		{
			*pRet = castor::point::dot( m_internal
				, static_cast< CVector2D * >( pVal )->getInternal() );
			return S_OK;
		}

		return hr;
	}

	STDMETHODIMP CVector2D::Length( FLOAT * pVal )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( pVal && pVal )
		{
			*pVal = FLOAT( castor::point::length( m_internal ) );
			return S_OK;
		}

		return hr;
	}
}
