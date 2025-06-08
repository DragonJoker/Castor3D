#include "CCastor3D/Castor3D.h"
#include "CCastor3D/Castor3DCommon.h"

#include <CastorUtils/Graphics/Font.hpp>

#ifdef __cplusplus
extern "C"
{
#endif

	static const C3DString ERROR_UNINITIALISED_FONT = cuT( "The font must be initialised" );

	C3D_CAPIMETHODIMP c3dFont_delete( C3DFont * object )
	{
		if ( !object )
			return C3D_POINTER;

		delete object;

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dFont_getHeight( C3DFont const * object, uint32_t * result )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_FONT );

		*result = object->internal->getHeight();

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dFont_getMaxGlyphHeight( C3DFont const * object, uint32_t * result )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_FONT );

		*result = object->internal->getMaxGlyphHeight();

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dFont_getMaxGlyphWidth( C3DFont const * object, uint32_t * result )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_FONT );

		*result = object->internal->getMaxGlyphWidth();

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dFont_getMaxImageHeight( C3DFont const * object, uint32_t * result )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_FONT );

		*result = object->internal->getMaxImageHeight();

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dFont_getMaxImageWidth( C3DFont const * object, uint32_t * result )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_FONT );

		*result = object->internal->getMaxImageWidth();

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dFont_getMaxBearing( C3DFont const * object, C3DVec2 * result )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_FONT );

		*result = cc3d::convert( object->internal->getMaxBearing() );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dFont_getGlyph( C3DFont const * object, char32_t glyph, C3DGlyph ** result )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_FONT );

		try
		{
			auto & res = object->internal->getGlyphAt( glyph );
			C3D_SafeAlloc( *result, C3DGlyph );
			( *result )->internal = &res;
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

#ifdef __cplusplus
}
#endif
