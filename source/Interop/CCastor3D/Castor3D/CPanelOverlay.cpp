#include "CCastor3D/Castor3D.h"
#include "CCastor3D/Castor3DCommon.h"

#include <Castor3D/Overlay/PanelOverlay.hpp>

#ifdef __cplusplus
extern "C"
{
#endif

	static C3DString const ERROR_UNINITIALISED_PANEL = cuT( "The panel must be initialised" );
	static C3DString const ERROR_UNINITIALISED_PANELMAT = cuT( "The material must be initialised" );

	C3D_CAPIMETHODIMP c3dPanelOverlay_delete( C3DPanelOverlay * object )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			cc3d::reportWarning( ERROR_UNINITIALISED_PANEL );

		delete object;

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dPanelOverlay_getPosition( C3DPanelOverlay const * object, C3DPosition * result )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_PANEL );

		result->x = object->internal->getPixelPosition().x();
		result->y = object->internal->getPixelPosition().y();

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dPanelOverlay_setPosition( C3DPanelOverlay const * object, C3DPosition const * val )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_PANEL );

		object->internal->setPixelPosition( { val->x, val->y } );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dPanelOverlay_getSize( C3DPanelOverlay const * object, C3DSize * result )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_PANEL );

		result->width = object->internal->getPixelSize().getWidth();
		result->height = object->internal->getPixelSize().getHeight();

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dPanelOverlay_setSize( C3DPanelOverlay const * object, C3DSize const * val )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_PANEL );

		object->internal->setPixelSize( { val->width, val->height } );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dPanelOverlay_getVisible( C3DPanelOverlay const * object, bool * result )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_PANEL );

		*result = object->internal->isVisible();

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dPanelOverlay_setVisible( C3DPanelOverlay const * object, bool val )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_PANEL );

		object->internal->setVisible( val );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dPanelOverlay_getMaterial( C3DPanelOverlay const * object, C3DMaterial ** result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_PANEL );

		try
		{
			auto res = object->internal->getMaterial();
			C3D_SafeAlloc( *result, C3DMaterial );
			( *result )->internal = res;
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dPanelOverlay_setMaterial( C3DPanelOverlay const * object, C3DMaterial const * val )
	{
		if ( !object || !val )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_PANEL );
		if ( !val->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_PANELMAT );

		object->internal->setMaterial( val->internal );

		return C3D_OK;
	}

#ifdef __cplusplus
}
#endif
