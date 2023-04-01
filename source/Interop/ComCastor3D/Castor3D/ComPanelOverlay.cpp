#include "ComCastor3D/Castor3D/ComPanelOverlay.hpp"

#include "ComCastor3D/Castor3D/ComMaterial.hpp"

namespace CastorCom
{
	namespace pnlovl
	{
		static const tstring ERROR_UNINITIALISED = _T( "The panel must be initialised" );
	}

	bool CPanelOverlay::isVisible( castor3d::PanelOverlay const * ov )
	{
		return ov->isVisible();
	}

	void CPanelOverlay::setVisible( castor3d::PanelOverlay * ov, bool v )
	{
		ov->setVisible( v );
	}

	STDMETHODIMP CPanelOverlay::get_Material( /* [out, retval] */ IMaterial ** pVal )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			if ( pVal )
			{
				hr = CMaterial::CreateInstance( pVal );

				if ( hr == S_OK )
				{
					static_cast< CMaterial * >( *pVal )->setInternal( m_internal->getMaterial() );
				}
			}
		}
		else
		{
			hr = CComError::dispatchError( E_POINTER
				, IID_IPanelOverlay
				, _T( "GetMaterial" )
				, pnlovl::ERROR_UNINITIALISED.c_str()
				, 0
				, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CPanelOverlay::put_Material( /* [in] */ IMaterial * val )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->setMaterial( static_cast< CMaterial * >( val )->getInternal() );
			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError( E_POINTER
				, IID_IPanelOverlay
				, _T( "SetMaterial" )
				, pnlovl::ERROR_UNINITIALISED.c_str()
				, 0
				, nullptr );
		}

		return hr;
	}
}
