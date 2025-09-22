#include "CCastor3D/Castor3D.h"
#include "CCastor3D/Castor3DCommon.h"

#include <Castor3D/Overlay/BorderPanelOverlay.hpp>

#ifdef __cplusplus
extern "C"
{
#endif

	static const C3DString ERROR_UNINITIALISED_BORDERPANEL = cuT( "The border panel must be initialised" );
	static const C3DString ERROR_UNINITIALISED_BORDERMAT = cuT( "The material must be initialised" );

	C3D_CAPIMETHODIMP c3dBorderPanelOverlay_delete( C3DBorderPanelOverlay * object )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			cc3d::reportWarning( ERROR_UNINITIALISED_BORDERPANEL );

		delete object;

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dBorderPanelOverlay_getPosition( C3DBorderPanelOverlay const * object, C3DPosition * result )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_BORDERPANEL );

		result->x = object->internal->getPixelPosition().x();
		result->y = object->internal->getPixelPosition().y();

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dBorderPanelOverlay_setPosition( C3DBorderPanelOverlay const * object, C3DPosition const * val )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_BORDERPANEL );

		object->internal->setPixelPosition( { val->x, val->y } );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dBorderPanelOverlay_getSize( C3DBorderPanelOverlay const * object, C3DSize * result )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_BORDERPANEL );

		result->width = object->internal->getPixelSize().getWidth();
		result->height = object->internal->getPixelSize().getHeight();

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dBorderPanelOverlay_setSize( C3DBorderPanelOverlay const * object, C3DSize const * val )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_BORDERPANEL );

		object->internal->setPixelSize( { val->width, val->height } );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dBorderPanelOverlay_getVisible( C3DBorderPanelOverlay const * object, bool * result )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_BORDERPANEL );

		*result = object->internal->isVisible();

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dBorderPanelOverlay_setVisible( C3DBorderPanelOverlay const * object, bool val )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_BORDERPANEL );

		object->internal->setVisible( val );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dBorderPanelOverlay_getMaterial( C3DBorderPanelOverlay const * object, C3DMaterial ** result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_BORDERPANEL );

		try
		{
			auto res = object->internal->getMaterial();
			C3D_SafeAlloc( *result, C3DMaterial );
			( *result )->internal = res;
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dBorderPanelOverlay_setMaterial( C3DBorderPanelOverlay const * object, C3DMaterial const * val )
	{
		if ( !object || !val )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_BORDERPANEL );
		if ( !val->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_BORDERMAT );

		object->internal->setMaterial( val->internal );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dBorderPanelOverlay_getLeftBorderSize( C3DBorderPanelOverlay const * object, uint32_t * result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_BORDERPANEL );

		*result = object->internal->getPixelLeftBorderSize();

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dBorderPanelOverlay_setLeftBorderSize( C3DBorderPanelOverlay const * object, uint32_t val )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_BORDERPANEL );

		object->internal->setPixelLeftBorderSize( val );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dBorderPanelOverlay_getRightBorderSize( C3DBorderPanelOverlay const * object, uint32_t * result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_BORDERPANEL );

		*result = object->internal->getPixelRightBorderSize();

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dBorderPanelOverlay_setRightBorderSize( C3DBorderPanelOverlay const * object, uint32_t val )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_BORDERPANEL );

		object->internal->setPixelRightBorderSize( val );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dBorderPanelOverlay_getTopBorderSize( C3DBorderPanelOverlay const * object, uint32_t * result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_BORDERPANEL );

		*result = object->internal->getPixelTopBorderSize();

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dBorderPanelOverlay_setTopBorderSize( C3DBorderPanelOverlay const * object, uint32_t val )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_BORDERPANEL );

		object->internal->setPixelTopBorderSize( val );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dBorderPanelOverlay_getBottomBorderSize( C3DBorderPanelOverlay const * object, uint32_t * result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_BORDERPANEL );

		*result = object->internal->getPixelBottomBorderSize();

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dBorderPanelOverlay_setBottomBorderSize( C3DBorderPanelOverlay const * object, uint32_t val )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_BORDERPANEL );

		object->internal->setPixelBottomBorderSize( val );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dBorderPanelOverlay_getBorderMaterial( C3DBorderPanelOverlay const * object, C3DMaterial ** result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_BORDERPANEL );

		try
		{
			auto res = object->internal->getBorderMaterial();
			C3D_SafeAlloc( *result, C3DMaterial );
			( *result )->internal = res;
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dBorderPanelOverlay_setBorderMaterial( C3DBorderPanelOverlay const * object, C3DMaterial const * val )
	{
		if ( !object || !val )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_BORDERPANEL );
		if ( !val->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_BORDERMAT );

		object->internal->setBorderMaterial( val->internal );

		return C3D_OK;
	}

#ifdef __cplusplus
}
#endif
