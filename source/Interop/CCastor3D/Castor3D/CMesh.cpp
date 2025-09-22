#include "CCastor3D/Castor3D.h"
#include "CCastor3D/Castor3DCommon.h"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Model/Mesh/Mesh.hpp>
#include <Castor3D/Model/Mesh/MeshFactory.hpp>
#include <Castor3D/Model/Mesh/MeshGenerator.hpp>
#include <Castor3D/Scene/Scene.hpp>

#ifdef __cplusplus
extern "C"
{
#endif

	static const C3DString ERROR_UNINITIALISED_MESH = cuT( "The mesh must be initialised" );
	static const C3DString ERROR_UNINITIALISED_MESHSUB = cuT( "The submesh must be initialised" );

	C3D_CAPIMETHODIMP c3dMesh_delete( C3DMesh * object )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->getInternal() )
			cc3d::reportWarning( ERROR_UNINITIALISED_MESH );

		object->releaseInternal();
		delete object;

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dMesh_getSubmeshCount( C3DMesh const * object, uint32_t * result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_MESH );

		try
		{
			*result = object->getInternal()->getSubmeshCount();
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dMesh_getSubmesh( C3DMesh const * object, uint32_t index, C3DSubmesh ** result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_MESH );

		try
		{
			auto res = object->getInternal()->getSubmesh( index );
			C3D_SafeAlloc( *result, C3DSubmesh );
			( *result )->internal = res;
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dMesh_createSubmesh( C3DMesh const * object, C3DSubmesh ** result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_MESH );

		try
		{
			auto res = object->getInternal()->createSubmesh();
			C3D_SafeAlloc( *result, C3DSubmesh );
			( *result )->internal = res;
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dMesh_removeSubmesh( C3DMesh const * object, C3DSubmesh * submesh )
	{
		if ( !object || !submesh )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_MESH );
		if ( !submesh->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_MESHSUB );

		try
		{
			object->getInternal()->removeSubmesh( *submesh->internal );
			submesh->internal = nullptr;
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

#ifdef __cplusplus
}
#endif
