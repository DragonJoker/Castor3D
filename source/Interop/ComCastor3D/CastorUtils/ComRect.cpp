#include "ComCastor3D/CastorUtils/ComRect.hpp"
#include "ComCastor3D/CastorUtils/ComPosition.hpp"

namespace CastorCom
{
	STDMETHODIMP CRect::Set( /* [in] */ INT left, /* [in] */ INT top, /* [in] */ INT right, /* [in] */ INT bottom )noexcept
	{
		m_internal.set( left, top, right, bottom );
		return S_OK;
	}

	STDMETHODIMP CRect::IntersectsPosition( /* [in] */ IPosition * pos, /* [in] */ eINTERSECTION * pVal )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( pVal )
		{
			*pVal = eINTERSECTION( m_internal.intersects( static_cast< CPosition * >( pos )->getInternal() ) );
			hr = S_OK;
		}

		return S_OK;
	}

	STDMETHODIMP CRect::IntersectsRectangle( /* [in] */ IRect * rect, /* [in] */ eINTERSECTION * pVal )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( pVal )
		{
			*pVal = eINTERSECTION( m_internal.intersects( static_cast< CRect * >( rect )->getInternal() ) );
			hr = S_OK;
		}

		return S_OK;
	}
}
