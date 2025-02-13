#include "CCastor3D/Castor3D.h"
#include "CCastor3D/Castor3DCommon.h"

#include <Castor3D/Scene/Light/DirectionalLight.hpp>

#ifdef __cplusplus
extern "C"
{
#endif

	static const C3DString ERROR_UNINITIALISED_DIRLGT = cuT( "The directional light must be initialised" );

	C3D_CAPIMETHODIMP c3dDirectionalLight_delete( C3DDirectionalLight * object )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			cc3d::reportWarning( ERROR_UNINITIALISED_DIRLGT );

		delete object;

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dDirectionalLight_getColour( C3DDirectionalLight const * object, C3DVec3 * result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_DIRLGT );

		*result = cc3d::convert( object->internal->getColour() );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dDirectionalLight_setColour( C3DDirectionalLight * object, C3DVec3 const * val )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_DIRLGT );

		object->internal->setColour( cc3d::convert( *val ) );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dDirectionalLight_getShadows( C3DDirectionalLight const * object, C3DShadow ** result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_DIRLGT );

		try
		{
			auto & res = object->internal->getShadowConfig();
			C3D_SafeAlloc( *result, C3DShadow );
			( *result )->internal = &res;
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dDirectionalLight_getIllumination( C3DDirectionalLight const * object, float * result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_DIRLGT );

		*result = object->internal->getIllumination().lux();

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dDirectionalLight_setIllumination( C3DDirectionalLight * object, float val )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_DIRLGT );

		object->internal->setIllumination( castor::Illumination{ val } );

		return C3D_OK;
	}

#ifdef __cplusplus
}
#endif
