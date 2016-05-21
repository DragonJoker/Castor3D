#include "ComTextureImage.hpp"
#include "ComLogger.hpp"

namespace CastorCom
{
	static const Castor::String ERROR_UNINITIALISED = cuT( "The texture image must be initialised" );

	CTextureImage::CTextureImage()
	{
	}

	CTextureImage::~CTextureImage()
	{
	}

	STDMETHODIMP CTextureImage::Initialise( /* [in] */ eTEXTURE_TYPE p_type, /* [in] */ unsigned int p_cpuAccess, /* [in] */ unsigned int p_gpuAccess )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			hr = m_internal->Initialise( Castor3D::eTEXTURE_TYPE( p_type ), uint8_t( p_cpuAccess ), uint8_t( p_gpuAccess ) ) ? S_OK : E_FAIL;
		}
		else
		{
			hr = CComError::DispatchError(
				E_FAIL,							// This represents the error
				IID_ITextureImage,				// This is the GUID of component throwing error
				cuT( "Initialise" ),			// This is generally displayed as the title
				ERROR_UNINITIALISED.c_str(),	// This is the description
				0,								// This is the context in the help file
				NULL );
		}

		return hr;
	}

	STDMETHODIMP CTextureImage::Cleanup()
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->Cleanup();
			hr = S_OK;
		}
		else
		{
			hr = CComError::DispatchError(
				E_FAIL,							// This represents the error
				IID_ITextureImage,				// This is the GUID of component throwing error
				cuT( "Cleanup" ),				// This is generally displayed as the title
				ERROR_UNINITIALISED.c_str(),	// This is the description
				0,								// This is the context in the help file
				NULL );
		}

		return hr;
	}

	STDMETHODIMP CTextureImage::Resize2D( /* [in] */ unsigned int w, /* [in] */ unsigned int h )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->Resize( Castor::Size{ w, h } );
			hr = S_OK;
		}
		else
		{
			hr = CComError::DispatchError(
				E_FAIL,							// This represents the error
				IID_ITextureImage,				// This is the GUID of component throwing error
				cuT( "Cleanup" ),				// This is generally displayed as the title
				ERROR_UNINITIALISED.c_str(),	// This is the description
				0,								// This is the context in the help file
				NULL );
		}

		return hr;
	}

	STDMETHODIMP CTextureImage::Resize3D( /* [in] */ unsigned int w, /* [in] */ unsigned int h, /* [in] */ unsigned int d )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->Resize( Castor::Point3ui{ w, h, d } );
			hr = S_OK;
		}
		else
		{
			hr = CComError::DispatchError(
				E_FAIL,							// This represents the error
				IID_ITextureImage,				// This is the GUID of component throwing error
				cuT( "Cleanup" ),				// This is generally displayed as the title
				ERROR_UNINITIALISED.c_str(),	// This is the description
				0,								// This is the context in the help file
				NULL );
		}

		return hr;
	}

	STDMETHODIMP CTextureImage::Static2DSource( /* [in] */ IPixelBuffer * val )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->SetSource( static_cast< CPixelBuffer * >( val )->GetInternal() );
			hr = S_OK;
		}
		else
		{
			hr = CComError::DispatchError(
				E_FAIL,							// This represents the error
				IID_ITextureImage,				// This is the GUID of component throwing error
				cuT( "Cleanup" ),				// This is generally displayed as the title
				ERROR_UNINITIALISED.c_str(),	// This is the description
				0,								// This is the context in the help file
				NULL );
		}

		return hr;
	}

	STDMETHODIMP CTextureImage::Static3DSource( /* [in] */ unsigned int w, /* [in] */ unsigned int h, /* [in] */ unsigned int d, /* [in] */ IPixelBuffer * val )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->SetSource( Castor::Point3ui{ w, h, d }, static_cast< CPixelBuffer * >( val )->GetInternal() );
			hr = S_OK;
		}
		else
		{
			hr = CComError::DispatchError(
				E_FAIL,							// This represents the error
				IID_ITextureImage,				// This is the GUID of component throwing error
				cuT( "Cleanup" ),				// This is generally displayed as the title
				ERROR_UNINITIALISED.c_str(),	// This is the description
				0,								// This is the context in the help file
				NULL );
		}

		return hr;
	}

	STDMETHODIMP CTextureImage::Dynamic2DSource( /* [in] */ unsigned int w, /* [in] */ unsigned int h, /* [in] */ ePIXEL_FORMAT format )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->SetSource( Castor::Size{ w, h }, Castor::ePIXEL_FORMAT( format ) );
			hr = S_OK;
		}
		else
		{
			hr = CComError::DispatchError(
				E_FAIL,							// This represents the error
				IID_ITextureImage,				// This is the GUID of component throwing error
				cuT( "Cleanup" ),				// This is generally displayed as the title
				ERROR_UNINITIALISED.c_str(),	// This is the description
				0,								// This is the context in the help file
				NULL );
		}

		return hr;
	}

	STDMETHODIMP CTextureImage::Dynamic3DSource( /* [in] */ unsigned int w, /* [in] */ unsigned int h, /* [in] */ unsigned int d, /* [in] */ ePIXEL_FORMAT format )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->SetSource( Castor::Point3ui{ w, h, d }, Castor::ePIXEL_FORMAT( format ) );
			hr = S_OK;
		}
		else
		{
			hr = CComError::DispatchError(
				E_FAIL,							// This represents the error
				IID_ITextureImage,				// This is the GUID of component throwing error
				cuT( "Cleanup" ),				// This is generally displayed as the title
				ERROR_UNINITIALISED.c_str(),	// This is the description
				0,								// This is the context in the help file
				NULL );
		}

		return hr;
	}
}
