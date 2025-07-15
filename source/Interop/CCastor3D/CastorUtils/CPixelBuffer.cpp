#include "CCastor3D/Castor3D.h"
#include "CCastor3D/Castor3DCommon.h"

#include <CastorUtils/Graphics/PixelBuffer.hpp>

#ifdef __cplusplus
extern "C"
{
#endif

	static const C3DString ERROR_UNINITIALISED_BUFFER = cuT( "The buffer must be initialised" );
	static const C3DString ERROR_INITIALISED_BUFFER = cuT( "The buffer is already initialised" );

	C3D_CAPIMETHODIMP c3dPixelBuffer_create( C3DSize const * size, C3D_PIXEL_FORMAT pf, C3DPixelBuffer ** result )
	{
		if ( !size || !result )
			return C3D_POINTER;
		if ( *result && ( *result )->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_INITIALISED_BUFFER );

		try
		{
			auto buffer = c3d::PxBufferBase::create( { size->width, size->height }, c3d::PixelFormat( pf ) );
			C3D_SafeAlloc( *result, C3DPixelBuffer );
			( *result )->internal = c3d::move( buffer );
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dPixelBuffer_delete( C3DPixelBuffer * object )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			cc3d::reportWarning( ERROR_UNINITIALISED_BUFFER );

		object->internal.reset();
		delete object;

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dPixelBuffer_getDimensions( C3DPixelBuffer const * object, C3DSize * result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_BUFFER );

		result->width = object->internal->getWidth();
		result->height = object->internal->getHeight();

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dPixelBuffer_getWidth( C3DPixelBuffer const * object, uint32_t * result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_BUFFER );

		*result = object->internal->getWidth();

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dPixelBuffer_getHeight( C3DPixelBuffer const * object, uint32_t * result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_BUFFER );

		*result = object->internal->getHeight();

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dPixelBuffer_getPixelFormat( C3DPixelBuffer const * object, C3D_PIXEL_FORMAT * result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_BUFFER );

		*result = C3D_PIXEL_FORMAT( object->internal->getFormat() );

		return C3D_OK;
	}

#ifdef __cplusplus
}
#endif
