#include "ComCastor3D/Castor3D/ComSkyboxBackground.hpp"

namespace CastorCom
{
	static const castor::String ERROR_UNINITIALISED = cuT( "The SkyboxBackground must be initialised" );

	CSkyboxBackground::CSkyboxBackground()
	{
	}

	CSkyboxBackground::~CSkyboxBackground()
	{
	}

	STDMETHODIMP CSkyboxBackground::put_LeftImage( BSTR filePath )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			auto path = castor::Path{ fromBstr( filePath ) };
			m_internal->loadLeftImage( path.getPath(), path.getFileName( true ) );
		}
		else
		{
			hr = CComError::dispatchError(
				E_FAIL,							// This represents the error
				IID_ISkyboxBackground,			// This is the GUID of PixelComponents throwing error
				cuT( "LeftImage" ),				// This is generally displayed as the title
				ERROR_UNINITIALISED.c_str(),	// This is the description
				0,								// This is the context in the help file
				nullptr );
		}

		return hr;
	}

	STDMETHODIMP CSkyboxBackground::put_RightImage( BSTR filePath )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			auto path = castor::Path{ fromBstr( filePath ) };
			m_internal->loadRightImage( path.getPath(), path.getFileName( true ) );
		}
		else
		{
			hr = CComError::dispatchError(
				E_FAIL,							// This represents the error
				IID_ISkyboxBackground,			// This is the GUID of PixelComponents throwing error
				cuT( "RightImage" ),			// This is generally displayed as the title
				ERROR_UNINITIALISED.c_str(),	// This is the description
				0,								// This is the context in the help file
				nullptr );
		}

		return hr;
	}

	STDMETHODIMP CSkyboxBackground::put_TopImage( BSTR filePath )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			auto path = castor::Path{ fromBstr( filePath ) };
			m_internal->loadTopImage( path.getPath(), path.getFileName( true ) );
		}
		else
		{
			hr = CComError::dispatchError(
				E_FAIL,							// This represents the error
				IID_ISkyboxBackground,			// This is the GUID of PixelComponents throwing error
				cuT( "TopImage" ),				// This is generally displayed as the title
				ERROR_UNINITIALISED.c_str(),	// This is the description
				0,								// This is the context in the help file
				nullptr );
		}

		return hr;
	}

	STDMETHODIMP CSkyboxBackground::put_BottomImage( BSTR filePath )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			auto path = castor::Path{ fromBstr( filePath ) };
			m_internal->loadBottomImage( path.getPath(), path.getFileName( true ) );
		}
		else
		{
			hr = CComError::dispatchError(
				E_FAIL,							// This represents the error
				IID_ISkyboxBackground,			// This is the GUID of PixelComponents throwing error
				cuT( "BottomImage" ),			// This is generally displayed as the title
				ERROR_UNINITIALISED.c_str(),	// This is the description
				0,								// This is the context in the help file
				nullptr );
		}

		return hr;
	}

	STDMETHODIMP CSkyboxBackground::put_FrontImage( BSTR filePath )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			auto path = castor::Path{ fromBstr( filePath ) };
			m_internal->loadFrontImage( path.getPath(), path.getFileName( true ) );
		}
		else
		{
			hr = CComError::dispatchError(
				E_FAIL,							// This represents the error
				IID_ISkyboxBackground,			// This is the GUID of PixelComponents throwing error
				cuT( "FrontImage" ),			// This is generally displayed as the title
				ERROR_UNINITIALISED.c_str(),	// This is the description
				0,								// This is the context in the help file
				nullptr );
		}

		return hr;
	}

	STDMETHODIMP CSkyboxBackground::put_BackImage( BSTR filePath )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			auto path = castor::Path{ fromBstr( filePath ) };
			m_internal->loadBackImage( path.getPath(), path.getFileName( true ) );
		}
		else
		{
			hr = CComError::dispatchError(
				E_FAIL,							// This represents the error
				IID_ISkyboxBackground,			// This is the GUID of PixelComponents throwing error
				cuT( "BackImage" ),				// This is generally displayed as the title
				ERROR_UNINITIALISED.c_str(),	// This is the description
				0,								// This is the context in the help file
				nullptr );
		}

		return hr;
	}

	STDMETHODIMP CSkyboxBackground::put_CrossImage( BSTR filePath )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			auto path = castor::Path{ fromBstr( filePath ) };
			m_internal->loadCrossTexture( path.getPath(), path.getFileName( true ) );
		}
		else
		{
			hr = CComError::dispatchError(
				E_FAIL,							// This represents the error
				IID_ISkyboxBackground,			// This is the GUID of PixelComponents throwing error
				cuT( "CrossImage" ),			// This is generally displayed as the title
				ERROR_UNINITIALISED.c_str(),	// This is the description
				0,								// This is the context in the help file
				nullptr );
		}

		return hr;
	}

	STDMETHODIMP CSkyboxBackground::LoadEquirectangularImage( BSTR filePath, unsigned int size )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			auto path = castor::Path{ fromBstr( filePath ) };
			m_internal->loadEquiTexture( path.getPath(), path.getFileName( true ), size );
		}
		else
		{
			hr = CComError::dispatchError(
				E_FAIL,								// This represents the error
				IID_ISkyboxBackground,				// This is the GUID of PixelComponents throwing error
				cuT( "LoadEquirectangularImage" ),	// This is generally displayed as the title
				ERROR_UNINITIALISED.c_str(),		// This is the description
				0,									// This is the context in the help file
				nullptr );
		}

		return hr;
	}
}
