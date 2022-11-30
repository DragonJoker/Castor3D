#include "ComCastor3D/Castor3D/ComLight.hpp"
#include "ComCastor3D/Castor3D/ComSceneNode.hpp"

namespace CastorCom
{
	namespace light
	{
		static const tstring ERROR_UNINITIALISED = _T( "The movable object must be initialised" );
	}

	STDMETHODIMP CLight::AttachTo( /* [in] */ ISceneNode * val )noexcept
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
					 IID_ILight,					// This is the GUID of PixelComponents throwing error
					 _T( "AttachTo" ),				// This is generally displayed as the title
					 light::ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 nullptr );
		}

		return hr;
	}

	STDMETHODIMP CLight::Detach()noexcept
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
					 IID_ILight,					// This is the GUID of PixelComponents throwing error
					 _T( "Detach" ),				// This is generally displayed as the title
					 light::ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 nullptr );
		}

		return hr;
	}
}
