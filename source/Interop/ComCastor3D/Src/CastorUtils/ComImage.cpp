#include "ComImage.hpp"
#include "ComColour.hpp"
#include "ComRect.hpp"
#include "ComSize.hpp"
#include "ComUtils.hpp"
#include "ComEngine.hpp"

namespace CastorCom
{
	static const TCHAR * ERROR_WRONG_FILE_NAME = _T( "The given file doesn't exist" );
	static const TCHAR * ERROR_IMAGE_EXISTS = _T( "The given image name already exists" );
	static const TCHAR * ERROR_UNINITIALISED_IMAGE = _T( "The image must be initialised" );
	static const TCHAR * ERROR_INITIALISED_IMAGE = _T( "The image is already initialised" );

	CImage::CImage()
	{
	}

	CImage::~CImage()
	{
	}

	STDMETHODIMP CImage::LoadFromFile( /* [in] */ IEngine * engine, /* [in] */ BSTR name, /* [in] */ BSTR val )
	{
		HRESULT hr = E_POINTER;

		if ( !m_image )
		{
			if ( engine )
			{
				CEngine * l_engn = static_cast< CEngine * >( engine );
				castor3d::Engine * l_engine = l_engn->getInternal();
				castor::String l_name = FromBstr( name );
				castor::Path l_path{ FromBstr( val ) };
				castor::Path l_pathImage = l_path;

				if ( !castor::File::fileExists( l_pathImage ) )
				{
					l_pathImage = l_engine->getDataDirectory() / l_path;
				}

				if ( !castor::File::fileExists( l_pathImage ) )
				{
					l_pathImage = l_engine->getDataDirectory() / cuT( "Texture" ) / l_path;
				}

				m_image = l_engine->getImageCache().add( l_name, l_pathImage );

				if ( !m_image )
				{
					hr = CComError::DispatchError( E_FAIL, IID_IImage, cuT( "LoadFromFile" ), ERROR_WRONG_FILE_NAME, 0, NULL );
				}
			}
		}
		else
		{
			hr = CComError::DispatchError( E_FAIL, IID_IImage, cuT( "LoadFromFile" ), ERROR_IMAGE_EXISTS, 0, NULL );
		}

		return hr;
	}

	STDMETHODIMP CImage::LoadFromFormat( /* [in] */ IEngine * engine, /* [in] */ BSTR name, /* [in] */ ePIXEL_FORMAT fmt, /* [in] */ ISize * size )
	{
		HRESULT hr = E_POINTER;

		if ( engine )
		{
			CEngine * l_engn = static_cast< CEngine * >( engine );
			castor3d::Engine * l_engine = l_engn->getInternal();
			castor::String l_name = FromBstr( name );

			if ( !m_image )
			{
				hr = S_OK;
				m_image = l_engine->getImageCache().add( l_name, *static_cast< CSize * >( size ), castor::PixelFormat( fmt ) );
			}
			else
			{
				hr = CComError::DispatchError( E_FAIL, IID_IImage, cuT( "LoadFromFormat" ), ERROR_IMAGE_EXISTS, 0, NULL );
			}
		}

		return hr;
	}

	STDMETHODIMP CImage::resample( /* [in] */ ISize * val )
	{
		HRESULT hr = E_POINTER;

		if ( m_image )
		{
			hr = S_OK;
			m_image->resample( *static_cast< CSize * >( val ) );
		}
		else
		{
			hr = CComError::DispatchError( E_FAIL, IID_IImage, cuT( "resample" ), ERROR_UNINITIALISED_IMAGE, 0, NULL );
		}

		return hr;
	}

	STDMETHODIMP CImage::fill( /* [in] */ IColour * val )
	{
		HRESULT hr = E_POINTER;

		if ( m_image )
		{
			hr = S_OK;
			m_image->fill( *static_cast< CColour * >( val ) );
		}
		else
		{
			hr = CComError::DispatchError( E_FAIL, IID_IImage, cuT( "Fill" ), ERROR_UNINITIALISED_IMAGE, 0, NULL );
		}

		return hr;
	}

	STDMETHODIMP CImage::copyImage( /* [in] */ IImage * val )
	{
		HRESULT hr = E_POINTER;

		if ( m_image )
		{
			hr = S_OK;
			m_image->copyImage( *static_cast< CImage * >( val )->m_image );
		}
		else
		{
			hr = CComError::DispatchError( E_FAIL, IID_IImage, cuT( "copyImage" ), ERROR_UNINITIALISED_IMAGE, 0, NULL );
		}

		return hr;
	}

	STDMETHODIMP CImage::subImage( /* [in] */ IRect * val, /* [out, retval] */ IImage ** pVal )
	{
		HRESULT hr = E_POINTER;

		if ( m_image )
		{
			hr = CImage::CreateInstance( pVal );

			if ( hr == S_OK )
			{
				castor::Image l_img = m_image->subImage( *static_cast< CRect * >( val ) );
				static_cast< CImage * >( *pVal )->m_image = std::make_shared< castor::Image >( l_img );
			}
		}
		else
		{
			hr = CComError::DispatchError( E_FAIL, IID_IImage, cuT( "subImage" ), ERROR_UNINITIALISED_IMAGE, 0, NULL );
		}

		return hr;
	}

	STDMETHODIMP CImage::flip( /* [out, retval] */ IImage ** pVal )
	{
		HRESULT hr = E_POINTER;

		if ( m_image )
		{
			hr = CImage::CreateInstance( pVal );

			if ( hr == S_OK )
			{
				castor::Image l_img = m_image->flip();
				static_cast< CImage * >( *pVal )->m_image = std::make_shared< castor::Image >( l_img );
			}
		}
		else
		{
			hr = CComError::DispatchError( E_FAIL, IID_IImage, cuT( "Flip" ), ERROR_UNINITIALISED_IMAGE, 0, NULL );
		}

		return hr;
	}

	STDMETHODIMP CImage::mirror( /* [out, retval] */ IImage ** pVal )
	{
		HRESULT hr = E_POINTER;

		if ( m_image )
		{
			hr = CImage::CreateInstance( pVal );

			if ( hr == S_OK )
			{
				castor::Image l_img = m_image->mirror();
				static_cast< CImage * >( *pVal )->m_image = std::make_shared< castor::Image >( l_img );
			}
		}
		else
		{
			hr = CComError::DispatchError( E_FAIL, IID_IImage, cuT( "Mirror" ), ERROR_UNINITIALISED_IMAGE, 0, NULL );
		}

		return hr;
	}
}
