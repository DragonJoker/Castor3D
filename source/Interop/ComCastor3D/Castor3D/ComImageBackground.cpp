#include "ComCastor3D/Castor3D/ComImageBackground.hpp"

namespace CastorCom
{
	static const tstring ERROR_UNINITIALISED = _T( "The ImageBackground must be initialised" );

	CImageBackground::CImageBackground()
	{
	}

	CImageBackground::~CImageBackground()
	{
	}

	STDMETHODIMP CImageBackground::put_Image( BSTR filePath )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			auto path = castor::Path{ fromBstr( filePath ) };
			m_internal->loadImage( path.getPath(), path.getFileName( true ) );
		}
		else
		{
			hr = CComError::dispatchError(
				E_FAIL,						// This represents the error
				IID_IImageBackground,		// This is the GUID of PixelComponents throwing error
				_T( "CreateTextureUnit" ),	// This is generally displayed as the title
				ERROR_UNINITIALISED.c_str(),	// This is the description
				0,								// This is the context in the help file
				nullptr );
		}

		return hr;
	}
}
