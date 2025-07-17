#include "CCastor3D/Castor3D.h"
#include "CCastor3D/Castor3DCommon.h"

#include <Castor3D/Scene/Camera.hpp>
#include <Castor3D/Scene/Scene.hpp>

#ifdef __cplusplus
extern "C"
{
#endif

	static const C3DString ERROR_UNINITIALISED_CAMERA = cuT( "The camera must be initialised" );
	static const C3DString ERROR_UNINITIALISED_CAMNODE = cuT( "The node must be initialised" );

	C3D_CAPIMETHODIMP c3dCamera_delete( C3DCamera * object )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->getInternal() )
			cc3d::reportWarning( ERROR_UNINITIALISED_CAMERA );

		object->releaseInternal();
		delete object;

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dCamera_getName( C3DCamera const * object, C3DString * result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_CAMERA );

		return cc3d::copyString( object->getInternal()->getName(), result );
	}

	C3D_CAPIMETHODIMP c3dCamera_getScene( C3DCamera const * object, C3DScene ** result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_CAMERA );

		try
		{
			auto res = object->getInternal()->getScene();
			C3D_SafeAlloc( *result, C3DScene );
			( *result )->setInternal( res );
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dCamera_getNode( C3DCamera const * object, C3DSceneNode ** result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_CAMERA );

		try
		{
			auto res = object->getInternal()->getParent();
			C3D_SafeAlloc( *result, C3DSceneNode );
			( *result )->setInternal( res );
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dCamera_attachTo( C3DCamera * object, C3DSceneNode * val )
	{
		if ( !object || !val )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_CAMERA );
		if ( !val->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_CAMNODE );

		object->getInternal()->attachTo( *val->getInternal() );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dCamera_detach( C3DCamera * object )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_CAMERA );

		object->getInternal()->detach();

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dCamera_getViewportType( C3DCamera const * object, C3D_VIEWPORT_TYPE * result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_CAMERA );

		*result = C3D_VIEWPORT_TYPE( object->getInternal()->getViewportType() );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dCamera_setViewportType( C3DCamera * object, C3D_VIEWPORT_TYPE val )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_CAMERA );

		object->getInternal()->setViewportType( c3d::ViewportType( val ) );

		return C3D_OK;
	}

#ifdef __cplusplus
}
#endif
