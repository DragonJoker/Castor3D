#include "CCastor3D/Castor3D.h"
#include "CCastor3D/Castor3DCommon.h"

#include <Castor3D/Scene/Shadow.hpp>

#ifdef __cplusplus
extern "C"
{
#endif

	static const C3DString ERROR_UNINITIALISED_SHADOW = cuT( "The shadow must be initialised" );

	C3D_CAPIMETHODIMP c3dShadow_delete( C3DShadow * object )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			cc3d::reportWarning( ERROR_UNINITIALISED_SHADOW );

		delete object;

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dShadow_getEnabled( C3DShadow const * object, bool * result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SHADOW );

		*result = object->internal->enabled;

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dShadow_setEnabled( C3DShadow * object, bool val )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SHADOW );

		object->internal->enabled = val;

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dShadow_getFilter( C3DShadow const * object, C3D_SHADOW_TYPE * result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SHADOW );

		*result = C3D_SHADOW_TYPE( object->internal->filterType );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dShadow_setFilter( C3DShadow * object, C3D_SHADOW_TYPE val )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SHADOW );

		object->internal->filterType = castor3d::ShadowType( val );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dShadow_getRawMinOffset( C3DShadow const * object, float * result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SHADOW );

		*result = object->internal->rawOffsets[0];

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dShadow_setRawMinOffset( C3DShadow * object, float val )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SHADOW );

		object->internal->rawOffsets[0] = val;

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dShadow_getRawMaxSlopeOffset( C3DShadow const * object, float * result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SHADOW );

		*result = object->internal->rawOffsets[1];

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dShadow_setRawMaxSlopeOffset( C3DShadow * object, float val )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SHADOW );

		object->internal->rawOffsets[1] = val;

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dShadow_getPcfMinOffset( C3DShadow const * object, float * result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SHADOW );

		*result = object->internal->pcfOffsets[0];

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dShadow_setPcfMinOffset( C3DShadow * object, float val )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SHADOW );

		object->internal->pcfOffsets[0] = val;

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dShadow_getPcfMaxSlopeOffset( C3DShadow const * object, float * result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SHADOW );

		*result = object->internal->pcfOffsets[1];

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dShadow_setPcfMaxSlopeOffset( C3DShadow * object, float val )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SHADOW );

		object->internal->pcfOffsets[1] = val;

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dShadow_getPcfFilterSize( C3DShadow const * object, uint32_t * result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SHADOW );

		*result = object->internal->pcfFilterSize.value();

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dShadow_setPcfFilterSize( C3DShadow * object, uint32_t val )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SHADOW );

		object->internal->pcfFilterSize = val;

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dShadow_getPcfSampleCount( C3DShadow const * object, uint32_t * result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SHADOW );

		*result = object->internal->pcfSampleCount.value();

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dShadow_setPcfSampleCount( C3DShadow * object, uint32_t val )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SHADOW );

		object->internal->pcfSampleCount = val;

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dShadow_getVsmMinVariance( C3DShadow const * object, float * result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SHADOW );

		*result = object->internal->vsmMinVariance;

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dShadow_setVsmMinVariance( C3DShadow * object, float val )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SHADOW );

		object->internal->vsmMinVariance = val;

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dShadow_getVsmLightBleedingReduction( C3DShadow const * object, float * result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SHADOW );

		*result = object->internal->vsmLightBleedingReduction;

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dShadow_setVsmLightBleedingReduction( C3DShadow * object, float val )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SHADOW );

		object->internal->vsmLightBleedingReduction = val;

		return C3D_OK;
	}

#ifdef __cplusplus
}
#endif
