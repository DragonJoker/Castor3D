#include "CCastor3D/Castor3D.h"
#include "CCastor3D/Castor3DCommon.h"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Overlay/Overlay.hpp>

#ifdef __cplusplus
extern "C"
{
#endif

	static const C3DString ERROR_UNINITIALISED_OVERLAY = cuT( "The overlay must be initialised" );
	static const C3DString ERROR_WRONG_OVERLAY_TYPE = cuT( "The overlay is not of the wepected type" );

	C3D_CAPIMETHODIMP c3dOverlay_delete( C3DOverlay * object )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->getInternal() )
			cc3d::reportWarning( ERROR_UNINITIALISED_OVERLAY );

		object->releaseInternal();
		delete object;

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dOverlay_getName( C3DOverlay const * object, C3DString * result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_OVERLAY );

		return cc3d::copyString( object->getInternal()->getName(), result );
	}

	C3D_CAPIMETHODIMP c3dOverlay_getOverlayType( C3DOverlay const * object, C3D_OVERLAY_TYPE * result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_OVERLAY );

		*result = C3D_OVERLAY_TYPE( object->getInternal()->getType() );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dOverlay_getPanelOverlay( C3DOverlay const * object, C3DPanelOverlay ** result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_OVERLAY );

		try
		{
			auto res = object->getInternal()->getPanelOverlay();
			if ( !res )
				return cc3d::reportError( C3D_FAILURE, ERROR_WRONG_OVERLAY_TYPE );

			C3D_SafeAlloc( *result, C3DPanelOverlay );
			( *result )->internal = res;
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dOverlay_getBorderPanelOverlay( C3DOverlay const * object, C3DBorderPanelOverlay ** result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_OVERLAY );

		try
		{
			auto res = object->getInternal()->getBorderPanelOverlay();
			if ( !res )
				return cc3d::reportError( C3D_FAILURE, ERROR_WRONG_OVERLAY_TYPE );

			C3D_SafeAlloc( *result, C3DBorderPanelOverlay );
			( *result )->internal = res;
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dOverlay_getTextOverlay( C3DOverlay const * object, C3DTextOverlay ** result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_OVERLAY );

		try
		{
			auto res = object->getInternal()->getTextOverlay();
			if ( !res )
				return cc3d::reportError( C3D_FAILURE, ERROR_WRONG_OVERLAY_TYPE );

			C3D_SafeAlloc( *result, C3DTextOverlay );
			( *result )->internal = res;
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dOverlay_getChildrenCount( C3DOverlay const * object, uint32_t level, uint32_t * result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_OVERLAY );

		*result = object->getInternal()->getChildrenCount( level );

		return C3D_OK;
	}

#ifdef __cplusplus
}
#endif
