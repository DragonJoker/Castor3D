#include "CCastor3D/Castor3D.h"
#include "CCastor3D/Castor3DCommon.h"

#include <CastorUtils/Graphics/Glyph.hpp>

#ifdef __cplusplus
extern "C"
{
#endif

	C3D_CAPIMETHODIMP c3dGlyph_delete( C3DGlyph * object )
	{
		if ( !object )
			return C3D_POINTER;

		delete object;

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dGlyph_getSize( C3DGlyph const * object, C3DVec2 * result )
	{
		if ( !object || !result )
			return C3D_POINTER;

		*result = cc3d::convert( object->internal->getSize() );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dGlyph_getBearing( C3DGlyph const * object, C3DVec2 * result )
	{
		if ( !object || !result )
			return C3D_POINTER;

		*result = cc3d::convert( object->internal->getBearing() );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dGlyph_getAdvance( C3DGlyph const * object, float * result )
	{
		if ( !object || !result )
			return C3D_POINTER;

		*result = object->internal->getAdvance();

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dGlyph_getBitmapSize( C3DGlyph const * object, C3DSize * result )
	{
		if ( !object || !result )
			return C3D_POINTER;

		*result = cc3d::convert( object->internal->getBitmapSize() );

		return C3D_OK;
	}

#ifdef __cplusplus
}
#endif
