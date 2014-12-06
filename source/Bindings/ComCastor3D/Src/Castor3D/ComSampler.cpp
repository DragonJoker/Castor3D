#include "ComSampler.hpp"
#include "ComLogger.hpp"

namespace CastorCom
{
	static const Castor::String ERROR_UNINITIALISED = cuT( "The sampler must be initialised" );

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
			hr = m_sampler->Initialise() ? S_OK : E_FAIL;
		}
		else
		{
			hr = CComError::DispatchError(
					 E_FAIL,						// This represents the error
					 IID_ISampler,					// This is the GUID of component throwing error
					 cuT( "Initialise" ),			// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 NULL );
		}

		return hr;
	}

	STDMETHODIMP CSampler::Cleanup()
	{
		HRESULT hr = E_POINTER;

		if ( m_sampler )
		{
			m_sampler->Cleanup();
			hr = S_OK;
		}
		else
		{
			hr = CComError::DispatchError(
					 E_FAIL,						// This represents the error
					 IID_ISampler,					// This is the GUID of component throwing error
					 cuT( "Cleanup" ),				// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 NULL );
		}

		return hr;
	}
}
