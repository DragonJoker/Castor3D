#include "CCastor3D/Castor3D.h"
#include "CCastor3D/Castor3DCommon.h"

#include <Castor3D/Overlay/TextOverlay.hpp>

#ifdef __cplusplus
extern "C"
{
#endif

	static C3DString const ERROR_UNINITIALISED_TEXT = cuT( "The text must be initialised" );
	static C3DString const ERROR_UNINITIALISED_TEXTMAT = cuT( "The material must be initialised" );

	C3D_CAPIMETHODIMP c3dTextOverlay_delete( C3DTextOverlay * object )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			cc3d::reportWarning( ERROR_UNINITIALISED_TEXT );

		delete object;

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dTextOverlay_getPosition( C3DTextOverlay const * object, C3DPosition * result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_TEXT );

		result->x = object->internal->getPixelPosition().x();
		result->y = object->internal->getPixelPosition().y();

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dTextOverlay_setPosition( C3DTextOverlay * object, C3DPosition const * val )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_TEXT );

		object->internal->setPixelPosition( { val->x, val->y } );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dTextOverlay_getSize( C3DTextOverlay const * object, C3DSize * result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_TEXT );

		result->width = object->internal->getPixelSize().getWidth();
		result->height = object->internal->getPixelSize().getHeight();

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dTextOverlay_setSize( C3DTextOverlay * object, C3DSize const * val )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_TEXT );

		object->internal->setPixelSize( { val->width, val->height } );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dTextOverlay_getVisible( C3DTextOverlay const * object, bool * result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_TEXT );

		*result = object->internal->isVisible();

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dTextOverlay_setVisible( C3DTextOverlay * object, bool val )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_TEXT );

		object->internal->setVisible( val );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dTextOverlay_getMaterial( C3DTextOverlay const * object, C3DMaterial ** result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_TEXT );

		try
		{
			auto res = object->internal->getMaterial();
			C3D_SafeAlloc( *result, C3DMaterial );
			( *result )->internal = res;
		}
		C3D_CatchCommonExceptions()

			return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dTextOverlay_setMaterial( C3DTextOverlay * object, C3DMaterial const * val )
	{
		if ( !object || !val )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_TEXT );
		if ( !val->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_TEXTMAT );

		object->internal->setMaterial( val->internal );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dTextOverlay_getFont( C3DTextOverlay const * object, C3DString * result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_TEXT );

		return cc3d::copyString( object->internal->getFontName(), result );
	}

	C3D_CAPIMETHODIMP c3dTextOverlay_setFont( C3DTextOverlay * object, C3DString val )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_TEXT );

		object->internal->setFont( val );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dTextOverlay_getCaption( C3DTextOverlay const * object, C3DString * result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_TEXT );

		return cc3d::copyString( c3d::makeString( object->internal->getCaption() ), result );
	}

	C3D_CAPIMETHODIMP c3dTextOverlay_setCaption( C3DTextOverlay * object, C3DString val )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_TEXT );

		object->internal->setCaption( c3d::toUtf8U32String( c3d::makeString( val ) ) );

		return C3D_OK;
	}

#ifdef __cplusplus
}
#endif
