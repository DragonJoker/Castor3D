#include "ComCastor3D/Castor3D/ComLightGroup.hpp"
#include "ComCastor3D/Castor3D/ComSceneNode.hpp"

namespace CastorCom
{
	STDMETHODIMP CLightGroup::AddLight( /* [in] */ ISceneNode * val )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->addInstance( *static_cast< CSceneNode * >( val )->getInternal() );
			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError(
				E_FAIL, // This represents the error
				IID_ILightGroup, // This is the GUID of the component throwing error
				_T( "AttachTo" ), // This is generally displayed as the title
				_T( "No LightGroup initialised" ), // This is the description
				0, // This is the context in the help file
				nullptr );
		}

		return hr;
	}

	STDMETHODIMP CLightGroup::RemoveLight(/* [in] */ ISceneNode * val )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			//m_internal->detach();
			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError(
				E_FAIL, // This represents the error
				IID_ILightGroup, // This is the GUID of the component throwing error
				_T( "Detach" ), // This is generally displayed as the title
				_T( "No LightGroup initialised" ), // This is the description
				0, // This is the context in the help file
				nullptr );
		}

		return hr;
	}
}
