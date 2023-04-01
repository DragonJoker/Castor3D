#include "ComCastor3D/Castor3D/ComOverlayCategory.hpp"

#include "ComCastor3D/Castor3D/ComMaterial.hpp"

namespace CastorCom
{
	namespace ovlcat
	{
		static const tstring ERROR_UNINITIALISED = _T( "The category must be initialised" );
	}

	STDMETHODIMP COverlayCategory::get_Material( /* [out, retval] */ IMaterial ** pVal )noexcept
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
				, IID_IOverlayCategory
				, _T( "GetMaterial" )
				, ovlcat::ERROR_UNINITIALISED.c_str()
				, 0
				, nullptr );
		}

		return hr;
	}

	STDMETHODIMP COverlayCategory::put_Material( /* [in] */ IMaterial * val )noexcept
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
				, IID_IOverlayCategory
				, _T( "SetMaterial" )
				, ovlcat::ERROR_UNINITIALISED.c_str()
				, 0
				, nullptr );
		}

		return hr;
	}
}
