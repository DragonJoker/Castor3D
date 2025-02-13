#include "CCastor3D/Castor3D.h"
#include "CCastor3D/Castor3DCommon.h"

#include <Castor3D/Scene/Light/SpotLight.hpp>

#ifdef __cplusplus
extern "C"
{
#endif

	static const C3DString ERROR_UNINITIALISED_SPTLGT = cuT( "The spot light must be initialised" );

	C3D_CAPIMETHODIMP c3dSpotLight_delete( C3DSpotLight * object )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			cc3d::reportWarning( ERROR_UNINITIALISED_SPTLGT );

		delete object;

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dSpotLight_getColour( C3DSpotLight const * object, C3DVec3 * result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SPTLGT );

		*result = cc3d::convert( object->internal->getColour() );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dSpotLight_setColour( C3DSpotLight * object, C3DVec3 const * val )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SPTLGT );

		object->internal->setColour( cc3d::convert( *val ) );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dSpotLight_getShadows( C3DSpotLight const * object, C3DShadow ** result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SPTLGT );

		try
		{
			auto & res = object->internal->getShadowConfig();
			C3D_SafeAlloc( *result, C3DShadow );
			( *result )->internal = &res;
		}
		C3D_CatchCommonExceptions()

			return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dSpotLight_getAttenuationRange( C3DSpotLight const * object, float * result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SPTLGT );

		*result = object->internal->getRange();

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dSpotLight_setAttenuationRange( C3DSpotLight * object, float val )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SPTLGT );

		object->internal->setRange( val );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dSpotLight_getIntensity( C3DSpotLight const * object, float * result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SPTLGT );

		*result = object->internal->getIntensity().candela();

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dSpotLight_setIntensity( C3DSpotLight * object, float val )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SPTLGT );

		object->internal->setIntensity( castor::LuminousIntensity{ val } );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dSpotLight_getExponent( C3DSpotLight const * object, float * result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SPTLGT );

		*result = object->internal->getExponent();

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dSpotLight_setExponent( C3DSpotLight * object, float val )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SPTLGT );

		object->internal->setExponent( val );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dSpotLight_getInnerCutOff( C3DSpotLight const * object, float * result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SPTLGT );

		*result = object->internal->getInnerCutOff().radians();

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dSpotLight_setInnerCutOff( C3DSpotLight * object, float val )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SPTLGT );

		object->internal->setInnerCutOff( castor::Angle::fromRadians( val ) );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dSpotLight_getOuterCutOff( C3DSpotLight const * object, float * result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SPTLGT );

		*result = object->internal->getOuterCutOff().radians();

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dSpotLight_setOuterCutOff( C3DSpotLight * object, float val )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SPTLGT );

		object->internal->setOuterCutOff( castor::Angle::fromRadians( val ) );

		return C3D_OK;
	}

#ifdef __cplusplus
}
#endif
