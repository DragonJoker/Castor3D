#include "CCastor3D/Castor3D.h"
#include "CCastor3D/Castor3DCommon.h"

#include <Castor3D/Scene/Light/Light.hpp>
#include <Castor3D/Scene/Scene.hpp>

#ifdef __cplusplus
extern "C"
{
#endif

	static const C3DString ERROR_UNINITIALISED_LIGHT = cuT( "The light must be initialised" );
	static const C3DString ERROR_UNINITIALISED_LIGHTNODE = cuT( "The node must be initialised" );
	static const C3DString ERROR_UNINITIALISED_LIGHTSCENE = cuT( "The scene must be initialised" );
	static const C3DString ERROR_WRONG_LIGHT_TYPE = cuT( "The light is not of the expected type" );

	C3D_CAPIMETHODIMP c3dLight_create( C3DScene * object, C3DString name, C3DSceneNode const * parent, C3D_LIGHT_TYPE type, C3DLight ** result )
	{
		if ( !object || !name || !parent || !result )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_LIGHTSCENE );

		try
		{
			auto res = object->getInternal()->createLight( castor::makeString( name )
				, *object->getInternal()
				, *parent->getInternal()
				, object->getInternal()->getLightsFactory()
				, castor3d::LightType( type ) );
			C3D_SafeAlloc( *result, C3DLight );
			( *result )->setInternal( castor::move( res ) );
		}
		C3D_CatchCommonExceptions()

			return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dLight_delete( C3DLight * object )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->getInternal() )
			cc3d::reportWarning( ERROR_UNINITIALISED_LIGHT );

		object->releaseInternal();
		delete object;

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dLight_getName( C3DLight const * object, C3DString * result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_LIGHT );

		return cc3d::copyString( object->getInternal()->getName(), result );
	}

	C3D_CAPIMETHODIMP c3dLight_getScene( C3DLight const * object, C3DScene ** result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_LIGHT );

		try
		{
			auto res = object->getInternal()->getScene();
			C3D_SafeAlloc( *result, C3DScene );
			( *result )->setInternal( res );
		}
		C3D_CatchCommonExceptions()

			return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dLight_getNode( C3DLight const * object, C3DSceneNode ** result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_LIGHT );

		try
		{
			auto res = object->getInternal()->getParent();
			C3D_SafeAlloc( *result, C3DSceneNode );
			( *result )->setInternal( res );
		}
		C3D_CatchCommonExceptions()

			return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dLight_attachTo( C3DLight * object, C3DSceneNode * val )
	{
		if ( !object || !val )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_LIGHT );
		if ( !val->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_LIGHTNODE );

		object->getInternal()->attachTo( *val->getInternal() );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dLight_detach( C3DLight * object )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_LIGHT );

		object->getInternal()->detach();

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dLight_getLightType( C3DLight const * object, C3D_LIGHT_TYPE * result )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_LIGHT );

		*result = C3D_LIGHT_TYPE( object->getInternal()->getLightType() );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dLight_getDirectionalLight( C3DLight const * object, C3DDirectionalLight ** result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_LIGHT );

		try
		{
			auto res = object->getInternal()->getDirectionalLight();
			if ( !res )
				return cc3d::reportError( C3D_FAILURE, ERROR_WRONG_LIGHT_TYPE );

			C3D_SafeAlloc( *result, C3DDirectionalLight );
			( *result )->internal = res;
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dLight_getPointLight( C3DLight const * object, C3DPointLight ** result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_LIGHT );

		try
		{
			auto res = object->getInternal()->getPointLight();
			if ( !res )
				return cc3d::reportError( C3D_FAILURE, ERROR_WRONG_LIGHT_TYPE );

			C3D_SafeAlloc( *result, C3DPointLight );
			( *result )->internal = res;
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dLight_getSpotLight( C3DLight const * object, C3DSpotLight ** result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_LIGHT );

		try
		{
			auto res = object->getInternal()->getSpotLight();
			if ( !res )
				return cc3d::reportError( C3D_FAILURE, ERROR_WRONG_LIGHT_TYPE );

			C3D_SafeAlloc( *result, C3DSpotLight );
			( *result )->internal = res;
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

#ifdef __cplusplus
}
#endif
