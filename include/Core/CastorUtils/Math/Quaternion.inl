#include "CastorUtils/Math/TransformationMatrix.hpp"

namespace castor
{
	//*************************************************************************************************

	namespace
	{
		template< typename T >
		inline T mixValues( T a, T b, T f )
		{
			return a + ( f * ( b - a ) );
		}
	}

	//*************************************************************************************************

	template< typename T >
	QuaternionT< T >::QuaternionT( NoInit const & )
		: Coords4< T >( buffer )
	{
	}

	template< typename T >
	QuaternionT< T >::QuaternionT( T x, T y, T z, T w )
		: QuaternionT( NoInit() )
	{
		quat.x = x;
		quat.y = y;
		quat.z = z;
		quat.w = w;
		point::normalise( *this );
	}

	template< typename T >
	QuaternionT< T >::QuaternionT()
		: QuaternionT( 0, 0, 0, 1 )
	{
	}

	template< typename T >
	QuaternionT< T >::QuaternionT( QuaternionT< T > const & rhs )
		: QuaternionT( rhs.quat.x, rhs.quat.y, rhs.quat.z, rhs.quat.w )
	{
	}

	template< typename T >
	QuaternionT< T >::QuaternionT( QuaternionT< T > && rhs )noexcept
		: QuaternionT( NoInit() )
	{
		std::memmove( buffer, rhs.buffer, sizeof( buffer ) );
	}

	template< typename T >
	QuaternionT< T >::QuaternionT( double const * rhs )
		: QuaternionT( T( rhs[0] ), T( rhs[1] ), T( rhs[2] ), T( rhs[3] ) )
	{
	}

	template< typename T >
	QuaternionT< T >::QuaternionT( float const * rhs )
		: QuaternionT( T( rhs[0] ), T( rhs[1] ), T( rhs[2] ), T( rhs[3] ) )
	{
	}

	template< typename T >
	QuaternionT< T >::QuaternionT( Point4f const & rhs )
		: QuaternionT( T( rhs[0] ), T( rhs[1] ), T( rhs[2] ), T( rhs[3] ) )
	{
	}

	template< typename T >
	QuaternionT< T >::QuaternionT( Point4d const & rhs )
		: QuaternionT( T( rhs[0] ), T( rhs[1] ), T( rhs[2] ), T( rhs[3] ) )
	{
	}

	template< typename T >
	QuaternionT< T >::~QuaternionT()
	{
	}

	template< typename T >
	QuaternionT< T > & QuaternionT< T >::operator=( QuaternionT< T > const & rhs )
	{
		std::memcpy( buffer, rhs.buffer, sizeof( buffer ) );
		return *this;
	}

	template< typename T >
	QuaternionT< T > & QuaternionT< T >::operator=( QuaternionT< T > && rhs )noexcept
	{
		if ( this != &rhs )
		{
			std::memmove( buffer, rhs.buffer, sizeof( buffer ) );
		}

		return *this;
	}

	template< typename T >
	QuaternionT< T > & QuaternionT< T >::operator+=( QuaternionT< T > const & rhs )
	{
		quat.x += rhs.quat.x;
		quat.y += rhs.quat.y;
		quat.z += rhs.quat.z;
		quat.w += rhs.quat.w;
		point::normalise( *this );
		return *this;
	}

	template< typename T >
	QuaternionT< T > & QuaternionT< T >::operator-=( QuaternionT< T > const & rhs )
	{
		quat.x -= rhs.quat.x;
		quat.y -= rhs.quat.y;
		quat.z -= rhs.quat.z;
		quat.w -= rhs.quat.w;
		point::normalise( *this );
		return *this;
	}

	template< typename T >
	QuaternionT< T > & QuaternionT< T >::operator*=( QuaternionT< T > const & rhs )
	{
		double const x = quat.x;
		double const y = quat.y;
		double const z = quat.z;
		double const w = quat.w;
		quat.x = T( w * rhs.quat.x + x * rhs.quat.w + y * rhs.quat.z - z * rhs.quat.y );
		quat.y = T( w * rhs.quat.y + y * rhs.quat.w + z * rhs.quat.x - x * rhs.quat.z );
		quat.z = T( w * rhs.quat.z + z * rhs.quat.w + x * rhs.quat.y - y * rhs.quat.x );
		quat.w = T( w * rhs.quat.w - x * rhs.quat.x - y * rhs.quat.y - z * rhs.quat.z );
		point::normalise( *this );
		return *this;
	}

	template< typename T >
	QuaternionT< T > & QuaternionT< T >::operator*=( double rhs )
	{
		quat.x = T( quat.x * rhs );
		quat.y = T( quat.y * rhs );
		quat.z = T( quat.z * rhs );
		quat.w = T( quat.w * rhs );
		point::normalise( *this );
		return *this;
	}

	template< typename T >
	QuaternionT< T > & QuaternionT< T >::operator*=( float rhs )
	{
		quat.x = T( quat.x * rhs );
		quat.y = T( quat.y * rhs );
		quat.z = T( quat.z * rhs );
		quat.w = T( quat.w * rhs );
		point::normalise( *this );
		return *this;
	}

	template< typename T >
	QuaternionT< T > QuaternionT< T >::fromMatrix( float const * rhs )
	{
		return fromMatrix( Matrix4x4f( rhs ) );
	}

	template< typename T >
	QuaternionT< T > QuaternionT< T >::fromMatrix( double const * rhs )
	{
		return fromMatrix( Matrix4x4d( rhs ) );
	}

	template< typename T >
	QuaternionT< T > QuaternionT< T >::fromMatrix( Matrix4x4f const & rhs )
	{
		QuaternionT< T > result;
		matrix::getRotate( rhs, result );
		return result;
	}

	template< typename T >
	QuaternionT< T > QuaternionT< T >::fromMatrix( Matrix4x4d const & rhs )
	{
		QuaternionT< T > result;
		matrix::getRotate( rhs, result );
		return result;
	}

	template< typename T >
	QuaternionT< T > QuaternionT< T >::fromAxisAngle( Point3f const & axis, Angle const & angle )
	{
		QuaternionT< T > result;
		Angle halfAngle = angle * 0.5f;
		auto norm = point::getNormalised( axis ) * halfAngle.sin();
		result.quat.x = T( norm[0] );
		result.quat.y = T( norm[1] );
		result.quat.z = T( norm[2] );
		result.quat.w = T( halfAngle.cos() );
		point::normalise( result );
		return result;
	}

	template< typename T >
	QuaternionT< T > QuaternionT< T >::fromAxisAngle( Point3d const & axis, Angle const & angle )
	{
		QuaternionT< T > result;
		Angle halfAngle = angle * 0.5;
		auto norm = point::getNormalised( axis ) * double( halfAngle.sin() );
		result.quat.x = T( norm[0] );
		result.quat.y = T( norm[1] );
		result.quat.z = T( norm[2] );
		result.quat.w = T( halfAngle.cos() );
		point::normalise( result );
		return result;
	}

	template< typename T >
	QuaternionT< T > QuaternionT< T >::fromAxes( Point3f const & x, Point3f const & y, Point3f const & z )
	{
		Matrix4x4f mtxRot;
		mtxRot[0][0] = x[0];
		mtxRot[1][0] = x[1];
		mtxRot[2][0] = x[2];
		mtxRot[0][1] = y[0];
		mtxRot[1][1] = y[1];
		mtxRot[2][1] = y[2];
		mtxRot[0][2] = z[0];
		mtxRot[1][2] = z[1];
		mtxRot[2][2] = z[2];
		return fromMatrix( mtxRot );
	}

	template< typename T >
	QuaternionT< T > QuaternionT< T >::fromAxes( Point3d const & x, Point3d const & y, Point3d const & z )
	{
		Matrix4x4d mtxRot;
		mtxRot[0][0] = x[0];
		mtxRot[1][0] = x[1];
		mtxRot[2][0] = x[2];
		mtxRot[0][1] = y[0];
		mtxRot[1][1] = y[1];
		mtxRot[2][1] = y[2];
		mtxRot[0][2] = z[0];
		mtxRot[1][2] = z[1];
		mtxRot[2][2] = z[2];
		return fromMatrix( mtxRot );
	}

	template< typename T >
	template< Vector3fT PtT >
	PtT & QuaternionT< T >::transform( PtT const & vector, PtT & result )const
	{
		Point3d u( quat.x, quat.y, quat.z );
		Point3d uv( point::cross( u, vector ) );
		Point3d uuv( point::cross( u, uv ) );
		uv *= 2.0 * quat.w;
		uuv *= 2;
		point::setPoint( result, point::getPoint( vector ) + uv + uuv );
		return result;
	}

	template< typename T >
	template< Vector3dT PtT >
	PtT & QuaternionT< T >::transform( PtT const & vector, PtT & result )const
	{
		Point3d u( quat.x, quat.y, quat.z );
		Point3d uv( point::cross( u, vector ) );
		Point3d uuv( point::cross( u, uv ) );
		uv *= 2 * quat.w;
		uuv *= 2;
		point::setPoint( result, point::getPoint( vector ) + uv + uuv );
		return result;
	}

	template< typename T >
	void QuaternionT< T >::toMatrix( double * matrix )const
	{
		Matrix4x4d mtx = Matrix4x4d( matrix );
		toMatrix( mtx );
	}

	template< typename T >
	void QuaternionT< T >::toMatrix( float * matrix )const
	{
		Matrix4x4f mtx = Matrix4x4f( matrix );
		toMatrix( mtx );
	}

	template< typename T >
	void QuaternionT< T >::toMatrix( Matrix4x4d & matrix )const
	{
		matrix::setRotate( matrix, *this );
	}

	template< typename T >
	void QuaternionT< T >::toMatrix( Matrix4x4f & matrix )const
	{
		matrix::setRotate( matrix, *this );
	}

	template< typename T >
	void QuaternionT< T >::toAxisAngle( Point3f & axis, Angle & angle )const
	{
		T const x = quat.x;
		T const y = quat.y;
		T const z = quat.z;
		T const w = quat.w;
		auto s = T( sqrt( T{ 1.0 } - ( w * w ) ) );

		if ( std::abs( s ) < std::numeric_limits< T >::epsilon() )
		{
			// angle is 0 (mod 2*pi), so any axis will do
			angle = 0.0_radians;
			axis[0] = T{ 1 };
			axis[1] = T{ 0 };
			axis[2] = T{ 0 };
		}
		else
		{
			angle = castor::acosf( w );
			angle *= 2;
			axis[0] = x / s;
			axis[1] = y / s;
			axis[2] = z / s;
		}

		point::normalise( axis );
	}

	template< typename T >
	void QuaternionT< T >::toAxisAngle( Point3d & axis, Angle & angle )const
	{
		T const x = quat.x;
		T const y = quat.y;
		T const z = quat.z;
		T const w = quat.w;
		auto s = T( sqrt( T{ 1.0 } - ( w * w ) ) );

		if ( std::abs( s ) < std::numeric_limits< T >::epsilon() )
		{
			// angle is 0 (mod 2*pi), so any axis will do
			angle = 0.0_radians;
			axis[0] = T{ 1 };
			axis[1] = T{ 0 };
			axis[2] = T{ 0 };
		}
		else
		{
			angle = castor::acosf( w );
			angle *= 2;
			axis[0] = x / s;
			axis[1] = y / s;
			axis[2] = z / s;
		}

		point::normalise( axis );
	}

	template< typename T >
	void QuaternionT< T >::toAxes( Point3f & x, Point3f & y, Point3f & z )const
	{
		Matrix4x4f mtxRot;
		toMatrix( mtxRot );
		x[0] = mtxRot[0][0];
		x[1] = mtxRot[1][0];
		x[2] = mtxRot[2][0];
		y[0] = mtxRot[0][1];
		y[1] = mtxRot[1][1];
		y[2] = mtxRot[2][1];
		z[0] = mtxRot[0][2];
		z[1] = mtxRot[1][2];
		z[2] = mtxRot[2][2];
	}

	template< typename T >
	void QuaternionT< T >::toAxes( Point3d & x, Point3d & y, Point3d & z )const
	{
		Matrix4x4d mtxRot;
		toMatrix( mtxRot );
		x[0] = mtxRot[0][0];
		x[1] = mtxRot[1][0];
		x[2] = mtxRot[2][0];
		y[0] = mtxRot[0][1];
		y[1] = mtxRot[1][1];
		y[2] = mtxRot[2][1];
		z[0] = mtxRot[0][2];
		z[1] = mtxRot[1][2];
		z[2] = mtxRot[2][2];
	}

	template< typename T >
	void QuaternionT< T >::conjugate()
	{
		quat.x = -quat.x;
		quat.y = -quat.y;
		quat.z = -quat.z;
	}

	template< typename T >
	QuaternionT< T > QuaternionT< T >::getConjugate()const
	{
		QuaternionT< T > result( *this );
		result.conjugate();
		return result;
	}

	template< typename T >
	double QuaternionT< T >::getMagnitude()const
	{
		return point::length( *this );
	}

	template< typename T >
	inline AngleT< T > QuaternionT< T >::getPitch()const
	{
		auto pitch = *this;
		pitch[1] = 0;
		pitch[2] = 0;
		double pitchMag = sqrt( pitch[3] * pitch[3] + pitch[0] * pitch[0] );
		pitch[3] /= T( pitchMag );
		pitch[0] /= T( pitchMag );
		return Angle::fromRadians( 2 * acos( pitch[3] ) );
	}

	template< typename T >
	inline AngleT< T > QuaternionT< T >::getYaw()const
	{
		auto yaw = *this;
		yaw[0] = 0;
		yaw[2] = 0;
		double yawMag = sqrt( yaw[3] * yaw[3] + yaw[1] * yaw[1] );
		yaw[3] /= T( yawMag );
		yaw[1] /= T( yawMag );
		return Angle::fromRadians( 2 * acos( yaw[3] ) );
	}

	template< typename T >
	inline AngleT< T > QuaternionT< T >::getRoll()const
	{
		auto roll = *this;
		roll[0] = 0;
		roll[1] = 0;
		double rollMag = sqrt( roll[3] * roll[3] + roll[2] * roll[2] );
		roll[3] /= T( rollMag );
		roll[2] /= T( rollMag );
		return Angle::fromRadians( 2 * acos( roll[3] ) );
	}

	template< typename T >
	QuaternionT< T > QuaternionT< T >::mix( QuaternionT< T > const & target, double factor )const
	{
		T cosTheta = point::dot( *this, target );

		// Perform a linear interpolation when cosTheta is close to 1 to avoid side effect of sin(angle) becoming a zero denominator
		if ( cosTheta > 1 - std::numeric_limits< T >::epsilon() )
		{
			// Linear interpolation
			return QuaternionT< T >(
				mixValues( quat.x, target.quat.x, T( factor ) ),
				mixValues( quat.y, target.quat.y, T( factor ) ),
				mixValues( quat.z, target.quat.z, T( factor ) ),
				mixValues( quat.w, target.quat.w, T( factor ) ) );
		}
		else
		{
			// Essential Mathematics, page 467
			T angle = acos( cosTheta );
			return ( sin( ( 1.0 - factor ) * angle ) * ( *this ) + sin( factor * angle ) * target ) / T( sin( angle ) );
		}
	}

	template< typename T >
	QuaternionT< T > QuaternionT< T >::mix( QuaternionT< T > const & target, float factor )const
	{
		T cosTheta = point::dot( *this, target );

		// Perform a linear interpolation when cosTheta is close to 1 to avoid side effect of sin(angle) becoming a zero denominator
		if ( cosTheta > 1 - std::numeric_limits< T >::epsilon() )
		{
			// Linear interpolation
			return QuaternionT< T >(
				mixValues( quat.x, target.quat.x, T( factor ) ),
				mixValues( quat.y, target.quat.y, T( factor ) ),
				mixValues( quat.z, target.quat.z, T( factor ) ),
				mixValues( quat.w, target.quat.w, T( factor ) ) );
		}
		else
		{
			// Essential Mathematics, page 467
			T angle = acos( cosTheta );
			return QuaternionT{ ( sin( ( 1.0 - factor ) * angle ) * ( *this ) + sin( factor * angle ) * target ) / T( sin( angle ) ) };
		}
	}

	template< typename T >
	QuaternionT< T > QuaternionT< T >::lerp( QuaternionT< T > const & target, double factor )const
	{
		// Lerp is only defined in [0, 1]
		assert( factor >= 0 );
		assert( factor <= 1 );

		return ( *this ) * ( 1.0 - factor ) + ( target * factor );
	}

	template< typename T >
	QuaternionT< T > QuaternionT< T >::lerp( QuaternionT< T > const & target, float factor )const
	{
		// Lerp is only defined in [0, 1]
		assert( factor >= 0 );
		assert( factor <= 1 );

		return ( *this ) * ( 1.0 - factor ) + ( target * factor );
	}

	template< typename T >
	QuaternionT< T > QuaternionT< T >::slerp( QuaternionT< T > const & target, double factor )const
	{
		//	Slerp = q1((q1^-1)q2)^t;
		T cosTheta = point::dot( *this, target );
		QuaternionT< T > result( target );

		// do we need to invert rotation?
		if ( cosTheta < 0 )
		{
			cosTheta = -cosTheta;
			result.quat.x = -result.quat.x;
			result.quat.y = -result.quat.y;
			result.quat.z = -result.quat.z;
			result.quat.w = -result.quat.w;
		}

		// Calculate coefficients
		T sclp, sclq;

		if ( ( T( 1.0 ) - cosTheta ) > 0.0001 ) // 0.0001 -> some epsillon
		{
			// Standard case (slerp)
			T omega, sinom;
			omega = acos( cosTheta ); // extract theta from dot product's cos theta
			sinom = sin( omega );
			sclp = T( sin( ( 1.0 - factor ) * omega ) / sinom );
			sclq = T( sin( factor * omega ) / sinom );
		}
		else
		{
			// Very close, do linear interp (because it's faster)
			sclp = T( 1.0 - factor );
			sclq = T( factor );
		}

		result.quat.x = sclp * quat.x + sclq * result.quat.x;
		result.quat.y = sclp * quat.y + sclq * result.quat.y;
		result.quat.z = sclp * quat.z + sclq * result.quat.z;
		result.quat.w = sclp * quat.w + sclq * result.quat.w;
		return result;
	}

	template< typename T >
	QuaternionT< T > QuaternionT< T >::slerp( QuaternionT< T > const & target, float factor )const
	{
		//	Slerp = q1((q1^-1)q2)^t;
		T cosTheta = point::dot( *this, target );
		QuaternionT< T > result( target );

		// do we need to invert rotation?
		if ( cosTheta < 0 )
		{
			cosTheta = -cosTheta;
			result.quat.x = -result.quat.x;
			result.quat.y = -result.quat.y;
			result.quat.z = -result.quat.z;
			result.quat.w = -result.quat.w;
		}

		// Calculate coefficients
		T sclp, sclq;

		if ( ( T( 1.0 ) - cosTheta ) > 0.0001 ) // 0.0001 -> some epsillon
		{
			// Standard case (slerp)
			auto omega = T( acos( cosTheta ) ); // extract theta from dot product's cos theta
			auto sinom = T( sin( omega ) );
			sclp = T( sin( ( 1.0 - factor ) * omega ) / sinom );
			sclq = T( sin( factor * omega ) / sinom );
		}
		else
		{
			// Very close, do linear interp (because it's faster)
			sclp = T( 1.0 - factor );
			sclq = T( factor );
		}

		result.quat.x = sclp * quat.x + sclq * result.quat.x;
		result.quat.y = sclp * quat.y + sclq * result.quat.y;
		result.quat.z = sclp * quat.z + sclq * result.quat.z;
		result.quat.w = sclp * quat.w + sclq * result.quat.w;
		return result;
	}

	template< typename T >
	QuaternionT< T > QuaternionT< T >::identity()
	{
		return QuaternionT< T >( 0, 0, 0, 1 );
	}

	template< typename T >
	QuaternionT< T > QuaternionT< T >::null()
	{
		return QuaternionT< T >( 0, 0, 0, 0 );
	}

	template< typename T >
	QuaternionT< T > operator+( QuaternionT< T > const & lhs, QuaternionT< T > const & rhs )
	{
		QuaternionT< T > result( lhs );
		result += rhs;
		return result;
	}

	template< typename T >
	QuaternionT< T > operator-( QuaternionT< T > const & lhs, QuaternionT< T > const & rhs )
	{
		QuaternionT< T > result( lhs );
		result -= rhs;
		return result;
	}

	template< typename T >
	QuaternionT< T > operator*( QuaternionT< T > const & lhs, QuaternionT< T > const & rhs )
	{
		QuaternionT< T > result( lhs );
		result *= rhs;
		return result;
	}

	template< typename T >
	QuaternionT< T > operator*( QuaternionT< T > const & lhs, double rhs )
	{
		QuaternionT< T > result( lhs );
		result *= rhs;
		return result;
	}

	template< typename T >
	QuaternionT< T > operator*( QuaternionT< T > const & lhs, float rhs )
	{
		QuaternionT< T > result( lhs );
		result *= rhs;
		return result;
	}

	template< typename T >
	QuaternionT< T > operator*( double lhs, QuaternionT< T > const & rhs )
	{
		QuaternionT< T > result( rhs );
		result *= lhs;
		return result;
	}

	template< typename T >
	QuaternionT< T > operator*( float lhs, QuaternionT< T > const & rhs )
	{
		QuaternionT< T > result( rhs );
		result *= lhs;
		return result;
	}

	template< typename T >
	QuaternionT< T > operator-( QuaternionT< T > const & rhs )
	{
		QuaternionT< T > result( rhs );
		result.w = -result.w;
		return result;
	}
}
