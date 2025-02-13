#include "CCastor3D/Castor3D.h"
#include "CCastor3D/Castor3DCommon.h"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Event/Frame/CpuFunctorEvent.hpp>
#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Scene/SceneNode.hpp>

#ifdef __cplusplus
extern "C"
{
#endif

	static const C3DString ERROR_UNINITIALISED_NODE = cuT( "The scene node must be initialised" );
	static const C3DString ERROR_UNINITIALISED_NODESCENE = cuT( "The scene must be initialised" );

	C3D_CAPIMETHODIMP c3dSceneNode_create( C3DScene * object, C3DString name, C3DSceneNode const * parent, C3DSceneNode ** result )
	{
		if ( !object || !name || !result )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_NODESCENE );

		try
		{
			if ( auto res = object->getInternal()->createSceneNode( castor::makeString( name ), *object->getInternal() ) )
			{
				if ( parent )
				{
					res->attachTo( *parent->getInternal() );
				}
				else
				{
					res->attachTo( *object->getInternal()->getObjectRootNode() );
				}

				C3D_SafeAlloc( *result, C3DSceneNode );
				( *result )->setInternal( castor::move( res ) );
			}
		}
		C3D_CatchCommonExceptions()

			return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dSceneNode_delete( C3DSceneNode * object )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->getInternal() )
			cc3d::reportWarning( ERROR_UNINITIALISED_NODE );

		object->releaseInternal();
		delete object;

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dSceneNode_getPosition( C3DSceneNode const * object, C3DVec3 * result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_NODE );

		*result = cc3d::convert( object->getInternal()->getPosition() );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dSceneNode_setPosition( C3DSceneNode * object, C3DVec3 const * val )
	{
		if ( !object || !val )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_NODE );

		object->getInternal()->setPosition( cc3d::convert( *val ) );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dSceneNode_getOrientation( C3DSceneNode const * object, C3DQuat * result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_NODE );

		*result = cc3d::convert( object->getInternal()->getOrientation() );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dSceneNode_setOrientation( C3DSceneNode * object, C3DQuat const * val )
	{
		if ( !object || !val )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_NODE );

		object->getInternal()->setOrientation( cc3d::convert( *val ) );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dSceneNode_getScaling( C3DSceneNode const * object, C3DVec3 * result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_NODE );

		*result = cc3d::convert( object->getInternal()->getScale() );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dSceneNode_setScaling( C3DSceneNode * object, C3DVec3 const * val )
	{
		if ( !object || !val )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_NODE );

		object->getInternal()->setScale( cc3d::convert( *val ) );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dSceneNode_getParent( C3DSceneNode const * object, C3DSceneNode ** result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_NODE );

		try
		{
			if ( auto res = object->getInternal()->getParent() )
			{
				C3D_SafeAlloc( *result, C3DSceneNode );
				( *result )->setInternal( res );
			}
			else
			{
				// here a null result is valid.
				*result = nullptr;
			}
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dSceneNode_attachTo( C3DSceneNode * object, C3DSceneNode const * val )
	{
		if ( !object || !val )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_NODE );
		if ( !val->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_NODE );

		object->getInternal()->attachTo( *val->getInternal() );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dSceneNode_detach( C3DSceneNode * object )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_NODE );

		object->getInternal()->detach( false );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dSceneNode_yaw( C3DSceneNode * object, float val )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_NODE );

		object->getInternal()->getScene()->getEngine()->postEvent( castor3d::makeCpuFunctorEvent( castor3d::CpuEventType::ePostCpuStep
			, [object, val]()
			{
				object->getInternal()->yaw( castor::Angle::fromRadians( val ) );
			} ) );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dSceneNode_pitch( C3DSceneNode * object, float val )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_NODE );

		object->getInternal()->getScene()->getEngine()->postEvent( castor3d::makeCpuFunctorEvent( castor3d::CpuEventType::ePostCpuStep
			, [object, val]()
			{
				object->getInternal()->pitch( castor::Angle::fromRadians( val ) );
			} ) );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dSceneNode_roll( C3DSceneNode * object, float val )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_NODE );

		object->getInternal()->getScene()->getEngine()->postEvent( castor3d::makeCpuFunctorEvent( castor3d::CpuEventType::ePostCpuStep
			, [object, val]()
			{
				object->getInternal()->roll( castor::Angle::fromRadians( val ) );
			} ) );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dSceneNode_rotate( C3DSceneNode * object, C3DQuat const * val )
	{
		if ( !object || !val )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_NODE );

		auto value = cc3d::convert( *val );
		object->getInternal()->getScene()->getEngine()->postEvent( castor3d::makeCpuFunctorEvent( castor3d::CpuEventType::ePostCpuStep
			, [object, value]()
			{
				object->getInternal()->rotate( value );
			} ) );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dSceneNode_translate( C3DSceneNode * object, C3DVec3 const * val )
	{
		if ( !object || !val )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_NODE );

		auto value = cc3d::convert( *val );
		object->getInternal()->getScene()->getEngine()->postEvent( castor3d::makeCpuFunctorEvent( castor3d::CpuEventType::ePostCpuStep
			, [object, value]()
			{
				object->getInternal()->translate( value );
			} ) );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dSceneNode_scale( C3DSceneNode * object, C3DVec3 const * val )
	{
		if ( !object || !val )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_NODE );

		auto value = cc3d::convert( *val );
		object->getInternal()->getScene()->getEngine()->postEvent( castor3d::makeCpuFunctorEvent( castor3d::CpuEventType::ePostCpuStep
			, [object, value]()
			{
				object->getInternal()->scale( value );
			} ) );

		return C3D_OK;
	}

#ifdef __cplusplus
}
#endif
