#include "CCastor3D/Castor3D.h"

#include <cmath>

#ifdef __cplusplus
extern "C"
{
#endif

	C3D_CAPIMETHODIMP c3dVec3_negate( C3DVec3 * object )
	{
		if ( !object )
			return C3D_POINTER;

		object->x = -object->x;
		object->y = -object->y;
		object->z = -object->z;

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dVec3_normalise( C3DVec3 * object )
	{
		float length{};
		auto ret = c3dVec3_length( object, &length );

		if ( ret == C3D_OK )
		{
			if ( length == 0.0f )
				return C3D_DOMAIN;

			object->x /= length;
			object->y /= length;
			object->z /= length;
		}

		return ret;
	}

	C3D_CAPIMETHODIMP c3dVec3_length( C3DVec3 const * object, float * result )
	{
		auto ret = c3dVec3_dot( object, object, result );

		if ( ret == C3D_OK )
		{
			*result = float( sqrt( *result ) );
		}

		return ret;
	}

	C3D_CAPIMETHODIMP c3dVec3_dot( C3DVec3 const * lhs, C3DVec3 const * rhs, float * result )
	{
		if ( !lhs || !rhs || !result )
			return C3D_POINTER;

		*result = lhs->x * rhs->x
			+ lhs->y * rhs->y
			+ lhs->z * rhs->z;

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dVec3_cross( C3DVec3 const * lhs, C3DVec3 const * rhs, C3DVec3 * result )
	{
		if ( !lhs || !rhs || !result )
			return C3D_POINTER;

		result->x = ( lhs->y * rhs->z ) - ( lhs->z * rhs->y );
		result->y = ( lhs->z * rhs->x ) - ( lhs->x * rhs->z );
		result->z = ( lhs->x * rhs->y ) - ( lhs->y * rhs->x );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dVec3_compMul( C3DVec3 const * lhs, C3DVec3 const * rhs, C3DVec3 * result )
	{
		if ( !lhs || !rhs || !result )
			return C3D_POINTER;

		result->x = lhs->x * rhs->x;
		result->y = lhs->y * rhs->y;
		result->z = lhs->z * rhs->z;

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dVec3_compAdd( C3DVec3 const * lhs, C3DVec3 const * rhs, C3DVec3 * result )
	{
		if ( !lhs || !rhs || !result )
			return C3D_POINTER;

		result->x = lhs->x + rhs->x;
		result->y = lhs->y + rhs->y;
		result->z = lhs->z + rhs->z;

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dVec3_compSub( C3DVec3 const * lhs, C3DVec3 const * rhs, C3DVec3 * result )
	{
		if ( !lhs || !rhs || !result )
			return C3D_POINTER;

		result->x = lhs->x - rhs->x;
		result->y = lhs->y - rhs->y;
		result->z = lhs->z - rhs->z;

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dVec3_mul( C3DVec3 const * lhs, float rhs, C3DVec3 * result )
	{
		if ( !lhs || !result )
			return C3D_POINTER;

		result->x = lhs->x * rhs;
		result->y = lhs->y * rhs;
		result->z = lhs->z * rhs;

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dVec3_div( C3DVec3 const * lhs, float rhs, C3DVec3 * result )
	{
		if ( !lhs || !result )
			return C3D_POINTER;

		if ( rhs == 0.0f )
			return C3D_DOMAIN;

		result->x = lhs->x / rhs;
		result->y = lhs->y / rhs;
		result->z = lhs->z / rhs;

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dVec3_add( C3DVec3 const * lhs, float rhs, C3DVec3 * result )
	{
		if ( !lhs || !result )
			return C3D_POINTER;

		result->x = lhs->x + rhs;
		result->y = lhs->y + rhs;
		result->z = lhs->z + rhs;

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dVec3_sub( C3DVec3 const * lhs, float rhs, C3DVec3 * result )
	{
		if ( !lhs || !result )
			return C3D_POINTER;

		result->x = lhs->x - rhs;
		result->y = lhs->y - rhs;
		result->z = lhs->z - rhs;

		return C3D_OK;
	}

#ifdef __cplusplus
}
#endif
