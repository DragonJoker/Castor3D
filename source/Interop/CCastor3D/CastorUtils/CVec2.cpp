#include "CCastor3D/Castor3D.h"

#include <cmath>

#ifdef __cplusplus
extern "C"
{
#endif

	C3D_CAPIMETHODIMP c3dVec2_negate( C3DVec2 * object )
	{
		if ( !object )
			return C3D_POINTER;

		object->x = -object->x;
		object->y = -object->y;

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dVec2_normalise( C3DVec2 * object )
	{
		float length{};
		auto ret = c3dVec2_length( object, &length );

		if ( ret == C3D_OK )
		{
			if ( length == 0.0f )
				return C3D_DOMAIN;

			object->x /= length;
			object->y /= length;
		}

		return ret;
	}

	C3D_CAPIMETHODIMP c3dVec2_length( C3DVec2 const * object, float * result )
	{
		auto ret = c3dVec2_dot( object, object, result );

		if ( ret == C3D_OK )
		{
			*result = float( sqrt( *result ) );
		}

		return ret;
	}

	C3D_CAPIMETHODIMP c3dVec2_dot( C3DVec2 const * lhs, C3DVec2 const * rhs, float * result )
	{
		if ( !lhs || !rhs || !result )
			return C3D_POINTER;

		*result = lhs->x * rhs->x
			+ lhs->y * rhs->y;

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dVec2_compMul( C3DVec2 const * lhs, C3DVec2 const * rhs, C3DVec2 * result )
	{
		if ( !lhs || !rhs || !result )
			return C3D_POINTER;

		result->x = lhs->x * rhs->x;
		result->y = lhs->y * rhs->y;

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dVec2_compAdd( C3DVec2 const * lhs, C3DVec2 const * rhs, C3DVec2 * result )
	{
		if ( !lhs || !rhs || !result )
			return C3D_POINTER;

		result->x = lhs->x + rhs->x;
		result->y = lhs->y + rhs->y;

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dVec2_compSub( C3DVec2 const * lhs, C3DVec2 const * rhs, C3DVec2 * result )
	{
		if ( !lhs || !rhs || !result )
			return C3D_POINTER;

		result->x = lhs->x - rhs->x;
		result->y = lhs->y - rhs->y;

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dVec2_mul( C3DVec2 const * lhs, float rhs, C3DVec2 * result )
	{
		if ( !lhs || !result )
			return C3D_POINTER;

		result->x = lhs->x * rhs;
		result->y = lhs->y * rhs;

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dVec2_div( C3DVec2 const * lhs, float rhs, C3DVec2 * result )
	{
		if ( !lhs || !result )
			return C3D_POINTER;

		if ( rhs == 0.0f )
			return C3D_DOMAIN;

		result->x = lhs->x / rhs;
		result->y = lhs->y / rhs;

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dVec2_add( C3DVec2 const * lhs, float rhs, C3DVec2 * result )
	{
		if ( !lhs || !result )
			return C3D_POINTER;

		result->x = lhs->x + rhs;
		result->y = lhs->y + rhs;

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dVec2_sub( C3DVec2 const * lhs, float rhs, C3DVec2 * result )
	{
		if ( !lhs || !result )
			return C3D_POINTER;

		result->x = lhs->x - rhs;
		result->y = lhs->y - rhs;

		return C3D_OK;
	}

#ifdef __cplusplus
}
#endif
