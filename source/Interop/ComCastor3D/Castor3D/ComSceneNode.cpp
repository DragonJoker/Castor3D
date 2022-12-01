#include "ComCastor3D/Castor3D/ComSceneNode.hpp"
#include "ComCastor3D/Castor3D/ComMovableObject.hpp"
#include "ComCastor3D/CastorUtils/ComAngle.hpp"
#include "ComCastor3D/CastorUtils/ComQuaternion.hpp"
#include "ComCastor3D/CastorUtils/ComVector3D.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Event/Frame/CpuFunctorEvent.hpp>

namespace CastorCom
{
	namespace node
	{
		static const tstring ERROR_UNINITIALISED = _T( "The scene node must be initialised" );
	}

	STDMETHODIMP CSceneNode::AttachObject( /* [in] */ IMovableObject * val )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->attachObject( *static_cast< CMovableObject * >( val )->getInternal() );
			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError( E_FAIL, IID_ISceneNode, _T( "attachObject" ), node::ERROR_UNINITIALISED.c_str(), 0, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CSceneNode::DetachObject( /* [in] */ IMovableObject * val )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->detachObject( *static_cast< CMovableObject * >( val )->getInternal() );
			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError( E_FAIL, IID_ISceneNode, _T( "detachObject" ), node::ERROR_UNINITIALISED.c_str(), 0, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CSceneNode::AttachTo( /* [in] */ ISceneNode * val )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->attachTo( *static_cast< CSceneNode * >( val )->getInternal() );
			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError( E_FAIL, IID_ISceneNode, _T( "attachTo" ), node::ERROR_UNINITIALISED.c_str(), 0, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CSceneNode::Detach()noexcept
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->detach();
			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError( E_FAIL, IID_ISceneNode, _T( "Detach" ), node::ERROR_UNINITIALISED.c_str(), 0, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CSceneNode::Yaw( /* [in] */ IAngle * val )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			auto value = static_cast< CAngle * >( val )->getInternal();
			m_internal->getScene()->getEngine()->postEvent( castor3d::makeCpuFunctorEvent( castor3d::EventType::ePostRender
				, [this, value]()
				{
					m_internal->yaw( value );
				} ) );
			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError( E_FAIL, IID_ISceneNode, _T( "Yaw" ), node::ERROR_UNINITIALISED.c_str(), 0, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CSceneNode::Pitch( /* [in] */ IAngle * val )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			auto value = static_cast< CAngle * >( val )->getInternal();
			m_internal->getScene()->getEngine()->postEvent( castor3d::makeCpuFunctorEvent( castor3d::EventType::ePostRender
				, [this, value]()
				{
					m_internal->pitch( value );
				} ) );
			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError( E_FAIL, IID_ISceneNode, _T( "Pitch" ), node::ERROR_UNINITIALISED.c_str(), 0, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CSceneNode::Roll( /* [in] */ IAngle * val )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			auto value = static_cast< CAngle * >( val )->getInternal();
			m_internal->getScene()->getEngine()->postEvent( castor3d::makeCpuFunctorEvent( castor3d::EventType::ePostRender
				, [this, value]()
				{
					m_internal->roll( value );
				} ) );
			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError( E_FAIL, IID_ISceneNode, _T( "Roll" ), node::ERROR_UNINITIALISED.c_str(), 0, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CSceneNode::Rotate( /* [in] */ IQuaternion * val )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			auto value = static_cast< CQuaternion * >( val )->getInternal();
			m_internal->getScene()->getEngine()->postEvent( castor3d::makeCpuFunctorEvent( castor3d::EventType::ePostRender
				, [this, value]()
				{
					m_internal->rotate( value );
				} ) );
			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError( E_FAIL, IID_ISceneNode, _T( "Rotate" ), node::ERROR_UNINITIALISED.c_str(), 0, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CSceneNode::Translate( /* [in] */ IVector3D * val )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			auto value = static_cast< CVector3D * >( val )->getInternal();
			m_internal->getScene()->getEngine()->postEvent( castor3d::makeCpuFunctorEvent( castor3d::EventType::ePostRender
				, [this, value]()
				{
					m_internal->translate( value );
				} ) );
			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError( E_FAIL, IID_ISceneNode, _T( "Translate" ), node::ERROR_UNINITIALISED.c_str(), 0, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CSceneNode::Scale( /* [in] */ IVector3D * val )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			auto value = static_cast< CVector3D * >( val )->getInternal();
			m_internal->getScene()->getEngine()->postEvent( castor3d::makeCpuFunctorEvent( castor3d::EventType::ePostRender
				, [this, value]()
				{
					m_internal->scale( value );
				} ) );
			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError( E_FAIL, IID_ISceneNode, _T( "Scale" ), node::ERROR_UNINITIALISED.c_str(), 0, nullptr );
		}

		return hr;
	}
}
