#include "CCastor3D/Castor3D.h"
#include "CCastor3D/Castor3DCommon.h"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Render/RenderTarget.hpp>

#ifdef __cplusplus
extern "C"
{
#endif

	static const C3DString ERROR_UNINITIALISED_TARGET = cuT( "The render target must be initialised" );

	C3D_CAPIMETHODIMP c3dRenderTarget_delete( C3DRenderTarget * object )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			cc3d::reportWarning( ERROR_UNINITIALISED_TARGET );

		delete object;

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dRenderTarget_initialise( C3DRenderTarget * object )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_TARGET );

		c3d::RenderDevice const & device = *object->internal->getOwner()->getRenderDevice();
		object->internal->initialise( device );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dRenderTarget_cleanup( C3DRenderTarget * object )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_TARGET );

		c3d::RenderDevice const & device = *object->internal->getOwner()->getRenderDevice();
		object->internal->cleanup( device );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dRenderTarget_getCamera( C3DRenderTarget const * object, C3DCamera ** result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_TARGET );

		try
		{
			auto res = object->internal->getCamera();
			C3D_SafeAlloc( *result, C3DCamera );
			( *result )->setInternal( res );
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dRenderTarget_setCamera( C3DRenderTarget * object, C3DCamera const * camera )
	{
		if ( !object || !camera )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_TARGET );
		if ( !camera->getInternal() )
			return cc3d::reportError( C3D_FAILURE, cuT( "The camera must be initialised" ) );

		object->internal->setCamera( *camera->getInternal() );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dRenderTarget_getViewportType( C3DRenderTarget const * object, C3D_VIEWPORT_TYPE * result )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_TARGET );

		*result = C3D_VIEWPORT_TYPE( object->internal->getViewportType() );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dRenderTarget_setViewportType( C3DRenderTarget * object, C3D_VIEWPORT_TYPE val )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_TARGET );

		object->internal->setViewportType( c3d::ViewportType( val ) );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dRenderTarget_getScene( C3DRenderTarget const * object, C3DScene ** result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_TARGET );

		try
		{
			auto res = object->internal->getScene();
			C3D_SafeAlloc( *result, C3DScene );
			( *result )->setInternal( res );
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dRenderTarget_setScene( C3DRenderTarget * object, C3DScene const * scene )
	{
		if ( !object || !scene )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_TARGET );
		if ( !scene->getInternal() )
			return cc3d::reportError( C3D_FAILURE, cuT( "The scene must be initialised" ) );

		object->internal->setScene( *scene->getInternal() );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dRenderTarget_getPixelFormat( C3DRenderTarget const * object, C3D_PIXEL_FORMAT * result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_TARGET );

		*result = C3D_PIXEL_FORMAT( object->internal->getPixelFormat() );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dRenderTarget_getRenderSize( C3DRenderTarget const * object, C3DSize * result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_TARGET );

		result->width = object->internal->getRenderSize().getWidth();
		result->height = object->internal->getRenderSize().getHeight();

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dRenderTarget_getDisplaySize( C3DRenderTarget const * object, C3DSize * result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_TARGET );

		result->width = object->internal->getDisplaySize().getWidth();
		result->height = object->internal->getDisplaySize().getHeight();

		return C3D_OK;
	}

#ifdef __cplusplus
}
#endif
