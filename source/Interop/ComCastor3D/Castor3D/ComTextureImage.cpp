#include "ComCastor3D/Castor3D/ComTextureImage.hpp"
#include "ComCastor3D/CastorUtils/ComLogger.hpp"

namespace CastorCom
{
	static const tstring ERROR_UNINITIALISED = _T( "The texture image must be initialised" );

	CTextureView::CTextureView()
	{
	}

	CTextureView::~CTextureView()
	{
	}

	STDMETHODIMP CTextureView::StaticSource( /* [in] */ IPixelBuffer * val )
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
				IID_ITextureView,				// This is the GUID of PixelComponents throwing error
				_T( "Cleanup" ),				// This is generally displayed as the title
				ERROR_UNINITIALISED.c_str(),	// This is the description
				0,								// This is the context in the help file
				nullptr );
		}

		return hr;
	}

	STDMETHODIMP CTextureView::Dynamic2DSource( /* [in] */ unsigned int w, /* [in] */ unsigned int h, /* [in] */ ePIXEL_FORMAT format )
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
				IID_ITextureView,				// This is the GUID of PixelComponents throwing error
				_T( "Cleanup" ),				// This is generally displayed as the title
				ERROR_UNINITIALISED.c_str(),	// This is the description
				0,								// This is the context in the help file
				nullptr );
		}

		return hr;
	}

	STDMETHODIMP CTextureView::Dynamic3DSource( /* [in] */ unsigned int w, /* [in] */ unsigned int h, /* [in] */ unsigned int d, /* [in] */ ePIXEL_FORMAT format )
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
				IID_ITextureView,				// This is the GUID of PixelComponents throwing error
				_T( "Cleanup" ),				// This is generally displayed as the title
				ERROR_UNINITIALISED.c_str(),	// This is the description
				0,								// This is the context in the help file
				nullptr );
		}

		return hr;
	}
}
