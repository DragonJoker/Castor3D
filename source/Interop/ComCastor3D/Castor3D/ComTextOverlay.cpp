#include "ComCastor3D/Castor3D/ComTextOverlay.hpp"

#include "ComCastor3D/Castor3D/ComMaterial.hpp"

namespace CastorCom
{
	namespace txtovl
	{
		static const tstring ERROR_UNINITIALISED = _T( "The text must be initialised" );
	}

	STDMETHODIMP CTextOverlay::get_Font( /* [out, retval] */ BSTR * pVal )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			if ( pVal )
			{
				*pVal = toBstr( m_internal->getFontName() );
				hr = S_OK;
			}
		}
		else
		{
			hr = CComError::dispatchError( E_POINTER
				, IID_ITextOverlay
				, _T( "GetMaterial" )
				, txtovl::ERROR_UNINITIALISED.c_str()
				, 0
				, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CTextOverlay::put_Font( /* [in] */ BSTR val )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->setFont( fromBstr( val ) );
			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError( E_POINTER
				, IID_ITextOverlay
				, _T( "SetMaterial" )
				, txtovl::ERROR_UNINITIALISED.c_str()
				, 0
				, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CTextOverlay::get_Material( /* [out, retval] */ IMaterial ** pVal )noexcept
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
				, IID_ITextOverlay
				, _T( "GetMaterial" )
				, txtovl::ERROR_UNINITIALISED.c_str()
				, 0
				, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CTextOverlay::put_Material( /* [in] */ IMaterial * val )noexcept
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
				, IID_ITextOverlay
				, _T( "SetMaterial" )
				, txtovl::ERROR_UNINITIALISED.c_str()
				, 0
				, nullptr );
		}

		return hr;
	}
}
