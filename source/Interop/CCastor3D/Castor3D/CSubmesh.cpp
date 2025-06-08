#include "CCastor3D/Castor3D.h"
#include "CCastor3D/Castor3DCommon.h"

#include <Castor3D/Model/Mesh/Submesh/Submesh.hpp>
#include <Castor3D/Model/Mesh/Submesh/Component/LinesMapping.hpp>
#include <Castor3D/Model/Mesh/Submesh/Component/TriFaceMapping.hpp>

#ifdef __cplusplus
extern "C"
{
#endif

	static const C3DString ERROR_UNINITIALISED_SUBMESH = cuT( "The submesh must be initialised" );
	static const C3DString ERROR_INITIALISED_SUBMESH = cuT( "The submesh must be removed from its parent mesh" );
	static const C3DString ERROR_WRONG_MAPPING_TYPE = cuT( "The index mapping is not of the expected type" );

	C3D_CAPIMETHODIMP c3dSubmesh_delete( C3DSubmesh * object )
	{
		if ( !object )
			return C3D_POINTER;
		if ( object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_INITIALISED_SUBMESH );

		delete object;

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dSubmesh_getPointsCount( C3DSubmesh const * object, uint32_t * result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SUBMESH );

		*result = object->internal->getPointsCount();

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dSubmesh_getIndexMappingType( C3DSubmesh const * object, C3D_INDEX_MAPPING_TYPE * result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SUBMESH );

		auto const & type = object->internal->getIndexMapping()->getType();
		*result = C3D_INDEX_MAPPING_TYPE_UNSUPPORTED;

		if ( type == castor3d::LinesMapping::TypeName )
		{
			*result = C3D_INDEX_MAPPING_TYPE_LINES;
		}
		else if ( type == castor3d::TriFaceMapping::TypeName )
		{
			*result = C3D_INDEX_MAPPING_TYPE_TRIANGLES;
		}

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dSubmesh_getTriFaceMapping( C3DSubmesh const * object, C3DTriFaceMapping ** result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SUBMESH );

		try
		{
			auto res = object->internal->getIndexMapping();
			if ( object->internal->getIndexMapping()->getType() != castor3d::TriFaceMapping::TypeName )
				return cc3d::reportError( C3D_FAILURE, ERROR_WRONG_MAPPING_TYPE );

			C3D_SafeAlloc( *result, C3DTriFaceMapping );
			( *result )->internal = static_cast< castor3d::TriFaceMapping * >( res );
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dSubmesh_getLineMapping( C3DSubmesh const * object, C3DLineMapping ** result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SUBMESH );

		try
		{
			auto res = object->internal->getIndexMapping();
			if ( object->internal->getIndexMapping()->getType() != castor3d::LinesMapping::TypeName )
				return cc3d::reportError( C3D_FAILURE, ERROR_WRONG_MAPPING_TYPE );

			C3D_SafeAlloc( *result, C3DLineMapping );
			( *result )->internal = static_cast< castor3d::LinesMapping * >( res );
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dSubmesh_addPoint( C3DSubmesh * object, C3DVec3 const * point )
	{
		if ( !object || !point )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SUBMESH );

		try
		{
			object->internal->addPoint( cc3d::convert( *point ) );
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

#ifdef __cplusplus
}
#endif
