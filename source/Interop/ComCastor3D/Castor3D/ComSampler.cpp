#include "ComCastor3D/Castor3D/ComSampler.hpp"
#include "ComCastor3D/CastorUtils/ComLogger.hpp"

namespace CastorCom
{
	static const tstring ERROR_UNINITIALISED = _T( "The sampler must be initialised" );
	static const tstring ERROR_INITIALISATION = _T( "Sampler initialisation failed" );

	STDMETHODIMP CSampler::Initialise()noexcept
	{
		HRESULT hr = E_POINTER;

		if ( getSampler() )
		{
			try
			{
				getSampler()->initialise( *getSampler()->getEngine()->getRenderDevice() );
				hr = S_OK;
			}
			catch ( std::exception & exc )
			{
				hr = CComError::dispatchError(
					E_FAIL,					// This represents the error
					IID_ISampler,			// This is the GUID of PixelComponents throwing error
					_T( "Initialise" ),		// This is generally displayed as the title
					toBstr( exc.what() ),	// This is the description
					0,						// This is the context in the help file
					nullptr );
			}
		}
		else
		{
			hr = CComError::dispatchError(
					 E_FAIL,						// This represents the error
					 IID_ISampler,					// This is the GUID of PixelComponents throwing error
					 _T( "Initialise" ),			// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 nullptr );
		}

		return hr;
	}

	STDMETHODIMP CSampler::Cleanup()noexcept
	{
		HRESULT hr = E_POINTER;

		if ( getSampler() )
		{
			getSampler()->cleanup();
			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError(
					 E_FAIL,						// This represents the error
					 IID_ISampler,					// This is the GUID of PixelComponents throwing error
					 _T( "Cleanup" ),				// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 nullptr );
		}

		return hr;
	}
}
