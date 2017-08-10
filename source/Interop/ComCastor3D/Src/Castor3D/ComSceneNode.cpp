#include "ComSceneNode.hpp"
#include "ComMovableObject.hpp"
#include "ComAngle.hpp"
#include "ComQuaternion.hpp"
#include "ComVector3D.hpp"

namespace CastorCom
{
	static const castor::String ERROR_UNINITIALISED = cuT( "The scene node must be initialised" );

	CSceneNode::CSceneNode()
	{
	}

	CSceneNode::~CSceneNode()
	{
	}

	STDMETHODIMP CSceneNode::attachObject( /* [in] */ IMovableObject * val )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->attachObject( *static_cast< CMovableObject * >( val )->getInternal() );
			hr = S_OK;
		}
		else
		{
			hr = CComError::DispatchError( E_FAIL, IID_ISceneNode, cuT( "attachObject" ), ERROR_UNINITIALISED.c_str(), 0, NULL );
		}

		return hr;
	}

	STDMETHODIMP CSceneNode::detachObject( /* [in] */ IMovableObject * val )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->detachObject( *static_cast< CMovableObject * >( val )->getInternal() );
			hr = S_OK;
		}
		else
		{
			hr = CComError::DispatchError( E_FAIL, IID_ISceneNode, cuT( "detachObject" ), ERROR_UNINITIALISED.c_str(), 0, NULL );
		}

		return hr;
	}

	STDMETHODIMP CSceneNode::attachTo( /* [in] */ ISceneNode * val )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->attachTo( static_cast< CSceneNode * >( val )->getInternal() );
			hr = S_OK;
		}
		else
		{
			hr = CComError::DispatchError( E_FAIL, IID_ISceneNode, cuT( "attachTo" ), ERROR_UNINITIALISED.c_str(), 0, NULL );
		}

		return hr;
	}

	STDMETHODIMP CSceneNode::detach()
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->detach();
			hr = S_OK;
		}
		else
		{
			hr = CComError::DispatchError( E_FAIL, IID_ISceneNode, cuT( "Detach" ), ERROR_UNINITIALISED.c_str(), 0, NULL );
		}

		return hr;
	}

	STDMETHODIMP CSceneNode::yaw( /* [in] */ IAngle * val )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->yaw( *static_cast< CAngle * >( val ) );
			hr = S_OK;
		}
		else
		{
			hr = CComError::DispatchError( E_FAIL, IID_ISceneNode, cuT( "Yaw" ), ERROR_UNINITIALISED.c_str(), 0, NULL );
		}

		return hr;
	}

	STDMETHODIMP CSceneNode::pitch( /* [in] */ IAngle * val )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->pitch( *static_cast< CAngle * >( val ) );
			hr = S_OK;
		}
		else
		{
			hr = CComError::DispatchError( E_FAIL, IID_ISceneNode, cuT( "Pitch" ), ERROR_UNINITIALISED.c_str(), 0, NULL );
		}

		return hr;
	}

	STDMETHODIMP CSceneNode::roll( /* [in] */ IAngle * val )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->roll( *static_cast< CAngle * >( val ) );
			hr = S_OK;
		}
		else
		{
			hr = CComError::DispatchError( E_FAIL, IID_ISceneNode, cuT( "Roll" ), ERROR_UNINITIALISED.c_str(), 0, NULL );
		}

		return hr;
	}

	STDMETHODIMP CSceneNode::rotate( /* [in] */ IQuaternion * val )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->rotate( *static_cast< CQuaternion * >( val ) );
			hr = S_OK;
		}
		else
		{
			hr = CComError::DispatchError( E_FAIL, IID_ISceneNode, cuT( "Rotate" ), ERROR_UNINITIALISED.c_str(), 0, NULL );
		}

		return hr;
	}

	STDMETHODIMP CSceneNode::translate( /* [in] */ IVector3D * val )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->translate( *static_cast< CVector3D * >( val ) );
			hr = S_OK;
		}
		else
		{
			hr = CComError::DispatchError( E_FAIL, IID_ISceneNode, cuT( "Translate" ), ERROR_UNINITIALISED.c_str(), 0, NULL );
		}

		return hr;
	}

	STDMETHODIMP CSceneNode::scale( /* [in] */ IVector3D * val )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->scale( *static_cast< CVector3D * >( val ) );
			hr = S_OK;
		}
		else
		{
			hr = CComError::DispatchError( E_FAIL, IID_ISceneNode, cuT( "Scale" ), ERROR_UNINITIALISED.c_str(), 0, NULL );
		}

		return hr;
	}
}
