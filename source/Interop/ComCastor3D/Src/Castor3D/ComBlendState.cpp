#include "ComBlendState.hpp"
#include "ComLogger.hpp"

namespace CastorCom
{
	static const Castor::String ERROR_UNINITIALISED_STATE = cuT( "The blend state must be initialised" );

	CBlendState::CBlendState()
	{
	}

	CBlendState::~CBlendState()
	{
	}

	STDMETHODIMP CBlendState::SetColourMask( /* [in] */ eWRITING_MASK red, /* [in] */ eWRITING_MASK green, /* [in] */ eWRITING_MASK blue, /* [in] */ eWRITING_MASK alpha )
	{
		HRESULT hr = E_POINTER;

		if ( m_state )
		{
			m_state->SetColourMask( Castor3D::eWRITING_MASK( red ), Castor3D::eWRITING_MASK( green ), Castor3D::eWRITING_MASK( blue ), Castor3D::eWRITING_MASK( alpha ) );
			hr = S_OK;
		}
		else
		{
			hr = CComError::DispatchError( E_FAIL, IID_IBlendState, cuT( "SetColourMask" ), ERROR_UNINITIALISED_STATE.c_str(), 0, NULL );
		}

		return hr;
	}
}
