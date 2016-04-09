#include "ComDepthStencilState.hpp"
#include "ComLogger.hpp"

namespace CastorCom
{
	static const Castor::String ERROR_UNINITIALISED_STATE = cuT( "The depth stencil state must be initialised" );

	CDepthStencilState::CDepthStencilState()
	{
	}

	CDepthStencilState::~CDepthStencilState()
	{
	}

	STDMETHODIMP CDepthStencilState::SetDepthRange( /* [in] */ double dnear, /* [in] */ double dfar )
	{
		HRESULT hr = E_POINTER;

		if ( m_state )
		{
			hr = S_OK;
			m_state->SetDepthRange( dnear, dfar );
		}
		else
		{
			hr = CComError::DispatchError(
					 E_FAIL,							// This represents the error
					 IID_IDepthStencilState,			// This is the GUID of component throwing error
					 cuT( "SetDepthRange" ),			// This is generally displayed as the title
					 ERROR_UNINITIALISED_STATE.c_str(),	// This is the description
					 0,									// This is the context in the help file
					 NULL );
		}

		return hr;
	}

	STDMETHODIMP CDepthStencilState::Initialise()
	{
		HRESULT hr = E_POINTER;

		if ( m_state )
		{
			hr = m_state->Initialise() ? S_OK : E_FAIL;
		}
		else
		{
			hr = CComError::DispatchError(
					 E_FAIL,							// This represents the error
					 IID_IDepthStencilState,			// This is the GUID of component throwing error
					 cuT( "Initialise" ),				// This is generally displayed as the title
					 ERROR_UNINITIALISED_STATE.c_str(),	// This is the description
					 0,									// This is the context in the help file
					 NULL );
		}

		return hr;
	}

	STDMETHODIMP CDepthStencilState::Cleanup()
	{
		HRESULT hr = E_POINTER;

		if ( m_state )
		{
			m_state->Cleanup();
			hr = S_OK;
		}
		else
		{
			hr = CComError::DispatchError(
					 E_FAIL,							// This represents the error
					 IID_IDepthStencilState,			// This is the GUID of component throwing error
					 cuT( "Cleanup" ),					// This is generally displayed as the title
					 ERROR_UNINITIALISED_STATE.c_str(),	// This is the description
					 0,									// This is the context in the help file
					 NULL );
		}

		return hr;
	}
}
