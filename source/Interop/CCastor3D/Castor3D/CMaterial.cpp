#include "CCastor3D/Castor3D.h"
#include "CCastor3D/Castor3DCommon.h"

#include <Castor3D/Material/Material.hpp>

#ifdef __cplusplus
extern "C"
{
#endif

	static const C3DString ERROR_UNINITIALISED_MAT = cuT( "The material must be initialised" );
	static const C3DString ERROR_UNINITIALISED_MATPASS = cuT( "The pass must be initialised" );

	C3D_CAPIMETHODIMP c3dMaterial_delete( C3DMaterial * object )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			cc3d::reportWarning( ERROR_UNINITIALISED_MAT );

		delete object;

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dMaterial_getPassCount( C3DMaterial const * object, uint32_t * result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_MAT );

		try
		{
			*result = object->internal->getPassCount();
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dMaterial_getPass( C3DMaterial const * object, uint32_t index, C3DPass ** result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_MAT );

		try
		{
			auto res = object->internal->getPass( index );
			C3D_SafeAlloc( *result, C3DPass );
			( *result )->internal = res;
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dMaterial_createPass( C3DMaterial const * object, C3DPass ** result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_MAT );

		try
		{
			auto res = object->internal->createPass();
			C3D_SafeAlloc( *result, C3DPass );
			( *result )->internal = res;
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dMaterial_removePass( C3DMaterial const * object, C3DPass * pass )
	{
		if ( !object || !pass )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_MAT );
		if ( !pass->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_MATPASS );

		try
		{
			object->internal->removePass( *pass->internal );
			pass->internal = nullptr;
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

#ifdef __cplusplus
}
#endif
