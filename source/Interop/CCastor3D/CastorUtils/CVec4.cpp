#include "CCastor3D/Castor3D.h"

#include <cmath>

#ifdef __cplusplus
extern "C"
{
#endif

	C3D_CAPIMETHODIMP c3dVector4D_negate( C3DVector4D * object )
	{
		if ( !object )
			return C3D_POINTER;

		object->x = -object->x;
		object->y = -object->y;
		object->z = -object->z;
		object->w = -object->w;

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dVector4D_normalise( C3DVector4D * object )
	{
		float length{};
		auto ret = c3dVector4D_length( object, &length );

		if ( ret == C3D_OK )
		{
			if ( length == 0.0f )
				return C3D_DOMAIN;

			object->x /= length;
			object->y /= length;
			object->z /= length;
			object->w /= length;
		}

		return ret;
	}

	C3D_CAPIMETHODIMP c3dVector4D_length( C3DVector4D const * object, float * result )
	{
		auto ret = c3dVector4D_dot( object, object, result );

		if ( ret == C3D_OK )
		{
			*result = sqrt( *result );
		}

		return ret;
	}

	C3D_CAPIMETHODIMP c3dVector4D_dot( C3DVector4D const * lhs, C3DVector4D const * rhs, float * result )
	{
		if ( !lhs || !rhs || !result )
			return C3D_POINTER;

		*result = lhs->x * rhs->x
			+ lhs->y * rhs->y
			+ lhs->z * rhs->z
			+ lhs->w * rhs->w;

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dVector4D_compMul( C3DVector4D const * lhs, C3DVector4D const * rhs, C3DVector4D * result )
	{
		if ( !lhs || !rhs || !result )
			return C3D_POINTER;

		result->x = lhs->x * rhs->x;
		result->y = lhs->y * rhs->y;
		result->z = lhs->z * rhs->z;
		result->w = lhs->w * rhs->w;

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dVector4D_compAdd( C3DVector4D const * lhs, C3DVector4D const * rhs, C3DVector4D * result )
	{
		if ( !lhs || !rhs || !result )
			return C3D_POINTER;

		result->x = lhs->x + rhs->x;
		result->y = lhs->y + rhs->y;
		result->z = lhs->z + rhs->z;
		result->w = lhs->w + rhs->w;

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dVector4D_compSub( C3DVector4D const * lhs, C3DVector4D const * rhs, C3DVector4D * result )
	{
		if ( !lhs || !rhs || !result )
			return C3D_POINTER;

		result->x = lhs->x - rhs->x;
		result->y = lhs->y - rhs->y;
		result->z = lhs->z - rhs->z;
		result->w = lhs->w - rhs->w;

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dVector4D_mul( C3DVector4D const * lhs, float rhs, C3DVector4D * result )
	{
		if ( !lhs || !result )
			return C3D_POINTER;

		result->x = lhs->x * rhs;
		result->y = lhs->y * rhs;
		result->z = lhs->z * rhs;
		result->w = lhs->w * rhs;

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dVector4D_div( C3DVector4D const * lhs, float rhs, C3DVector4D * result )
	{
		if ( !lhs || !result )
			return C3D_POINTER;

		if ( rhs == 0.0f )
			return C3D_DOMAIN;

		result->x = lhs->x / rhs;
		result->y = lhs->y / rhs;
		result->z = lhs->z / rhs;
		result->w = lhs->w / rhs;

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dVector4D_add( C3DVector4D const * lhs, float rhs, C3DVector4D * result )
	{
		if ( !lhs || !result )
			return C3D_POINTER;

		result->x = lhs->x + rhs;
		result->y = lhs->y + rhs;
		result->z = lhs->z + rhs;
		result->w = lhs->w + rhs;

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dVector4D_sub( C3DVector4D const * lhs, float rhs, C3DVector4D * result )
	{
		if ( !lhs || !result )
			return C3D_POINTER;

		result->x = lhs->x - rhs;
		result->y = lhs->y - rhs;
		result->z = lhs->z - rhs;
		result->w = lhs->w - rhs;

		return C3D_OK;
	}

#ifdef __cplusplus
}
#endif
