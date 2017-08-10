#include "ComBlendState.hpp"
#include "ComLogger.hpp"

namespace CastorCom
{
	static const castor::String ERROR_UNINITIALISED_STATE = cuT( "The blend state must be initialised" );

	CBlendState::CBlendState()
	{
	}

	CBlendState::~CBlendState()
	{
	}

	STDMETHODIMP CBlendState::setColourMask( /* [in] */ eWRITING_MASK red, /* [in] */ eWRITING_MASK green, /* [in] */ eWRITING_MASK blue, /* [in] */ eWRITING_MASK alpha )
	{
		HRESULT hr = E_POINTER;

		if ( m_state )
		{
			m_state->setColourMask( castor3d::WritingMask( red ), castor3d::WritingMask( green ), castor3d::WritingMask( blue ), castor3d::WritingMask( alpha ) );
			hr = S_OK;
		}
		else
		{
			hr = CComError::DispatchError( E_FAIL, IID_IBlendState, cuT( "setColourMask" ), ERROR_UNINITIALISED_STATE.c_str(), 0, NULL );
		}

		return hr;
	}
}
