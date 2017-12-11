#include "ComRenderTarget.hpp"

namespace CastorCom
{
	static const castor::String ERROR_UNINITIALISED = cuT( "The render target must be initialised" );

	CRenderTarget::CRenderTarget()
	{
	}

	CRenderTarget::~CRenderTarget()
	{
	}

	STDMETHODIMP CRenderTarget::Initialise( /* [in] */ unsigned int index )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->initialise( index );
			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError(
					 E_FAIL,						// This represents the error
					 IID_IRenderTarget,				// This is the GUID of PixelComponents throwing error
					 cuT( "File" ),					// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 nullptr );
		}

		return hr;
	}

	STDMETHODIMP CRenderTarget::Cleanup()
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
					 E_FAIL,						// This represents the error
					 IID_IRenderTarget,				// This is the GUID of PixelComponents throwing error
					 cuT( "File" ),					// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 nullptr );
		}

		return hr;
	}
}
