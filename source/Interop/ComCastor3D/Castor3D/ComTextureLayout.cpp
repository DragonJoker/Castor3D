#include "ComCastor3D/Castor3D/ComTextureLayout.hpp"
#include "ComCastor3D/CastorUtils/ComLogger.hpp"

namespace CastorCom
{
	static const tstring ERROR_UNINITIALISED = _T( "The texture layout must be initialised" );

	CTextureLayout::CTextureLayout()
	{
	}

	CTextureLayout::~CTextureLayout()
	{
	}

	STDMETHODIMP CTextureLayout::Initialise()
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			hr = m_internal->initialise() ? S_OK : E_FAIL;
		}
		else
		{
			hr = CComError::dispatchError(
				E_FAIL,							// This represents the error
				IID_ITextureLayout,				// This is the GUID of PixelComponents throwing error
				_T( "Initialise" ),			// This is generally displayed as the title
				ERROR_UNINITIALISED.c_str(),	// This is the description
				0,								// This is the context in the help file
				nullptr );
		}

		return hr;
	}

	STDMETHODIMP CTextureLayout::Cleanup()
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->cleanup();
			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError(
				E_FAIL,							// This represents the error
				IID_ITextureLayout,				// This is the GUID of PixelComponents throwing error
				_T( "Cleanup" ),				// This is generally displayed as the title
				ERROR_UNINITIALISED.c_str(),	// This is the description
				0,								// This is the context in the help file
				nullptr );
		}

		return hr;
	}

	STDMETHODIMP CTextureLayout::InitFromFile( /* [in] */ BSTR path )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			castor::Path filePath{ fromBstr( path ) };
			m_internal->setSource( filePath.getPath()
				, filePath.getFileName( true ) );

			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError(
				E_FAIL,							// This represents the error
				IID_ITextureLayout,				// This is the GUID of PixelComponents throwing error
				_T( "InitFromFile" ),			// This is generally displayed as the title
				ERROR_UNINITIALISED.c_str(),	// This is the description
				0,								// This is the context in the help file
				nullptr );
		}

		return hr;
	}

	STDMETHODIMP CTextureLayout::InitFromBuffer( /* [in] */ IPixelBuffer * val )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->setSource( static_cast< CPixelBuffer * >( val )->getInternal(), true );
			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError(
				E_FAIL,							// This represents the error
				IID_ITextureLayout,				// This is the GUID of PixelComponents throwing error
				_T( "InitFromBuffer" ),			// This is generally displayed as the title
				ERROR_UNINITIALISED.c_str(),	// This is the description
				0,								// This is the context in the help file
				nullptr );
		}

		return hr;
	}

	STDMETHODIMP CTextureLayout::Init2D( /* [in] */ unsigned int w, /* [in] */ unsigned int h, /* [in] */ ePIXEL_FORMAT format )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->setSource( castor::PxBufferBase::create( castor::Size{ w, h }, castor::PixelFormat( format ) ) );
			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError(
				E_FAIL,							// This represents the error
				IID_ITextureLayout,				// This is the GUID of PixelComponents throwing error
				_T( "Init2D" ),					// This is generally displayed as the title
				ERROR_UNINITIALISED.c_str(),	// This is the description
				0,								// This is the context in the help file
				nullptr );
		}

		return hr;
	}

	STDMETHODIMP CTextureLayout::Init3D( /* [in] */ unsigned int w, /* [in] */ unsigned int h, /* [in] */ unsigned int d, /* [in] */ ePIXEL_FORMAT format )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->setSource( castor::PxBufferBase::create( castor::Size{ w, h * d }, castor::PixelFormat( format ) ) );
			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError(
				E_FAIL,							// This represents the error
				IID_ITextureLayout,				// This is the GUID of PixelComponents throwing error
				_T( "Init3D" ),					// This is generally displayed as the title
				ERROR_UNINITIALISED.c_str(),	// This is the description
				0,								// This is the context in the help file
				nullptr );
		}

		return hr;
	}
}
