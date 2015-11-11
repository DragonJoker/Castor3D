#include "Point.hpp"
#include "Angle.hpp"
#include "TransformationMatrix.hpp"

namespace Castor
{
	namespace
	{
		template< typename T, typename U >
		inline void QuaternionToRotationMatrix( QuaternionT< T > const & p_quat, SquareMatrix< U, 4 > & p_matrix )
		{
			matrix::set_rotate( p_matrix, p_quat );
		}
		template< typename T, typename U >
		inline void RotationMatrixToQuaternion( QuaternionT< T > & p_quat, SquareMatrix< U, 4 > const & p_matrix )
		{
			double l_dTrace = double( p_matrix[0][0] + p_matrix[1][1] + p_matrix[2][2] );
			double l_dRoot;

			if ( l_dTrace > 0 )
			{
				// |w| > 1/2, may as well choose w > 1/2
				l_dRoot = std::sqrt( l_dTrace + 1 );  // 2w
				p_quat.w = T( 0.5 * l_dRoot );
				l_dRoot = 0.5 / l_dRoot;  // 1/(4w)
				p_quat.x = T( ( p_matrix[2][1] - p_matrix[1][2] ) * l_dRoot );
				p_quat.y = T( ( p_matrix[0][2] - p_matrix[2][0] ) * l_dRoot );
				p_quat.z = T( ( p_matrix[1][0] - p_matrix[0][1] ) * l_dRoot );
			}
			else
			{
				// |w| <= 1/2
				static uint32_t s_iNext[3] = { 1, 2, 0 };
				uint32_t i = 0;

				if ( p_matrix[1][1] > p_matrix[0][0] )
				{
					i = 1;
				}

				if ( p_matrix[2][2] > p_matrix[i][i] )
				{
					i = 2;
				}

				uint32_t j = s_iNext[i];
				uint32_t k = s_iNext[j];
				l_dRoot = std::sqrt( double( p_matrix[i][i] - p_matrix[j][j] - p_matrix[k][k] + 1 ) );
				T * l_apkQuat[3] = { &p_quat.x, &p_quat.y, &p_quat.z };
				*l_apkQuat[i] = 0.5 * l_dRoot;
				l_dRoot = 0.5 / l_dRoot;
				*l_apkQuat[j] = double( p_matrix[j][i] + p_matrix[i][j] ) * l_dRoot;
				*l_apkQuat[k] = double( p_matrix[k][i] + p_matrix[i][k] ) * l_dRoot;
				p_quat.w = T( double( p_matrix[k][j] - p_matrix[j][k] ) * l_dRoot );
			}

			point::normalise( p_quat );
		}

#if defined( CASTOR_USE_RSQRT )

		float rsqrt( float number )
		{
			long i;
			float x2, y;
			const float threehalfs = 1.5F;
			x2 = number * 0.5F;
			y = number;
			i = *( long * )&y;                       // evil floating point bit level hacking
			i = 0x5f3759df - ( i >> 1 );               // what the fuck?
			y = *( float * )&i;
			y = y * ( threehalfs - ( x2 * y * y ) );   // 1st iteration
			y = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed
			return y;
		}

#else

		inline float rsqrt( float number )
		{
			return 1 / sqrt( number );
		}

#endif

		inline double mix( double p_a, double p_b, double p_f )
		{
			return p_a + ( p_f * ( p_b - p_a ) );
		}

		inline float mix( float p_a, float p_b, float p_f )
		{
			return p_a + ( p_f * ( p_b - p_a ) );
		}
	}

	template< typename T >
	QuaternionT< T >::QuaternionT( NoInit const & )
		: Coords4< T >( buffer )
	{
	}

	template< typename T >
	QuaternionT< T >::QuaternionT( T p_x, T p_y, T p_z, T p_w )
		: QuaternionT( NoInit() )
	{
		x = p_x;
		y = p_y;
		z = p_z;
		w = p_w;
		point::normalise( *this );
	}

	template< typename T >
	QuaternionT< T >::QuaternionT()
		: QuaternionT( 0, 0, 0, 1 )
	{
	}

	template< typename T >
	QuaternionT< T >::QuaternionT( QuaternionT< T > const & p_q )
		: QuaternionT( p_q.x, p_q.y, p_q.z, p_q.w )
	{
	}

	template< typename T >
	QuaternionT< T >::QuaternionT( QuaternionT< T > && p_q )
		: QuaternionT( NoInit() )
	{
		std::memmove( buffer, p_q.buffer, sizeof( buffer ) );
	}

	template< typename T >
	QuaternionT< T >::QuaternionT( double const * p_q )
		: QuaternionT( p_q[0], p_q[1], p_q[2], p_q[3] )
	{
	}

	template< typename T >
	QuaternionT< T >::QuaternionT( float const * p_q )
		: QuaternionT( p_q[0], p_q[1], p_q[2], p_q[3] )
	{
	}

	template< typename T >
	QuaternionT< T >::QuaternionT( Point4f const & p_ptValues )
		: QuaternionT( p_ptValues[0], p_ptValues[1], p_ptValues[2], p_ptValues[3] )
	{
	}

	template< typename T >
	QuaternionT< T >::QuaternionT( Point4d const & p_ptValues )
		: QuaternionT( p_ptValues[0], p_ptValues[1], p_ptValues[2], p_ptValues[3] )
	{
	}

	template< typename T >
	QuaternionT< T >::QuaternionT( Point3f const & p_vector, Angle const & p_angle )
		: QuaternionT( NoInit() )
	{
		FromAxisAngle( p_vector, p_angle );
	}

	template< typename T >
	QuaternionT< T >::QuaternionT( Point3d const & p_vector, Angle const & p_angle )
		: QuaternionT( NoInit() )
	{
		FromAxisAngle( p_vector, p_angle );
	}

	template< typename T >
	QuaternionT< T >::QuaternionT( Angle const & p_yaw, Angle const & p_pitch, Angle const & p_roll )
		: QuaternionT( NoInit() )
	{
		FromEulerAngles( p_yaw, p_pitch, p_roll );
	}

	template< typename T >
	QuaternionT< T >::~QuaternionT()
	{
	}

	template< typename T >
	QuaternionT< T > & QuaternionT< T >::operator=( QuaternionT< T > const & p_q )
	{
		std::memcpy( buffer, p_q.buffer, sizeof( buffer ) );
		return *this;
	}

	template< typename T >
	QuaternionT< T > & QuaternionT< T >::operator=( QuaternionT< T > && p_q )
	{
		if ( this != &p_q )
		{
			std::memmove( buffer, p_q.buffer, sizeof( buffer ) );
		}

		return *this;
	}

	template< typename T >
	QuaternionT< T > & QuaternionT< T >::operator+=( QuaternionT< T > const & p_q )
	{
		x += p_q.x;
		y += p_q.y;
		z += p_q.z;
		w += p_q.w;
		point::normalise( *this );
		return *this;
	}

	template< typename T >
	QuaternionT< T > & QuaternionT< T >::operator-=( QuaternionT< T > const & p_q )
	{
		x -= p_q.x;
		y -= p_q.y;
		z -= p_q.z;
		w -= p_q.w;
		point::normalise( *this );
		return *this;
	}

	template< typename T >
	QuaternionT< T > & QuaternionT< T >::operator*=( QuaternionT< T > const & p_q )
	{
		double const l_x = x;
		double const l_y = y;
		double const l_z = z;
		double const l_w = w;
		double const l_qx = p_q.x;
		double const l_qy = p_q.y;
		double const l_qz = p_q.z;
		double const l_qw = p_q.w;
		x = T( l_w * l_qx + l_x * l_qw + l_y * l_qz - l_z * l_qy );
		y = T( l_w * l_qy + l_y * l_qw + l_z * l_qx - l_x * l_qz );
		z = T( l_w * l_qz + l_z * l_qw + l_x * l_qy - l_y * l_qx );
		w = T( l_w * l_qw - l_x * l_qx - l_y * l_qy - l_z * l_qz );
		point::normalise( *this );
		return *this;
	}

	template< typename T >
	QuaternionT< T > & QuaternionT< T >::operator*=( double p_rScalar )
	{
		x = T( x * p_rScalar );
		y = T( y * p_rScalar );
		z = T( z * p_rScalar );
		w = T( w * p_rScalar );
		point::normalise( *this );
		return *this;
	}

	template< typename T >
	QuaternionT< T > & QuaternionT< T >::operator*=( float p_rScalar )
	{
		x = T( x * p_rScalar );
		y = T( y * p_rScalar );
		z = T( z * p_rScalar );
		w = T( w * p_rScalar );
		point::normalise( *this );
		return *this;
	}

	template< typename T >
	Point3f & QuaternionT< T >::Transform( Point3f const & p_vector, Point3f & p_ptResult )const
	{
		Point3d u( x, y, z );
		Point3d uv( u ^ p_vector );
		Point3d uuv( u ^ uv );
		uv *= 2 * w;
		uuv *= 2;
		p_ptResult = p_vector + uv + uuv;
		return p_ptResult;
	}

	template< typename T >
	Point3d & QuaternionT< T >::Transform( Point3d const & p_vector, Point3d & p_ptResult )const
	{
		Point3d u( x, y, z );
		Point3d uv( u ^ p_vector );
		Point3d uuv( u ^ uv );
		uv *= 2 * w;
		uuv *= 2;
		p_ptResult = p_vector + uv + uuv;
		return p_ptResult;
	}

	template< typename T >
	void QuaternionT< T >::ToRotationMatrix( double * p_matrix )const
	{
		Matrix4x4d l_mtx = Matrix4x4d( p_matrix );
		QuaternionToRotationMatrix( *this, l_mtx );
	}

	template< typename T >
	void QuaternionT< T >::ToRotationMatrix( float * p_matrix )const
	{
		Matrix4x4f l_mtx = Matrix4x4f( p_matrix );
		QuaternionToRotationMatrix( *this, l_mtx );
	}

	template< typename T >
	void QuaternionT< T >::FromRotationMatrix( Matrix4x4f const & p_matrix )
	{
		RotationMatrixToQuaternion( *this, p_matrix );
	}

	template< typename T >
	void QuaternionT< T >::FromRotationMatrix( Matrix4x4d const & p_matrix )
	{
		RotationMatrixToQuaternion( *this, p_matrix );
	}

	template< typename T >
	void QuaternionT< T >::FromAxisAngle( Point3f const & p_vector, Angle const & p_angle )
	{
		Angle l_halfAngle = p_angle * 0.5f;
		Point3f l_norm = point::get_normalised( p_vector ) * l_halfAngle.Sin();
		x = T( l_norm[0] );
		y = T( l_norm[1] );
		z = T( l_norm[2] );
		w = T( l_halfAngle.Cos() );
		point::normalise( *this );
	}

	template< typename T >
	void QuaternionT< T >::FromAxisAngle( Point3d const & p_vector, Angle const & p_angle )
	{
		Angle l_halfAngle = p_angle * 0.5;
		Point3d l_norm = point::get_normalised( p_vector ) * l_halfAngle.Sin();
		x = T( l_norm[0] );
		y = T( l_norm[1] );
		z = T( l_norm[2] );
		w = T( l_halfAngle.Cos() );
		point::normalise( *this );
	}

	template< typename T >
	void QuaternionT< T >::ToAxisAngle( Point3f & p_vector, Angle & p_angle )const
	{
		double const x = this->x;
		double const y = this->y;
		double const z = this->z;
		double const w = this->w;
		double l_rSqrLength = x * x + y * y + z * z;

		if ( l_rSqrLength > 0.0 )
		{
			p_angle = Angle::FromRadians( 2.0 * acos( w ) );
			double l_rSin = p_angle.Sin();
			p_vector[0] = float( x / l_rSin );
			p_vector[1] = float( y / l_rSin );
			p_vector[2] = float( z / l_rSin );
		}
		else
		{
			// angle is 0 (mod 2*pi), so any axis will do
			p_angle = Angle::FromRadians( 0.0 );
			p_vector[0] = 1.0f;
			p_vector[1] = 0.0f;
			p_vector[2] = 0.0f;
		}

		point::normalise( p_vector );
	}

	template< typename T >
	void QuaternionT< T >::ToAxisAngle( Point3d & p_vector, Angle & p_angle )const
	{
		double const x = this->x;
		double const y = this->y;
		double const z = this->z;
		double const w = this->w;
		double l_rSqrLength = x * x + y * y + z * z;

		if ( l_rSqrLength > 0.0 )
		{
			p_angle = Angle::FromRadians( 2.0 * acos( w ) );
			double l_rSin = p_angle.Sin();
			p_vector[0] = x / l_rSin;
			p_vector[1] = y / l_rSin;
			p_vector[2] = z / l_rSin;
		}
		else
		{
			// angle is 0 (mod 2*pi), so any axis will do
			p_angle = Angle::FromRadians( 0.0 );
			p_vector[0] = 1.0;
			p_vector[1] = 0.0;
			p_vector[2] = 0.0;
		}

		point::normalise( p_vector );
	}

	template< typename T >
	void QuaternionT< T >::FromAxes( Point3f const & p_x, Point3f const & p_y, Point3f const & p_z )
	{
		Matrix4x4f l_mtxRot;
		l_mtxRot[0][0] = p_x[0];
		l_mtxRot[1][0] = p_x[1];
		l_mtxRot[2][0] = p_x[2];
		l_mtxRot[0][1] = p_y[0];
		l_mtxRot[1][1] = p_y[1];
		l_mtxRot[2][1] = p_y[2];
		l_mtxRot[0][2] = p_z[0];
		l_mtxRot[1][2] = p_z[1];
		l_mtxRot[2][2] = p_z[2];
		FromRotationMatrix( l_mtxRot );
	}

	template< typename T >
	void QuaternionT< T >::FromAxes( Point3d const & p_x, Point3d const & p_y, Point3d const & p_z )
	{
		Matrix4x4d l_mtxRot;
		l_mtxRot[0][0] = p_x[0];
		l_mtxRot[1][0] = p_x[1];
		l_mtxRot[2][0] = p_x[2];
		l_mtxRot[0][1] = p_y[0];
		l_mtxRot[1][1] = p_y[1];
		l_mtxRot[2][1] = p_y[2];
		l_mtxRot[0][2] = p_z[0];
		l_mtxRot[1][2] = p_z[1];
		l_mtxRot[2][2] = p_z[2];
		FromRotationMatrix( l_mtxRot );
	}

	template< typename T >
	void QuaternionT< T >::ToAxes( Point3f & p_x, Point3f & p_y, Point3f & p_z )const
	{
		Matrix4x4f l_mtxRot;
		ToRotationMatrix( l_mtxRot );
		p_x[0] = l_mtxRot[0][0];
		p_x[1] = l_mtxRot[1][0];
		p_x[2] = l_mtxRot[2][0];
		p_y[0] = l_mtxRot[0][1];
		p_y[1] = l_mtxRot[1][1];
		p_y[2] = l_mtxRot[2][1];
		p_z[0] = l_mtxRot[0][2];
		p_z[1] = l_mtxRot[1][2];
		p_z[2] = l_mtxRot[2][2];
	}

	template< typename T >
	void QuaternionT< T >::ToAxes( Point3d & p_x, Point3d & p_y, Point3d & p_z )const
	{
		Matrix4x4d l_mtxRot;
		ToRotationMatrix( l_mtxRot );
		p_x[0] = l_mtxRot[0][0];
		p_x[1] = l_mtxRot[1][0];
		p_x[2] = l_mtxRot[2][0];
		p_y[0] = l_mtxRot[0][1];
		p_y[1] = l_mtxRot[1][1];
		p_y[2] = l_mtxRot[2][1];
		p_z[0] = l_mtxRot[0][2];
		p_z[1] = l_mtxRot[1][2];
		p_z[2] = l_mtxRot[2][2];
	}

	template< typename T >
	void QuaternionT< T >::FromEulerAngles( Angle const & p_pitch, Angle const & p_yaw, Angle const & p_roll )
	{
		T l_c2 = cos( T( p_yaw.Radians() * 0.5 ) );
		T l_s2 = sin( T( p_yaw.Radians() * 0.5 ) );
		T l_c3 = cos( T( p_roll.Radians() * 0.5 ) );
		T l_s3 = sin( T( p_roll.Radians() * 0.5 ) );
		T l_c1 = cos( T( p_pitch.Radians() * 0.5 ) );
		T l_s1 = sin( T( p_pitch.Radians() * 0.5 ) );
		x = T( l_s2 * l_s3 * l_c1 ) + T( l_c2 * l_c3 * l_s1 );
		y = T( l_s2 * l_c3 * l_c1 ) + T( l_c2 * l_s3 * l_s1 );
		z = T( l_c2 * l_s3 * l_c1 ) - T( l_s2 * l_c3 * l_s1 );
		w = T( l_c2 * l_c3 * l_c1 ) - T( l_s2 * l_s3 * l_s1 );
		point::normalise( *this );
	}

	template< typename T >
	void QuaternionT< T >::ToEulerAngles( Angle & p_pitch, Angle & p_yaw, Angle & p_roll )
	{
		p_yaw = GetYaw();
		p_pitch = GetPitch();
		p_roll = GetRoll();
	}

	template< typename T >
	Angle QuaternionT< T >::GetYaw()const
	{
		return Angle::FromRadians( 2 * acos( w ) );// Angle::FromRadians( asin( T( -2.0 ) * ( x * z - w * y ) ) );
	}

	template< typename T >
	Angle QuaternionT< T >::GetPitch()const
	{
		T l_res1 = T( 2.0 ) * ( y * z + w * x );
		T l_res2 = w * w - x * x - y * y + z * z;
		return Angle::FromRadians( atan2( l_res1, l_res2 ) );
	}

	template< typename T >
	Angle QuaternionT< T >::GetRoll()const
	{
		T l_res1 = T( 2.0 ) * ( x * y + w * z );
		T l_res2 = w * w + x * x - y * y - z * z;
		return Angle::FromRadians( atan2( l_res1, l_res2 ) );
	}

	template< typename T >
	void QuaternionT< T >::Conjugate()
	{
		T w = m_data.quaternion.w;
		point::negate( *this );
		m_data.quaternion.w = w;
	}

	template< typename T >
	QuaternionT< T > QuaternionT< T >::GetConjugate()const
	{
		QuaternionT< T > l_return( *this );
		l_return.Conjugate();
		return l_return;
	}

	template< typename T >
	double QuaternionT< T >::GetMagnitude()const
	{
		return point::distance( *this );
	}

	template< typename T >
	QuaternionT< T > QuaternionT< T >::Mix( QuaternionT< T > const & p_target, double p_factor )const
	{
		T l_cosTheta = point::dot( *this, p_target );

		// Perform a linear interpolation when cosTheta is close to 1 to avoid side effect of sin(angle) becoming a zero denominator
		if ( l_cosTheta > 1 - std::numeric_limits< T >::epsilon() )
		{
			// Linear interpolation
			return QuaternionT< T >(
					   mix( x, p_target.x, T( p_factor ) ),
					   mix( y, p_target.y, T( p_factor ) ),
					   mix( z, p_target.z, T( p_factor ) ),
					   mix( w, p_target.w, T( p_factor ) ) );
		}
		else
		{
			// Essential Mathematics, page 467
			T l_angle = acos( l_cosTheta );
			return ( sin( ( 1.0 - p_factor ) * l_angle ) * ( *this ) + sin( p_factor * l_angle ) * p_target ) / sin( l_angle );
		}
	}

	template< typename T >
	QuaternionT< T > QuaternionT< T >::Mix( QuaternionT< T > const & p_target, float p_factor )const
	{
		T l_cosTheta = point::dot( *this, p_target );

		// Perform a linear interpolation when cosTheta is close to 1 to avoid side effect of sin(angle) becoming a zero denominator
		if ( l_cosTheta > 1 - std::numeric_limits< T >::epsilon() )
		{
			// Linear interpolation
			return QuaternionT< T >(
					   mix( x, p_target.x, T( p_factor ) ),
					   mix( y, p_target.y, T( p_factor ) ),
					   mix( z, p_target.z, T( p_factor ) ),
					   mix( w, p_target.w, T( p_factor ) ) );
		}
		else
		{
			// Essential Mathematics, page 467
			T l_angle = acos( l_cosTheta );
			return ( sin( ( 1.0 - p_factor ) * l_angle ) * ( *this ) + sin( p_factor * l_angle ) * p_target ) / sin( l_angle );
		}
	}

	template< typename T >
	QuaternionT< T > QuaternionT< T >::Lerp( QuaternionT< T > const & p_target, double p_factor )const
	{
		// Lerp is only defined in [0, 1]
		assert( p_factor >= 0 );
		assert( p_factor <= 1 );

		return ( *this ) * ( 1.0 - p_factor ) + ( p_target * p_factor );
	}

	template< typename T >
	QuaternionT< T > QuaternionT< T >::Lerp( QuaternionT< T > const & p_target, float p_factor )const
	{
		// Lerp is only defined in [0, 1]
		assert( p_factor >= 0 );
		assert( p_factor <= 1 );

		return ( *this ) * ( 1.0 - p_factor ) + ( p_target * p_factor );
	}

	template< typename T >
	QuaternionT< T > QuaternionT< T >::Slerp( QuaternionT< T > const & p_target, double p_factor )const
	{
		//	Slerp = q1((q1^-1)q2)^t;
		T l_cosTheta = point::dot( *this, p_target );
		QuaternionT< T > l_target( p_target );

		// Do we need to invert rotation?
		if ( l_cosTheta < 0 )
		{
			l_cosTheta = -l_cosTheta;
			l_target = -p_target;
		}

		// Perform a linear interpolation when cosTheta is close to 1 to avoid side effect of sin(angle) becoming a zero denominator
		if ( l_cosTheta > 1 - std::numeric_limits< T >::epsilon() )
		{
			// Linear interpolation
			return QuaternionT< T >(
					   mix( x, p_target.x, T( p_factor ) ),
					   mix( y, p_target.y, T( p_factor ) ),
					   mix( z, p_target.z, T( p_factor ) ),
					   mix( w, p_target.w, T( p_factor ) ) );
		}
		else
		{
			// Essential Mathematics, page 467
			T l_angle = acos( l_cosTheta );
			return ( sin( ( 1.0 - p_factor ) * l_angle ) * ( *this ) + sin( p_factor * l_angle ) * p_target ) / sin( l_angle );
		}
	}

	template< typename T >
	QuaternionT< T > QuaternionT< T >::Slerp( QuaternionT< T > const & p_target, float p_factor )const
	{
		//	Slerp = q1((q1^-1)q2)^t;
		T l_cosTheta = point::dot( *this, p_target );
		QuaternionT< T > l_target( p_target );

		// Do we need to invert rotation?
		if ( l_cosTheta < 0 )
		{
			l_cosTheta = -l_cosTheta;
			l_target = -p_target;
		}

		// Perform a linear interpolation when cosTheta is close to 1 to avoid side effect of sin(angle) becoming a zero denominator
		if ( l_cosTheta > 1 - std::numeric_limits< T >::epsilon() )
		{
			// Linear interpolation
			return QuaternionT< T >(
					   mix( x, p_target.x, T( p_factor ) ),
					   mix( y, p_target.y, T( p_factor ) ),
					   mix( z, p_target.z, T( p_factor ) ),
					   mix( w, p_target.w, T( p_factor ) ) );
		}
		else
		{
			// Essential Mathematics, page 467
			T l_angle = acos( l_cosTheta );
			return ( sin( ( 1.0 - p_factor ) * l_angle ) * ( *this ) + sin( p_factor * l_angle ) * p_target ) / sin( l_angle );
		}
	}

	template< typename T >
	QuaternionT< T > QuaternionT< T >::Identity()
	{
		return QuaternionT< T >( 0, 0, 0, 1 );
	}

	template< typename T >
	QuaternionT< T > QuaternionT< T >::Null()
	{
		return QuaternionT< T >( 0, 0, 0, 0 );
	}

	template< typename T >
	QuaternionT< T > operator+( QuaternionT< T > const & p_qA, QuaternionT< T > const & p_qB )
	{
		QuaternionT< T > l_return( p_qA );
		l_return += p_qB;
		return l_return;
	}

	template< typename T >
	QuaternionT< T > operator-( QuaternionT< T > const & p_qA, QuaternionT< T > const & p_qB )
	{
		QuaternionT< T > l_return( p_qA );
		l_return -= p_qB;
		return l_return;
	}

	template< typename T >
	QuaternionT< T > operator*( QuaternionT< T > const & p_qA, QuaternionT< T > const & p_qB )
	{
		QuaternionT< T > l_return( p_qA );
		l_return *= p_qB;
		return l_return;
	}

	template< typename T >
	QuaternionT< T > operator*( QuaternionT< T > const & p_q, double p_rScalar )
	{
		QuaternionT< T > l_return( p_q );
		l_return *= p_rScalar;
		return l_return;
	}

	template< typename T >
	QuaternionT< T > operator*( QuaternionT< T > const & p_q, float p_rScalar )
	{
		QuaternionT< T > l_return( p_q );
		l_return *= p_rScalar;
		return l_return;
	}

	template< typename T >
	QuaternionT< T > operator*( double p_rScalar, QuaternionT< T > const & p_q )
	{
		QuaternionT< T > l_return( p_q );
		l_return *= p_rScalar;
		return l_return;
	}

	template< typename T >
	QuaternionT< T > operator*( float p_rScalar, QuaternionT< T > const & p_q )
	{
		QuaternionT< T > l_return( p_q );
		l_return *= p_rScalar;
		return l_return;
	}

	template< typename T >
	QuaternionT< T > operator-( QuaternionT< T > const & p_q )
	{
		QuaternionT< T > l_return( p_q );
		l_return.w = -l_return.w;
		return l_return;
	}
}
