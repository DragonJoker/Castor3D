#include "CCastor3D/Castor3D.h"
#include "CCastor3D/Castor3DCommon.h"

#include <Castor3D/Model/Mesh/Submesh/Component/TriFaceMapping.hpp>

#ifdef __cplusplus
extern "C"
{
#endif

	static const C3DString ERROR_UNINITIALISED_TRIANGLES = cuT( "The mapping must be initialised" );

	C3D_CAPIMETHODIMP c3dTriFaceMapping_delete( C3DTriFaceMapping * object )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			cc3d::reportWarning( ERROR_UNINITIALISED_TRIANGLES );
		
		delete object;

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dTriFaceMapping_getFacesCount( C3DTriFaceMapping const * object, uint32_t * result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_TRIANGLES );

		*result = object->internal->getData().getCount();

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dTriFaceMapping_addFace( C3DTriFaceMapping * object, uint32_t x, uint32_t y, uint32_t z )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_TRIANGLES );

		object->internal->getData().addFace( x, y, z );

		return C3D_OK;
	}

#ifdef __cplusplus
}
#endif
