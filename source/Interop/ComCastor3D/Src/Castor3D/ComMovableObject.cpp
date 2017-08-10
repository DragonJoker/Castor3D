#include "ComMovableObject.hpp"
#include "ComSceneNode.hpp"

namespace CastorCom
{
	static const castor::String ERROR_UNINITIALISED = cuT( "The movable object must be initialised" );

	CMovableObject::CMovableObject()
	{
	}

	CMovableObject::~CMovableObject()
	{
	}

	STDMETHODIMP CMovableObject::attachTo( /* [in] */ ISceneNode * val )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->attachTo( static_cast< CSceneNode * >( val )->getInternal() );
			hr = S_OK;
		}
		else
		{
			hr = CComError::DispatchError(
					 E_FAIL,						// This represents the error
					 IID_IMovableObject,			// This is the GUID of PixelComponents throwing error
					 cuT( "attachTo" ),				// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 NULL );
		}

		return hr;
	}

	STDMETHODIMP CMovableObject::detach()
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->detach();
			hr = S_OK;
		}
		else
		{
			hr = CComError::DispatchError(
					 E_FAIL,						// This represents the error
					 IID_IMovableObject,			// This is the GUID of PixelComponents throwing error
					 cuT( "Detach" ),				// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 NULL );
		}

		return hr;
	}
}
