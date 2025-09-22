#include "CCastor3D/Castor3D.h"
#include "CCastor3D/Castor3DCommon.h"

#include <Castor3D/Scene/Light/PointLight.hpp>

#ifdef __cplusplus
extern "C"
{
#endif
	
	static const C3DString ERROR_UNINITIALISED_PNTLGT = cuT( "The point light must be initialised" );

	C3D_CAPIMETHODIMP c3dPointLight_delete( C3DPointLight * object )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			cc3d::reportWarning( ERROR_UNINITIALISED_PNTLGT );

		delete object;

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dPointLight_getColour( C3DPointLight const * object, C3DVec3 * result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_PNTLGT );

		*result = cc3d::convert( object->internal->getColour() );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dPointLight_setColour( C3DPointLight const * object, C3DVec3 const * val )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_PNTLGT );

		object->internal->setColour( cc3d::convert( *val ) );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dPointLight_getShadows( C3DPointLight const * object, C3DShadow ** result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_PNTLGT );

		try
		{
			auto & res = object->internal->getShadowConfig();
			C3D_SafeAlloc( *result, C3DShadow );
			( *result )->internal = &res;
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dPointLight_getAttenuationRange( C3DPointLight const * object, float * result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_PNTLGT );

		*result = object->internal->getRange();

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dPointLight_setAttenuationRange( C3DPointLight const * object, float val )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_PNTLGT );

		object->internal->setRange( val );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dPointLight_getIntensity( C3DPointLight const * object, float * result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_PNTLGT );

		*result = object->internal->getIntensity().candela();

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dPointLight_setIntensity( C3DPointLight const * object, float val )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_PNTLGT );

		object->internal->setIntensity( c3d::LuminousIntensity{ val } );

		return C3D_OK;
	}

#ifdef __cplusplus
}
#endif
