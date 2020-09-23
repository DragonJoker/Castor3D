#include "ComCastor3D/Castor3D/ComCamera.hpp"
#include "ComCastor3D/Castor3D/ComSceneNode.hpp"

namespace CastorCom
{
	static const tstring ERROR_UNINITIALISED = _T( "The camera must be initialised" );

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
			m_internal->attachTo( *static_cast< CSceneNode * >( val )->getInternal() );
			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError(
					 E_FAIL,						// This represents the error
					 IID_ICamera,					// This is the GUID of PixelComponents throwing error
					_T( "attachTo" ),				// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 nullptr );
		}

		return hr;
	}

	STDMETHODIMP CCamera::Detach()
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
					 IID_ICamera,					// This is the GUID of PixelComponents throwing error
					_T( "Detach" ),				// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 nullptr );
		}

		return hr;
	}

	STDMETHODIMP CCamera::Resize( /* [in] */ unsigned int width, /* [in] */ unsigned int height )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->resize( width, height );
			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError(
					 E_FAIL,						// This represents the error
					 IID_ICamera,					// This is the GUID of PixelComponents throwing error
					 _T( "Resize" ),				// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 nullptr );
		}

		return hr;
	}
}
