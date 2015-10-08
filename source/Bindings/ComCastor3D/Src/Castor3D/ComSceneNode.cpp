#include "ComSceneNode.hpp"
#include "ComMovableObject.hpp"
#include "ComAngle.hpp"
#include "ComQuaternion.hpp"
#include "ComVector3D.hpp"

namespace CastorCom
{
	static const Castor::String ERROR_UNINITIALISED = cuT( "The scene node must be initialised" );

	CSceneNode::CSceneNode()
	{
	}

	CSceneNode::~CSceneNode()
	{
	}

	STDMETHODIMP CSceneNode::AttachObject( /* [in] */ IMovableObject * val )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->AttachObject( static_cast< CMovableObject * >( val )->GetInternal() );
			hr = S_OK;
		}
		else
		{
			hr = CComError::DispatchError( E_FAIL, IID_ISceneNode, cuT( "AttachObject" ), ERROR_UNINITIALISED.c_str(), 0, NULL );
		}

		return hr;
	}

	STDMETHODIMP CSceneNode::DetachObject( /* [in] */ IMovableObject * val )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->DetachObject( static_cast< CMovableObject * >( val )->GetInternal() );
			hr = S_OK;
		}
		else
		{
			hr = CComError::DispatchError( E_FAIL, IID_ISceneNode, cuT( "DetachObject" ), ERROR_UNINITIALISED.c_str(), 0, NULL );
		}

		return hr;
	}

	STDMETHODIMP CSceneNode::AttachTo( /* [in] */ ISceneNode * val )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->AttachTo( static_cast< CSceneNode * >( val )->GetInternal() );
			hr = S_OK;
		}
		else
		{
			hr = CComError::DispatchError( E_FAIL, IID_ISceneNode, cuT( "AttachTo" ), ERROR_UNINITIALISED.c_str(), 0, NULL );
		}

		return hr;
	}

	STDMETHODIMP CSceneNode::Detach()
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->Detach();
			hr = S_OK;
		}
		else
		{
			hr = CComError::DispatchError( E_FAIL, IID_ISceneNode, cuT( "Detach" ), ERROR_UNINITIALISED.c_str(), 0, NULL );
		}

		return hr;
	}

	STDMETHODIMP CSceneNode::Yaw( /* [in] */ IAngle * val )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->Yaw( *static_cast< CAngle * >( val ) );
			hr = S_OK;
		}
		else
		{
			hr = CComError::DispatchError( E_FAIL, IID_ISceneNode, cuT( "Yaw" ), ERROR_UNINITIALISED.c_str(), 0, NULL );
		}

		return hr;
	}

	STDMETHODIMP CSceneNode::Pitch( /* [in] */ IAngle * val )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->Pitch( *static_cast< CAngle * >( val ) );
			hr = S_OK;
		}
		else
		{
			hr = CComError::DispatchError( E_FAIL, IID_ISceneNode, cuT( "Pitch" ), ERROR_UNINITIALISED.c_str(), 0, NULL );
		}

		return hr;
	}

	STDMETHODIMP CSceneNode::Roll( /* [in] */ IAngle * val )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->Roll( *static_cast< CAngle * >( val ) );
			hr = S_OK;
		}
		else
		{
			hr = CComError::DispatchError( E_FAIL, IID_ISceneNode, cuT( "Roll" ), ERROR_UNINITIALISED.c_str(), 0, NULL );
		}

		return hr;
	}

	STDMETHODIMP CSceneNode::Rotate( /* [in] */ IQuaternion * val )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->Rotate( *static_cast< CQuaternion * >( val ) );
			hr = S_OK;
		}
		else
		{
			hr = CComError::DispatchError( E_FAIL, IID_ISceneNode, cuT( "Rotate" ), ERROR_UNINITIALISED.c_str(), 0, NULL );
		}

		return hr;
	}

	STDMETHODIMP CSceneNode::Translate( /* [in] */ IVector3D * val )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->Translate( *static_cast< CVector3D * >( val ) );
			hr = S_OK;
		}
		else
		{
			hr = CComError::DispatchError( E_FAIL, IID_ISceneNode, cuT( "Translate" ), ERROR_UNINITIALISED.c_str(), 0, NULL );
		}

		return hr;
	}

	STDMETHODIMP CSceneNode::Scale( /* [in] */ IVector3D * val )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->Scale( *static_cast< CVector3D * >( val ) );
			hr = S_OK;
		}
		else
		{
			hr = CComError::DispatchError( E_FAIL, IID_ISceneNode, cuT( "Scale" ), ERROR_UNINITIALISED.c_str(), 0, NULL );
		}

		return hr;
	}
}
