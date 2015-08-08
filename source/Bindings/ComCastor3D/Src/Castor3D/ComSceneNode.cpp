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
			hr = CComError::DispatchError(
					 E_FAIL,						// This represents the error
					 IID_ISceneNode,				// This is the GUID of component throwing error
					 cuT( "AttachObject" ),			// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 NULL );
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
			hr = CComError::DispatchError(
					 E_FAIL,						// This represents the error
					 IID_ISceneNode,				// This is the GUID of component throwing error
					 cuT( "DetachObject" ),			// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 NULL );
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
			hr = CComError::DispatchError(
					 E_FAIL,						// This represents the error
					 IID_ISceneNode,				// This is the GUID of component throwing error
					 cuT( "AttachTo" ),				// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 NULL );
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
			hr = CComError::DispatchError(
					 E_FAIL,						// This represents the error
					 IID_ISceneNode,				// This is the GUID of component throwing error
					 cuT( "Detach" ),				// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 NULL );
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
			hr = CComError::DispatchError(
					 E_FAIL,						// This represents the error
					 IID_ISceneNode,				// This is the GUID of component throwing error
					 cuT( "Yaw" ),					// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 NULL );
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
			hr = CComError::DispatchError(
					 E_FAIL,						// This represents the error
					 IID_ISceneNode,				// This is the GUID of component throwing error
					 cuT( "Pitch" ),				// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 NULL );
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
			hr = CComError::DispatchError(
					 E_FAIL,						// This represents the error
					 IID_ISceneNode,				// This is the GUID of component throwing error
					 cuT( "Roll" ),					// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 NULL );
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
			hr = CComError::DispatchError(
					 E_FAIL,						// This represents the error
					 IID_ISceneNode,				// This is the GUID of component throwing error
					 cuT( "Rotate" ),				// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 NULL );
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
			hr = CComError::DispatchError(
					 E_FAIL,						// This represents the error
					 IID_ISceneNode,				// This is the GUID of component throwing error
					 cuT( "Translate" ),			// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 NULL );
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
			hr = CComError::DispatchError(
					 E_FAIL,						// This represents the error
					 IID_ISceneNode,				// This is the GUID of component throwing error
					 cuT( "Scale" ),				// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 NULL );
		}

		return hr;
	}
}
