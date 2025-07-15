#include "CCastor3D/Castor3D.h"
#include "CCastor3D/Castor3DCommon.h"

#include <CastorUtils/Math/Quaternion.hpp>

#ifdef __cplusplus
extern "C"
{
#endif

	C3D_CAPIMETHODIMP c3dQuat_fromMatrix( C3DMat4 const * matrix, C3DQuat * result )
	{
		if ( !result || !matrix )
			return C3D_POINTER;

		*result = cc3d::convert( c3d::Quaternion::fromMatrix( cc3d::convert( *matrix ) ) );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dQuat_fromAxisAngle( C3DVec3 const * axis, float angle, C3DQuat * result )
	{
		if ( !result || !axis )
			return C3D_POINTER;

		*result = cc3d::convert( c3d::Quaternion::fromAxisAngle( cc3d::convert( *axis ), c3d::Angle::fromRadians( angle ) ) );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dQuat_fromAxes( C3DVec3 const * x, C3DVec3 const * y, C3DVec3 const * z, C3DQuat * result )
	{
		if ( !result || !x || !y || !z )
			return C3D_POINTER;

		*result = cc3d::convert( c3d::Quaternion::fromAxes( cc3d::convert( *x ), cc3d::convert( *y ), cc3d::convert( *z ) ) );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dQuat_getRotationMatrix( C3DQuat const * object, C3DMat4 * result )
	{
		if ( !object || !result )
			return C3D_POINTER;

		c3d::Matrix4x4f matrix;
		cc3d::convert( *object ).toMatrix( matrix );
		*result = cc3d::convert( matrix );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dQuat_getAxisAngle( C3DQuat const * object, C3DVec3 * axis,  float * angle )
	{
		if ( !object || !axis || !angle )
			return C3D_POINTER;

		c3d::Point3f caxis;
		c3d::Angle cangle;
		cc3d::convert( *object ).toAxisAngle( caxis, cangle );
		*axis = cc3d::convert( caxis );
		*angle = cangle.radians();

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dQuat_getAxes( C3DQuat const * object, C3DVec3 * x,  C3DVec3 * y,  C3DVec3 * z )
	{
		if ( !object || !x || !y || !z )
			return C3D_POINTER;

		c3d::Point3f cx;
		c3d::Point3f cy;
		c3d::Point3f cz;
		cc3d::convert( *object ).toAxes( cx, cy, cz );
		*x = cc3d::convert( cx );
		*y = cc3d::convert( cy );
		*z = cc3d::convert( cz );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dQuat_getPitch( C3DQuat const * object, float * result )
	{
		if ( !object || !result )
			return C3D_POINTER;

		if ( object->w == 0.0f )
		{
			*result = 0.0f;
		}
		else
		{
			C3DQuat pitch = *object;
			pitch.y = 0;
			pitch.z = 0;
			double pitchMag = sqrt( pitch.w * pitch.w + pitch.x * pitch.x );
			pitch.w /= float( pitchMag );
			pitch.x /= float( pitchMag );
			*result = float( 2.0f * acos( pitch.w ) );
		}

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dQuat_getYaw( C3DQuat const * object, float * result )
	{
		if ( !object || !result )
			return C3D_POINTER;

		if ( object->w == 0.0f )
		{
			*result = 0.0f;
		}
		else
		{
			C3DQuat yaw = *object;
			yaw.x = 0;
			yaw.z = 0;
			double yawMag = sqrt( yaw.w * yaw.w + yaw.y * yaw.y );
			yaw.w /= float( yawMag );
			yaw.y /= float( yawMag );
			*result = float( 2.0f * acos( yaw.w ) );
		}

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dQuat_getRoll( C3DQuat const * object, float * result )
	{
		if ( !object || !result )
			return C3D_POINTER;

		if ( object->w == 0.0f )
		{
			*result = 0.0f;
		}
		else
		{
			C3DQuat roll = *object;
			roll.x = 0;
			roll.y = 0;
			double rollMag = sqrt( roll.w * roll.w + roll.z * roll.z );
			roll.w /= float( rollMag );
			roll.z /= float( rollMag );
			*result = float( 2.0f * acos( roll.w ) );
		}

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dQuat_transform( C3DQuat const * object, C3DVec3 const * val, C3DVec3 * result )
	{
		if ( !object || !result )
			return C3D_POINTER;

		c3d::Point3f res;
		cc3d::convert( *object ).transform( c3d::Point3f{ val->x, val->y, val->z }, res );
		*result = cc3d::convert( res );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dQuat_getMagnitude( C3DQuat const * object, float * result )
	{
		if ( !object || !result )
			return C3D_POINTER;

		*result = float( cc3d::convert( *object ).getMagnitude() );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dQuat_conjugate( C3DQuat * object )
	{
		if ( !object )
			return C3D_POINTER;

		*object = cc3d::convert( cc3d::convert( *object ).getConjugate() );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dQuat_slerp( C3DQuat const * lhs, C3DQuat const * rhs, float percent, C3DQuat * result )
	{
		if ( !lhs || !rhs || !result )
			return C3D_POINTER;

		*result = cc3d::convert( cc3d::convert( *lhs ).slerp( cc3d::convert( *rhs ), percent ) );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dQuat_mix( C3DQuat const * lhs, C3DQuat const * rhs, float percent, C3DQuat * result )
	{
		if ( !lhs || !rhs || !result )
			return C3D_POINTER;

		*result = cc3d::convert( cc3d::convert( *lhs ).mix( cc3d::convert( *rhs ), percent ) );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dQuat_mul( C3DQuat const * lhs, C3DQuat const * rhs, C3DQuat * result )
	{
		if ( !lhs || !rhs || !result )
			return C3D_POINTER;

		*result = cc3d::convert( cc3d::convert( *lhs ) * cc3d::convert( *rhs ) );

		return C3D_OK;
	}

#ifdef __cplusplus
}
#endif
