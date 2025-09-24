#include "CCastor3D/Castor3D.h"
#include "CCastor3D/Castor3DCommon.h"

#include <CastorUtils/Graphics/Image.hpp>

#ifdef __cplusplus
extern "C"
{
#endif

	static C3DChar const * const ERROR_UNINITIALISED_IMAGE = cuT( "The image must be initialised" );

	C3D_CAPIMETHODIMP c3dImage_delete( C3DImage * object )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			cc3d::reportWarning( ERROR_UNINITIALISED_IMAGE );

		delete object;

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dImage_getBuffer( C3DImage const * object, C3DPixelBuffer ** result )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_IMAGE );

		try
		{
			auto const & src = object->internal->getPxBuffer();
			auto buffer = c3d::PxBufferBase::create( src.getDimensions(), src.getFormat(), src.getConstPtr(), src.getFormat() );
			C3D_SafeAlloc( *result, C3DPixelBuffer );
			( *result )->internal = c3d::move( buffer );
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dImage_resample( C3DImage const * object, C3DSize const * size )
	{
		if ( !object || !size )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_IMAGE );

		object->internal->resample( { size->width, size->height } );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dImage_fill( C3DImage const * object, C3DRgbaColour const * colour )
	{
		if ( !object || !colour )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_IMAGE );

		object->internal->fill( c3d::RgbaColour::fromComponents( colour->r, colour->g, colour->b, colour->a ) );

		return C3D_OK;
	}

#ifdef __cplusplus
}
#endif

