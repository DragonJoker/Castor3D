#include "ComTextureImage.hpp"
#include "ComLogger.hpp"

namespace CastorCom
{
	static const castor::String ERROR_UNINITIALISED = cuT( "The texture image must be initialised" );

	CTextureImage::CTextureImage()
	{
	}

	CTextureImage::~CTextureImage()
	{
	}

	STDMETHODIMP CTextureImage::Resize2D( /* [in] */ unsigned int w, /* [in] */ unsigned int h )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->resize( castor::Size{ w, h } );
			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError(
				E_FAIL,							// This represents the error
				IID_ITextureImage,				// This is the GUID of PixelComponents throwing error
				cuT( "Cleanup" ),				// This is generally displayed as the title
				ERROR_UNINITIALISED.c_str(),	// This is the description
				0,								// This is the context in the help file
				nullptr );
		}

		return hr;
	}

	STDMETHODIMP CTextureImage::Resize3D( /* [in] */ unsigned int w, /* [in] */ unsigned int h, /* [in] */ unsigned int d )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->resize( castor::Point3ui{ w, h, d } );
			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError(
				E_FAIL,							// This represents the error
				IID_ITextureImage,				// This is the GUID of PixelComponents throwing error
				cuT( "Cleanup" ),				// This is generally displayed as the title
				ERROR_UNINITIALISED.c_str(),	// This is the description
				0,								// This is the context in the help file
				nullptr );
		}

		return hr;
	}

	STDMETHODIMP CTextureImage::StaticSource( /* [in] */ IPixelBuffer * val )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->initialiseSource( static_cast< CPixelBuffer * >( val )->getInternal() );
			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError(
				E_FAIL,							// This represents the error
				IID_ITextureImage,				// This is the GUID of PixelComponents throwing error
				cuT( "Cleanup" ),				// This is generally displayed as the title
				ERROR_UNINITIALISED.c_str(),	// This is the description
				0,								// This is the context in the help file
				nullptr );
		}

		return hr;
	}

	STDMETHODIMP CTextureImage::Dynamic2DSource( /* [in] */ unsigned int w, /* [in] */ unsigned int h, /* [in] */ ePIXEL_FORMAT format )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->initialiseSource( castor::PxBufferBase::create( castor::Size{ w, h }, castor::PixelFormat( format ) ) );
			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError(
				E_FAIL,							// This represents the error
				IID_ITextureImage,				// This is the GUID of PixelComponents throwing error
				cuT( "Cleanup" ),				// This is generally displayed as the title
				ERROR_UNINITIALISED.c_str(),	// This is the description
				0,								// This is the context in the help file
				nullptr );
		}

		return hr;
	}

	STDMETHODIMP CTextureImage::Dynamic3DSource( /* [in] */ unsigned int w, /* [in] */ unsigned int h, /* [in] */ unsigned int d, /* [in] */ ePIXEL_FORMAT format )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->initialiseSource( castor::PxBufferBase::create( castor::Size{ w, h * d }, castor::PixelFormat( format ) ) );
			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError(
				E_FAIL,							// This represents the error
				IID_ITextureImage,				// This is the GUID of PixelComponents throwing error
				cuT( "Cleanup" ),				// This is generally displayed as the title
				ERROR_UNINITIALISED.c_str(),	// This is the description
				0,								// This is the context in the help file
				nullptr );
		}

		return hr;
	}
}
