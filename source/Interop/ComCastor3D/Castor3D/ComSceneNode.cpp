#include "ComCastor3D/Castor3D/ComSceneNode.hpp"
#include "ComCastor3D/Castor3D/ComMovableObject.hpp"
#include "ComCastor3D/CastorUtils/ComAngle.hpp"
#include "ComCastor3D/CastorUtils/ComQuaternion.hpp"
#include "ComCastor3D/CastorUtils/ComVector3D.hpp"

namespace CastorCom
{
	static const tstring ERROR_UNINITIALISED = _T( "The scene node must be initialised" );

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
			m_internal->attachObject( *static_cast< CMovableObject * >( val )->getInternal() );
			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError( E_FAIL, IID_ISceneNode, _T( "attachObject" ), ERROR_UNINITIALISED.c_str(), 0, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CSceneNode::DetachObject( /* [in] */ IMovableObject * val )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->detachObject( *static_cast< CMovableObject * >( val )->getInternal() );
			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError( E_FAIL, IID_ISceneNode, _T( "detachObject" ), ERROR_UNINITIALISED.c_str(), 0, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CSceneNode::AttachTo( /* [in] */ ISceneNode * val )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->attachTo( static_cast< CSceneNode * >( val )->getInternal() );
			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError( E_FAIL, IID_ISceneNode, _T( "attachTo" ), ERROR_UNINITIALISED.c_str(), 0, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CSceneNode::Detach()
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->detach();
			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError( E_FAIL, IID_ISceneNode, _T( "Detach" ), ERROR_UNINITIALISED.c_str(), 0, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CSceneNode::Yaw( /* [in] */ IAngle * val )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->yaw( *static_cast< CAngle * >( val ) );
			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError( E_FAIL, IID_ISceneNode, _T( "Yaw" ), ERROR_UNINITIALISED.c_str(), 0, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CSceneNode::Pitch( /* [in] */ IAngle * val )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->pitch( *static_cast< CAngle * >( val ) );
			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError( E_FAIL, IID_ISceneNode, _T( "Pitch" ), ERROR_UNINITIALISED.c_str(), 0, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CSceneNode::Roll( /* [in] */ IAngle * val )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->roll( *static_cast< CAngle * >( val ) );
			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError( E_FAIL, IID_ISceneNode, _T( "Roll" ), ERROR_UNINITIALISED.c_str(), 0, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CSceneNode::Rotate( /* [in] */ IQuaternion * val )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->rotate( *static_cast< CQuaternion * >( val ) );
			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError( E_FAIL, IID_ISceneNode, _T( "Rotate" ), ERROR_UNINITIALISED.c_str(), 0, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CSceneNode::Translate( /* [in] */ IVector3D * val )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->translate( *static_cast< CVector3D * >( val ) );
			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError( E_FAIL, IID_ISceneNode, _T( "Translate" ), ERROR_UNINITIALISED.c_str(), 0, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CSceneNode::Scale( /* [in] */ IVector3D * val )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->scale( *static_cast< CVector3D * >( val ) );
			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError( E_FAIL, IID_ISceneNode, _T( "Scale" ), ERROR_UNINITIALISED.c_str(), 0, nullptr );
		}

		return hr;
	}
}
