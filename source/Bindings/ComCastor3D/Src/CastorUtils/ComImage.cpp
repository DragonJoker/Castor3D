#include "ComImage.hpp"
#include "ComColour.hpp"
#include "ComRect.hpp"
#include "ComSize.hpp"
#include "ComUtils.hpp"
#include "ComEngine.hpp"

namespace CastorCom
{
	static const Castor::String ERROR_WRONG_FILE_NAME = cuT( "The given file doesn't exist" );
	static const Castor::String ERROR_IMAGE_EXISTS = cuT( "The given image name already exists" );
	static const Castor::String ERROR_UNINITIALISED_IMAGE = cuT( "The image must be initialised" );
	static const Castor::String ERROR_INITIALISED_IMAGE = cuT( "The image is already initialised" );

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
				Castor3D::ImageCollection & l_mgr = l_engine->GetImageManager();
				Castor::String l_name = FromBstr( name );
				Castor::Path l_path = FromBstr( val );
				m_image = l_mgr.find( l_name );
				Castor::Path l_pathImage = l_path;

				if ( !Castor::File::FileExists( l_pathImage ) )
				{
					l_pathImage = l_engine->GetDataPath() / l_path;
				}

				if ( !Castor::File::FileExists( l_pathImage ) )
				{
					l_pathImage = l_engine->GetDataPath() / cuT( "Texture" ) / l_path;
				}

				if ( Castor::File::FileExists( l_pathImage ) )
				{
					if ( !m_image )
					{
						m_image = std::make_shared< Castor::Image >( l_name, l_pathImage );
					}
					else
					{
						Castor::Image::BinaryLoader()( *m_image, l_pathImage );
					}

					hr = S_OK;
				}
				else
				{
					hr = CComError::DispatchError(
							 E_FAIL,						// This represents the error
							 IID_IImage,					// This is the GUID of component throwing error
							 cuT( "LoadFromFile" ),			// This is generally displayed as the title
							 ERROR_WRONG_FILE_NAME.c_str(),	// This is the description
							 0,								// This is the context in the help file
							 NULL );
				}
			}
		}
		else
		{
			hr = CComError::DispatchError(
					 E_FAIL,							// This represents the error
					 IID_IImage,						// This is the GUID of component throwing error
					 cuT( "LoadFromFile" ),				// This is generally displayed as the title
					 ERROR_INITIALISED_IMAGE.c_str(),	// This is the description
					 0,									// This is the context in the help file
					 NULL );
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
			Castor3D::ImageCollection & l_mgr = l_engine->GetImageManager();
			Castor::String l_name = FromBstr( name );
			m_image = l_mgr.find( l_name );

			if ( !m_image )
			{
				hr = S_OK;
				m_image = std::make_shared< Castor::Image >( l_name, *static_cast< CSize * >( size ), Castor::ePIXEL_FORMAT( fmt ) );
			}
			else
			{
				hr = CComError::DispatchError(
						 E_FAIL,						// This represents the error
						 IID_IImage,					// This is the GUID of component throwing error
						 cuT( "LoadFromFormat" ),		// This is generally displayed as the title
						 ERROR_IMAGE_EXISTS.c_str(),	// This is the description
						 0,								// This is the context in the help file
						 NULL );
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
			hr = CComError::DispatchError(
					 E_FAIL,							// This represents the error
					 IID_IImage,						// This is the GUID of component throwing error
					 cuT( "Resample" ),					// This is generally displayed as the title
					 ERROR_UNINITIALISED_IMAGE.c_str(),	// This is the description
					 0,									// This is the context in the help file
					 NULL );
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
			hr = CComError::DispatchError(
					 E_FAIL,							// This represents the error
					 IID_IImage,						// This is the GUID of component throwing error
					 cuT( "Fill" ),						// This is generally displayed as the title
					 ERROR_UNINITIALISED_IMAGE.c_str(),	// This is the description
					 0,									// This is the context in the help file
					 NULL );
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
			hr = CComError::DispatchError(
					 E_FAIL,							// This represents the error
					 IID_IImage,						// This is the GUID of component throwing error
					 cuT( "CopyImage" ),				// This is generally displayed as the title
					 ERROR_UNINITIALISED_IMAGE.c_str(),	// This is the description
					 0,									// This is the context in the help file
					 NULL );
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
			hr = CComError::DispatchError(
					 E_FAIL,							// This represents the error
					 IID_IImage,						// This is the GUID of component throwing error
					 cuT( "SubImage" ),					// This is generally displayed as the title
					 ERROR_UNINITIALISED_IMAGE.c_str(),	// This is the description
					 0,									// This is the context in the help file
					 NULL );
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
			hr = CComError::DispatchError(
					 E_FAIL,							// This represents the error
					 IID_IImage,						// This is the GUID of component throwing error
					 cuT( "Flip" ),						// This is generally displayed as the title
					 ERROR_UNINITIALISED_IMAGE.c_str(),	// This is the description
					 0,									// This is the context in the help file
					 NULL );
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
			hr = CComError::DispatchError(
					 E_FAIL,							// This represents the error
					 IID_IImage,						// This is the GUID of component throwing error
					 cuT( "Mirror" ),					// This is generally displayed as the title
					 ERROR_UNINITIALISED_IMAGE.c_str(),	// This is the description
					 0,									// This is the context in the help file
					 NULL );
		}

		return hr;
	}
}
