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
}
