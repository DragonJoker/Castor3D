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

	CImage::CImage()
	{
	}

	CImage::~CImage()
	{
	}

	STDMETHODIMP CImage::LoadFromFile( /* [in] */ IEngine * engine, /* [in] */ BSTR name, /* [in] */ BSTR val, /* [in] */ boolean dropAlpha )
	{
		HRESULT hr = E_POINTER;

		if ( !m_image )
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

				m_image = l_engine->getImageCache().add( l_name
					, l_pathImage );

				if ( !m_image )
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

	STDMETHODIMP CImage::LoadFromFormat( /* [in] */ IEngine * engine, /* [in] */ BSTR name, /* [in] */ ePIXEL_FORMAT fmt, /* [in] */ ISize * size )
	{
		HRESULT hr = E_POINTER;

		if ( engine )
		{
			CEngine * l_engn = static_cast< CEngine * >( engine );
			castor3d::Engine * l_engine = l_engn->getInternal();
			castor::String l_name = fromBstr( name );

			if ( !m_image )
			{
				hr = S_OK;
				m_image = l_engine->getImageCache().add( l_name, *static_cast< CSize * >( size ), castor::PixelFormat( fmt ) );
			}
			else
			{
				hr = CComError::dispatchError( E_FAIL, IID_IImage, _T( "LoadFromFormat" ), ERROR_IMAGE_EXISTS, 0, nullptr );
			}
		}

		return hr;
	}

	STDMETHODIMP CImage::Resample( /* [in] */ ISize * val )
	{
		HRESULT hr = E_POINTER;

		if ( m_image )
		{
			hr = S_OK;
			m_image->resample( *static_cast< CSize * >( val ) );
		}
		else
		{
			hr = CComError::dispatchError( E_FAIL, IID_IImage, _T( "Resample" ), ERROR_UNINITIALISED_IMAGE, 0, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CImage::Fill( /* [in] */ IRgbaColour * val )
	{
		HRESULT hr = E_POINTER;

		if ( m_image )
		{
			hr = S_OK;
			m_image->fill( *static_cast< CRgbaColour * >( val ) );
		}
		else
		{
			hr = CComError::dispatchError( E_FAIL, IID_IImage, _T( "Fill" ), ERROR_UNINITIALISED_IMAGE, 0, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CImage::CopyImage( /* [in] */ IImage * val )
	{
		HRESULT hr = E_POINTER;

		if ( m_image )
		{
			hr = S_OK;
			m_image->copyImage( *static_cast< CImage * >( val )->m_image );
		}
		else
		{
			hr = CComError::dispatchError( E_FAIL, IID_IImage, _T( "CcopyImage" ), ERROR_UNINITIALISED_IMAGE, 0, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CImage::SubImage( /* [in] */ IRect * val, /* [out, retval] */ IImage ** pVal )
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
			hr = CComError::dispatchError( E_FAIL, IID_IImage, _T( "SubImage" ), ERROR_UNINITIALISED_IMAGE, 0, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CImage::Flip( /* [out, retval] */ IImage ** pVal )
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
			hr = CComError::dispatchError( E_FAIL, IID_IImage, _T( "Flip" ), ERROR_UNINITIALISED_IMAGE, 0, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CImage::Mirror( /* [out, retval] */ IImage ** pVal )
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
			hr = CComError::dispatchError( E_FAIL, IID_IImage, _T( "Mirror" ), ERROR_UNINITIALISED_IMAGE, 0, nullptr );
		}

		return hr;
	}
}
