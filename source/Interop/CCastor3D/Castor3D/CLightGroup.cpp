#include "CCastor3D/Castor3D.h"
#include "CCastor3D/Castor3DCommon.h"

#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Scene/Light/LightGroup.hpp>

#ifdef __cplusplus
extern "C"
{
#endif

	static const C3DString ERROR_UNINITIALISED_LIGHTGRP = cuT( "The light group must be initialised" );
	static const C3DString ERROR_UNINITIALISED_LIGHTGRPNODE = cuT( "The node must be initialised" );
	static const C3DString ERROR_WRONG_LIGHTGRP_TYPE = cuT( "The light group is not of the expected type" );

	C3D_CAPIMETHODIMP c3dLightGroup_delete( C3DLightGroup * object )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->getInternal() )
			cc3d::reportWarning( ERROR_UNINITIALISED_LIGHTGRP );

		object->releaseInternal();
		delete object;

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dLightGroup_getName( C3DLightGroup const * object, C3DString * result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_LIGHTGRP );

		return cc3d::copyString( object->getInternal()->getName(), result );
	}

	C3D_CAPIMETHODIMP c3dLightGroup_getScene( C3DLightGroup const * object, C3DScene ** result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_LIGHTGRP );

		try
		{
			auto res = object->getInternal()->getScene();
			C3D_SafeAlloc( *result, C3DScene );
			( *result )->setInternal( res );
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dLightGroup_addLight( C3DLightGroup const * object, C3DSceneNode const * val )
	{
		if ( !object || !val )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_LIGHTGRP );
		if ( !val->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_LIGHTGRPNODE );

		object->getInternal()->addInstance( *val->getInternal() );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dLightGroup_removeLight( C3DLightGroup const * object, C3DSceneNode const * val )
	{
		if ( !object || !val )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_LIGHTGRP );
		if ( !val->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_LIGHTGRPNODE );

		//object->internal->removeInstance( *val->internal );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dLightGroup_getLightType( C3DLightGroup const * object, C3D_LIGHT_TYPE * result )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_LIGHTGRP );

		*result = C3D_LIGHT_TYPE( object->getInternal()->getLightType() );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dLightGroup_getDirectionalLight( C3DLightGroup const * object, C3DDirectionalLight ** result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_LIGHTGRP );

		try
		{
			auto res = object->getInternal()->getDirectionalLight();
			if ( !res )
				return cc3d::reportError( C3D_FAILURE, ERROR_WRONG_LIGHTGRP_TYPE );

			C3D_SafeAlloc( *result, C3DDirectionalLight );
			( *result )->internal = res;
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dLightGroup_getPointLight( C3DLightGroup const * object, C3DPointLight ** result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_LIGHTGRP );

		try
		{
			auto res = object->getInternal()->getPointLight();
			if ( !res )
				return cc3d::reportError( C3D_FAILURE, ERROR_WRONG_LIGHTGRP_TYPE );

			C3D_SafeAlloc( *result, C3DPointLight );
			( *result )->internal = res;
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dLightGroup_getSpotLight( C3DLightGroup const * object, C3DSpotLight ** result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_LIGHTGRP );

		try
		{
			auto res = object->getInternal()->getSpotLight();
			if ( !res )
				return cc3d::reportError( C3D_FAILURE, ERROR_WRONG_LIGHTGRP_TYPE );

			C3D_SafeAlloc( *result, C3DSpotLight );
			( *result )->internal = res;
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

#ifdef __cplusplus
}
#endif
