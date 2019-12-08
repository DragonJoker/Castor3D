#include "ComCastor3D/Castor3D/ComSampler.hpp"
#include "ComCastor3D/CastorUtils/ComLogger.hpp"

namespace CastorCom
{
	static const tstring ERROR_UNINITIALISED = _T( "The sampler must be initialised" );

	CSampler::CSampler()
	{
	}

	CSampler::~CSampler()
	{
	}

	STDMETHODIMP CSampler::Initialise()
	{
		HRESULT hr = E_POINTER;

		if ( m_sampler )
		{
			hr = m_sampler->initialise() ? S_OK : E_FAIL;
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

	STDMETHODIMP CSampler::Cleanup()
	{
		HRESULT hr = E_POINTER;

		if ( m_sampler )
		{
			m_sampler->cleanup();
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
