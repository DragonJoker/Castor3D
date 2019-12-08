#include "ComCastor3D/Castor3D/ComRenderTarget.hpp"

namespace CastorCom
{
	static const tstring ERROR_UNINITIALISED = _T( "The render target must be initialised" );

	CRenderTarget::CRenderTarget()
	{
	}

	CRenderTarget::~CRenderTarget()
	{
	}

	STDMETHODIMP CRenderTarget::Initialise()
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->initialise();
			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError(
					 E_FAIL,						// This represents the error
					 IID_IRenderTarget,				// This is the GUID of PixelComponents throwing error
					 _T( "File" ),					// This is generally displayed as the title
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
					 _T( "File" ),					// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 nullptr );
		}

		return hr;
	}
}
