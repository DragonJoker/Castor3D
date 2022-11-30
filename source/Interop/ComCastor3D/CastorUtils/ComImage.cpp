#include "ComCastor3D/CastorUtils/ComImage.hpp"
#include "ComCastor3D/CastorUtils/ComRgbaColour.hpp"
#include "ComCastor3D/CastorUtils/ComRect.hpp"
#include "ComCastor3D/CastorUtils/ComSize.hpp"
#include "ComCastor3D/Castor3D/ComEngine.hpp"
#include "ComCastor3D/ComUtils.hpp"

namespace CastorCom
{
	static const TCHAR * ERROR_WRONG_FILE_NAME = _T( "The given file doesn't exist" );
	static const TCHAR * ERROR_IMAGE_EXISTS = _T( "The given image name already exists" );
	static const TCHAR * ERROR_UNINITIALISED_IMAGE = _T( "The image must be initialised" );
	static const TCHAR * ERROR_INITIALISED_IMAGE = _T( "The image is already initialised" );

	STDMETHODIMP CImage::LoadFromFile( /* [in] */ IEngine * engine, /* [in] */ BSTR name, /* [in] */ BSTR val, /* [in] */ boolean dropAlpha )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( !m_internal )
		{
			if ( engine )
			{
				CEngine * l_engn = static_cast< CEngine * >( engine );
				castor3d::Engine * l_engine = l_engn->getInternal();
				castor::String l_name = fromBstr( name );
				castor::Path l_path{ fromBstr( val ) };
				castor::Path l_pathImage = l_path;

				if ( !castor::File::fileExists( l_pathImage ) )
				{
					l_pathImage = l_engine->getDataDirectory() / l_path;
				}

				if ( !castor::File::fileExists( l_pathImage ) )
				{
					l_pathImage = l_engine->getDataDirectory() / cuT( "Texture" ) / l_path;
				}

				auto img = l_engine->createImage( l_name
					, castor::ImageCreateParams{ l_pathImage } );
				m_internal = l_engine->addImage( l_name, img ).lock();

				if ( !m_internal )
				{
					hr = CComError::dispatchError( E_FAIL, IID_IImage, _T( "LoadFromFile" ), ERROR_WRONG_FILE_NAME, 0, nullptr );
				}
			}
		}
		else
		{
			hr = CComError::dispatchError( E_FAIL, IID_IImage, _T( "LoadFromFile" ), ERROR_IMAGE_EXISTS, 0, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CImage::LoadFromFormat( /* [in] */ IEngine * engine, /* [in] */ BSTR name, /* [in] */ ePIXEL_FORMAT fmt, /* [in] */ ISize * size )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( engine )
		{
			CEngine * l_engn = static_cast< CEngine * >( engine );
			castor3d::Engine * l_engine = l_engn->getInternal();
			castor::String l_name = fromBstr( name );

			if ( !m_internal )
			{
				hr = S_OK;
				auto img = l_engine->createImage( l_name
					, castor::ImageCreateParams{ static_cast< CSize * >( size )->getInternal()
						, castor::PixelFormat( fmt ) } );
				m_internal = l_engine->addImage( l_name, img ).lock();
			}
			else
			{
				hr = CComError::dispatchError( E_FAIL, IID_IImage, _T( "LoadFromFormat" ), ERROR_IMAGE_EXISTS, 0, nullptr );
			}
		}

		return hr;
	}

	STDMETHODIMP CImage::Resample( /* [in] */ ISize * val )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			hr = S_OK;
			m_internal->resample( static_cast< CSize * >( val )->getInternal() );
		}
		else
		{
			hr = CComError::dispatchError( E_FAIL, IID_IImage, _T( "Resample" ), ERROR_UNINITIALISED_IMAGE, 0, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CImage::Fill( /* [in] */ IRgbaColour * val )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			hr = S_OK;
			m_internal->fill( static_cast< CRgbaColour * >( val )->getInternal() );
		}
		else
		{
			hr = CComError::dispatchError( E_FAIL, IID_IImage, _T( "Fill" ), ERROR_UNINITIALISED_IMAGE, 0, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CImage::Flip( /* [out, retval] */ IImage ** pVal )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			hr = CImage::CreateInstance( pVal );

			if ( hr == S_OK )
			{
				castor::Image l_img = m_internal->flip();
				static_cast< CImage * >( *pVal )->m_internal = std::make_shared< castor::Image >( l_img );
			}
		}
		else
		{
			hr = CComError::dispatchError( E_FAIL, IID_IImage, _T( "Flip" ), ERROR_UNINITIALISED_IMAGE, 0, nullptr );
		}

		return hr;
	}
}
