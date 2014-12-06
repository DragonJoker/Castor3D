#include "ComCamera.hpp"
#include "ComSceneNode.hpp"

namespace CastorCom
{
	static const Castor::String ERROR_UNINITIALISED = cuT( "The camera must be initialised" );

	CCamera::CCamera()
	{
	}

	CCamera::~CCamera()
	{
	}

	STDMETHODIMP CCamera::AttachTo( /* [in] */ ISceneNode * val )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->AttachTo( static_cast< CSceneNode * >( val )->GetInternal().get() );
			hr = S_OK;
		}
		else
		{
			hr = CComError::DispatchError(
					 E_FAIL,						// This represents the error
					 IID_ICamera,					// This is the GUID of component throwing error
					 cuT( "AttachTo" ),				// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 NULL );
		}

		return hr;
	}

	STDMETHODIMP CCamera::Detach()
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->Detach();
			hr = S_OK;
		}
		else
		{
			hr = CComError::DispatchError(
					 E_FAIL,						// This represents the error
					 IID_ICamera,					// This is the GUID of component throwing error
					 cuT( "Detach" ),				// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 NULL );
		}

		return hr;
	}

	STDMETHODIMP CCamera::Resize( /* [in] */ unsigned int width, /* [in] */ unsigned int height )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->Resize( width, height );
			hr = S_OK;
		}
		else
		{
			hr = CComError::DispatchError(
					 E_FAIL,						// This represents the error
					 IID_ICamera,					// This is the GUID of component throwing error
					 cuT( "Resize" ),				// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 NULL );
		}

		return hr;
	}
}
