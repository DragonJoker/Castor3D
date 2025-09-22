#include "CCastor3D/Castor3D.h"
#include "CCastor3D/Castor3DCommon.h"

#include <Castor3D/Scene/Geometry.hpp>
#include <Castor3D/Scene/Scene.hpp>

#ifdef __cplusplus
extern "C"
{
#endif

	static const C3DString ERROR_UNINITIALISED_GEOMETRY = cuT( "The geometry must be initialised" );
	static const C3DString ERROR_UNINITIALISED_GEOMNODE = cuT( "The node must be initialised" );
	static const C3DString ERROR_UNINITIALISED_GEOMMAT = cuT( "The material must be initialised" );
	static const C3DString ERROR_UNINITIALISED_GEOMMESH = cuT( "The mesh must be initialised" );
	static const C3DString ERROR_UNINITIALISED_GEOMSUBMESH = cuT( "The submesh must be initialised" );

	C3D_CAPIMETHODIMP c3dGeometry_delete( C3DGeometry * object )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->getInternal() )
			cc3d::reportWarning( ERROR_UNINITIALISED_GEOMETRY );

		object->releaseInternal();
		delete object;

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dGeometry_getName( C3DGeometry const * object, C3DString * result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_GEOMETRY );

		return cc3d::copyString( object->getInternal()->getName(), result );
	}

	C3D_CAPIMETHODIMP c3dGeometry_getScene( C3DGeometry const * object, C3DScene ** result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_GEOMETRY );

		try
		{
			auto res = object->getInternal()->getScene();
			C3D_SafeAlloc( *result, C3DScene );
			( *result )->setInternal( res );
		}
		C3D_CatchCommonExceptions()

			return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dGeometry_getNode( C3DGeometry const * object, C3DSceneNode ** result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_GEOMETRY );

		try
		{
			auto res = object->getInternal()->getParent();
			C3D_SafeAlloc( *result, C3DSceneNode );
			( *result )->setInternal( res );
		}
		C3D_CatchCommonExceptions()

			return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dGeometry_attachTo( C3DGeometry const * object, C3DSceneNode const * val )
	{
		if ( !object || !val )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_GEOMETRY );
		if ( !val->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_GEOMNODE );

		object->getInternal()->attachTo( *val->getInternal() );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dGeometry_detach( C3DGeometry const * object )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_GEOMETRY );

		object->getInternal()->detach();

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dGeometry_getMesh( C3DGeometry const * object, C3DMesh ** result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_GEOMETRY );

		try
		{
			auto res = object->getInternal()->getMesh();
			C3D_SafeAlloc( *result, C3DMesh );
			( *result )->setInternal( res );
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dGeometry_setMesh( C3DGeometry const * object, C3DMesh const * val )
	{
		if ( !object || !val )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_GEOMETRY );
		if ( !val->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_GEOMMESH );

		object->getInternal()->setMesh( val->getInternal() );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dGeometry_getMaterial( C3DGeometry const * object, C3DSubmesh const * submesh, C3DMaterial ** result )
	{
		if ( !object || !submesh || !result )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_GEOMETRY );
		if ( !submesh->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_GEOMSUBMESH );

		try
		{
			auto res = object->getInternal()->getMaterial( *submesh->internal );
			C3D_SafeAlloc( *result, C3DMaterial );
			( *result )->internal = res;
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dGeometry_setMaterial( C3DGeometry const * object, C3DSubmesh const * submesh, C3DMaterial const * val )
	{
		if ( !object || !submesh || !val )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_GEOMETRY );
		if ( !submesh->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_GEOMSUBMESH );
		if ( !val->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_GEOMMAT );

		object->getInternal()->setMaterial( *submesh->internal, val->internal );

		return C3D_OK;
	}

#ifdef __cplusplus
}
#endif
