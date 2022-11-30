#include "ComCastor3D/Castor3D/ComMovableObject.hpp"
#include "ComCastor3D/Castor3D/ComSceneNode.hpp"

namespace CastorCom
{
	namespace moveble
	{
		static const tstring ERROR_UNINITIALISED = _T( "The movable object must be initialised" );
	}

	STDMETHODIMP CMovableObject::AttachTo( /* [in] */ ISceneNode * val )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->attachTo( *static_cast< CSceneNode * >( val )->getInternal() );
			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError(
					 E_FAIL,						// This represents the error
					 IID_IMovableObject,			// This is the GUID of PixelComponents throwing error
					 _T( "AttachTo" ),				// This is generally displayed as the title
					 moveble::ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 nullptr );
		}

		return hr;
	}

	STDMETHODIMP CMovableObject::Detach()noexcept
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->detach();
			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError(
					 E_FAIL,						// This represents the error
					 IID_IMovableObject,			// This is the GUID of PixelComponents throwing error
					 _T( "Detach" ),				// This is generally displayed as the title
					 moveble::ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 nullptr );
		}

		return hr;
	}
}
