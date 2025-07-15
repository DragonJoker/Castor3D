#include "CCastor3D/Castor3D.h"

#include <CastorUtils/Math/SquareMatrix.hpp>

#ifdef __cplusplus
extern "C"
{
#endif

	C3D_CAPIMETHODIMP c3dMat4_setIdentity( C3DMat4 * object )
	{
		if ( !object )
			return C3D_POINTER;

		object->m11 = 1.0; object->m12 = 0.0; object->m13 = 0.0; object->m14 = 0.0;
		object->m21 = 0.0; object->m22 = 1.0; object->m23 = 0.0; object->m24 = 0.0;
		object->m31 = 0.0; object->m32 = 0.0; object->m33 = 1.0; object->m34 = 0.0;
		object->m41 = 0.0; object->m42 = 0.0; object->m43 = 0.0; object->m44 = 1.0;

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dMat4_transpose( C3DMat4 * object )
	{
		if ( !object )
			return C3D_POINTER;

		std::swap( object->m12, object->m21 );
		std::swap( object->m13, object->m31 );
		std::swap( object->m14, object->m41 );
		std::swap( object->m23, object->m32 );
		std::swap( object->m24, object->m42 );
		std::swap( object->m34, object->m43 );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dMat4_invert( C3DMat4 * object )
	{
		if ( !object )
			return C3D_POINTER;

		auto matrix = c3d::Matrix4x4f{
			{ object->m11, object->m12, object->m13, object->m14
			, object->m21, object->m22, object->m23, object->m24
			, object->m31, object->m32, object->m33, object->m34
			, object->m41, object->m42, object->m43, object->m44 } }.getInverse();
		*object =
			{ matrix[0][0], matrix[0][1], matrix[0][2], matrix[0][3]
			, matrix[1][0], matrix[1][1], matrix[1][2], matrix[1][3]
			, matrix[2][0], matrix[2][1], matrix[2][2], matrix[2][3]
			, matrix[3][0], matrix[3][1], matrix[3][2], matrix[3][3] };

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dMat4_compMul( C3DMat4 const * lhs, C3DMat4 const * rhs, C3DMat4 * result )
	{
		if ( !lhs || !rhs || !result )
			return C3D_POINTER;

		result->m11 = lhs->m11 * rhs->m11; result->m12 = lhs->m12 * rhs->m12; result->m13 = lhs->m13 * rhs->m13; result->m14 = lhs->m14 * rhs->m14;
		result->m21 = lhs->m21 * rhs->m21; result->m22 = lhs->m22 * rhs->m22; result->m23 = lhs->m23 * rhs->m23; result->m24 = lhs->m24 * rhs->m24;
		result->m31 = lhs->m31 * rhs->m31; result->m32 = lhs->m32 * rhs->m32; result->m33 = lhs->m33 * rhs->m33; result->m34 = lhs->m34 * rhs->m34;
		result->m41 = lhs->m41 * rhs->m41; result->m42 = lhs->m42 * rhs->m42; result->m43 = lhs->m43 * rhs->m43; result->m44 = lhs->m44 * rhs->m44;

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dMat4_mul( C3DMat4 const * lhs, C3DMat4 const * rhs, C3DMat4 * result )
	{
		auto matrix = c3d::Matrix4x4f{
				{ lhs->m11, lhs->m12, lhs->m13, lhs->m14
				, lhs->m21, lhs->m22, lhs->m23, lhs->m24
				, lhs->m31, lhs->m32, lhs->m33, lhs->m34
				, lhs->m41, lhs->m42, lhs->m43, lhs->m44 } }
			* c3d::Matrix4x4f{
				{ rhs->m11, rhs->m12, rhs->m13, rhs->m14
				, rhs->m21, rhs->m22, rhs->m23, rhs->m24
				, rhs->m31, rhs->m32, rhs->m33, rhs->m34
				, rhs->m41, rhs->m42, rhs->m43, rhs->m44 } };
		*result =
			{ matrix[0][0], matrix[0][1], matrix[0][2], matrix[0][3]
			, matrix[1][0], matrix[1][1], matrix[1][2], matrix[1][3]
			, matrix[2][0], matrix[2][1], matrix[2][2], matrix[2][3]
			, matrix[3][0], matrix[3][1], matrix[3][2], matrix[3][3] };

		return C3D_OK;
	}

#ifdef __cplusplus
}
#endif
