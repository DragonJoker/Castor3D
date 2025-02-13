#include "CCastor3D/Castor3D.h"
#include "CCastor3D/Castor3DCommon.h"

#include <Castor3D/Model/Mesh/Submesh/Component/LinesMapping.hpp>

#ifdef __cplusplus
extern "C"
{
#endif

	static const C3DString ERROR_UNINITIALISED_LINES = cuT( "The mapping must be initialised" );

	C3D_CAPIMETHODIMP c3dLinesMapping_delete( C3DLinesMapping * object )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			cc3d::reportWarning( ERROR_UNINITIALISED_LINES );

		delete object;

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dLinesMapping_getLinesCount( C3DLinesMapping const * object, uint32_t * result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_LINES );

		*result = object->internal->getData().getCount();

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dLinesMapping_addLine( C3DLinesMapping * object, uint32_t x, uint32_t y )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_LINES );

		object->internal->getData().addLine( x, y );

		return C3D_OK;
	}

#ifdef __cplusplus
}
#endif
