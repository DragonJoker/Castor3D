#include "ComCastor3D/Castor3D/ComBorderPanelOverlay.hpp"

#include "ComCastor3D/Castor3D/ComMaterial.hpp"

namespace CastorCom
{
	namespace bdrovl
	{
		static const tstring ERROR_UNINITIALISED = _T( "The border panel must be initialised" );
	}

	STDMETHODIMP CBorderPanelOverlay::get_Material( /* [out, retval] */ IMaterial ** pVal )noexcept
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
				, bdrovl::ERROR_UNINITIALISED.c_str()
				, 0
				, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CBorderPanelOverlay::put_Material( /* [in] */ IMaterial * val )noexcept
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
				, bdrovl::ERROR_UNINITIALISED.c_str()
				, 0
				, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CBorderPanelOverlay::get_BorderMaterial( /* [out, retval] */ IMaterial ** pVal )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			if ( pVal )
			{
				hr = CMaterial::CreateInstance( pVal );

				if ( hr == S_OK )
				{
					static_cast< CMaterial * >( *pVal )->setInternal( m_internal->getBorderMaterial() );
				}
			}
		}
		else
		{
			hr = CComError::dispatchError( E_POINTER
				, IID_IBorderPanelOverlay
				, _T( "GetBorderMaterial" )
				, bdrovl::ERROR_UNINITIALISED.c_str()
				, 0
				, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CBorderPanelOverlay::put_BorderMaterial( /* [in] */ IMaterial * val )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->setBorderMaterial( static_cast< CMaterial * >( val )->getInternal() );
			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError( E_POINTER
				, IID_IBorderPanelOverlay
				, _T( "SetBorderMaterial" )
				, bdrovl::ERROR_UNINITIALISED.c_str()
				, 0
				, nullptr );
		}

		return hr;
	}
}
