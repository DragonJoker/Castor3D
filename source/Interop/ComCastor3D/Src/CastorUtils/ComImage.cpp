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
				Castor3D::Engine * l_engine = l_engn->GetInternal();
				Castor::String l_name = FromBstr( name );
				Castor::Path l_path = FromBstr( val );
				Castor::Path l_pathImage = l_path;

				if ( !Castor::File::FileExists( l_pathImage ) )
				{
					l_pathImage = l_engine->GetDataDirectory() / l_path;
				}

				if ( !Castor::File::FileExists( l_pathImage ) )
				{
					l_pathImage = l_engine->GetDataDirectory() / cuT( "Texture" ) / l_path;
				}

				m_image = l_engine->GetImageManager().create( l_name, l_pathImage );

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
			Castor3D::Engine * l_engine = l_engn->GetInternal();
			Castor::String l_name = FromBstr( name );

			if ( !m_image )
			{
				hr = S_OK;
				m_image = l_engine->GetImageManager().create( l_name, *static_cast< CSize * >( size ), Castor::ePIXEL_FORMAT( fmt ) );
			}
			else
			{
				hr = CComError::DispatchError( E_FAIL, IID_IImage, cuT( "LoadFromFormat" ), ERROR_IMAGE_EXISTS, 0, NULL );
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
			m_image->Resample( *static_cast< CSize * >( val ) );
		}
		else
		{
			hr = CComError::DispatchError( E_FAIL, IID_IImage, cuT( "Resample" ), ERROR_UNINITIALISED_IMAGE, 0, NULL );
		}

		return hr;
	}

	STDMETHODIMP CImage::Fill( /* [in] */ IColour * val )
	{
		HRESULT hr = E_POINTER;

		if ( m_image )
		{
			hr = S_OK;
			m_image->Fill( *static_cast< CColour * >( val ) );
		}
		else
		{
			hr = CComError::DispatchError( E_FAIL, IID_IImage, cuT( "Fill" ), ERROR_UNINITIALISED_IMAGE, 0, NULL );
		}

		return hr;
	}

	STDMETHODIMP CImage::CopyImage( /* [in] */ IImage * val )
	{
		HRESULT hr = E_POINTER;

		if ( m_image )
		{
			hr = S_OK;
			m_image->CopyImage( *static_cast< CImage * >( val )->m_image );
		}
		else
		{
			hr = CComError::DispatchError( E_FAIL, IID_IImage, cuT( "CopyImage" ), ERROR_UNINITIALISED_IMAGE, 0, NULL );
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
				Castor::Image l_img = m_image->SubImage( *static_cast< CRect * >( val ) );
				static_cast< CImage * >( *pVal )->m_image = std::make_shared< Castor::Image >( l_img );
			}
		}
		else
		{
			hr = CComError::DispatchError( E_FAIL, IID_IImage, cuT( "SubImage" ), ERROR_UNINITIALISED_IMAGE, 0, NULL );
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
				Castor::Image l_img = m_image->Flip();
				static_cast< CImage * >( *pVal )->m_image = std::make_shared< Castor::Image >( l_img );
			}
		}
		else
		{
			hr = CComError::DispatchError( E_FAIL, IID_IImage, cuT( "Flip" ), ERROR_UNINITIALISED_IMAGE, 0, NULL );
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
				Castor::Image l_img = m_image->Mirror();
				static_cast< CImage * >( *pVal )->m_image = std::make_shared< Castor::Image >( l_img );
			}
		}
		else
		{
			hr = CComError::DispatchError( E_FAIL, IID_IImage, cuT( "Mirror" ), ERROR_UNINITIALISED_IMAGE, 0, NULL );
		}

		return hr;
	}
}
