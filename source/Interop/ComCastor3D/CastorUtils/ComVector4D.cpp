#include "ComCastor3D/CastorUtils/ComVector4D.hpp"
#include "ComCastor3D/ComUtils.hpp"

namespace CastorCom
{
	STDMETHODIMP CVector4D::Negate()noexcept
	{
		castor::point::negate( m_internal );
		return S_OK;
	}

	STDMETHODIMP CVector4D::Normalise()noexcept
	{
		castor::point::normalise( m_internal );
		return S_OK;
	}

	STDMETHODIMP CVector4D::Dot( IVector4D * pVal, FLOAT * pRet )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( pVal && pRet )
		{
			*pRet = castor::point::dot( m_internal
				, static_cast< CVector4D * >( pVal )->getInternal() );
			hr = S_OK;
		}

		return hr;
	}

	STDMETHODIMP CVector4D::Length( FLOAT * pVal )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( pVal && pVal )
		{
			*pVal = FLOAT( castor::point::length( m_internal ) );
			hr = S_OK;
		}

		return hr;
	}
}
