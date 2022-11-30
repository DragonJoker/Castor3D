#include "ComCastor3D/Castor3D/ComRenderTarget.hpp"

namespace CastorCom
{
	static const tstring ERROR_UNINITIALISED = _T( "The render target must be initialised" );

	STDMETHODIMP CRenderTarget::Initialise()noexcept
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			auto & device = *m_internal->getOwner()->getRenderDevice();
			auto queueData = device.graphicsData();
			m_internal->initialise( device, *queueData );
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

	STDMETHODIMP CRenderTarget::Cleanup()noexcept
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			auto & device = *m_internal->getOwner()->getRenderDevice();
			m_internal->cleanup( device );
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
