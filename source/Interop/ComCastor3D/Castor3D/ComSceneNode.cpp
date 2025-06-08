#include "ComCastor3D/Castor3D/ComSceneNode.hpp"

#include "ComCastor3D/Castor3D/ComScene.hpp"

namespace CastorCom
{
	STDMETHODIMP CSceneNode::AttachTo( /*[in]*/ ISceneNode * val )noexcept
	{
		if ( !val )
			return E_POINTER;
		if ( !m_internal )
			return dispatchUninitialised( _T( "AttachTo" ) );

		return convert( c3dSceneNode_attachTo( m_internal
			, static_cast< CSceneNode * >( val )->getInternal() ) );
	}

	STDMETHODIMP CSceneNode::Detach()noexcept
	{
		if ( !m_internal )
			return dispatchUninitialised( _T( "Detach" ) );

		return convert( c3dSceneNode_detach( m_internal ) );
	}

	STDMETHODIMP CSceneNode::Yaw( /*[in]*/ FLOAT val )noexcept
	{
		if ( !m_internal )
			return dispatchUninitialised( _T( "Yaw" ) );

		return convert( c3dSceneNode_yaw( m_internal, val ) );
	}

	STDMETHODIMP CSceneNode::Pitch( /*[in]*/ FLOAT val )noexcept
	{
		if ( !m_internal )
			return dispatchUninitialised( _T( "Yaw" ) );

		return convert( c3dSceneNode_pitch( m_internal, val ) );
	}

	STDMETHODIMP CSceneNode::Roll( /*[in]*/ FLOAT val )noexcept
	{
		if ( !m_internal )
			return dispatchUninitialised( _T( "Yaw" ) );

		return convert( c3dSceneNode_roll( m_internal, val ) );
	}

	STDMETHODIMP CSceneNode::Rotate( /*[in]*/ IQuaternion * val )noexcept
	{
		if ( !val )
			return E_POINTER;
		if ( !m_internal )
			return dispatchUninitialised( _T( "Rotate" ) );

		return convert( c3dSceneNode_rotate( m_internal
			, &static_cast< CQuaternion * >( val )->getInternal() ) );
	}

	STDMETHODIMP CSceneNode::Translate( /*[in]*/ IVector3D * val )noexcept
	{
		if ( !val )
			return E_POINTER;
		if ( !m_internal )
			return dispatchUninitialised( _T( "Translate" ) );

		return convert( c3dSceneNode_translate( m_internal
			, &static_cast< CVector3D * >( val )->getInternal() ) );
	}

	STDMETHODIMP CSceneNode::Scale( /*[in]*/ IVector3D * val )noexcept
	{
		if ( !val )
			return E_POINTER;
		if ( !m_internal )
			return dispatchUninitialised( _T( "Scale" ) );

		return convert( c3dSceneNode_scale( m_internal
			, &static_cast< CVector3D * >( val )->getInternal() ) );
	}
}
