#include "TransformationMatrix.hpp"

namespace Castor
{
	namespace
	{
		template< typename T >
		inline T mix_values( T p_a, T p_b, T p_f )
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
	QuaternionT< T >::QuaternionT( double const * p_values )
		: QuaternionT( p_values[0], p_values[1], p_values[2], p_values[3] )
	{
	}

	template< typename T >
	QuaternionT< T >::QuaternionT( float const * p_values )
		: QuaternionT( p_values[0], p_values[1], p_values[2], p_values[3] )
	{
	}

	template< typename T >
	QuaternionT< T >::QuaternionT( Point4f const & p_ptValues )
		: QuaternionT( p_values[0], p_values[1], p_values[2], p_values[3] )
	{
	}

	template< typename T >
	QuaternionT< T >::QuaternionT( Point4d const & p_values )
		: QuaternionT( p_values[0], p_values[1], p_values[2], p_values[3] )
	{
	}

	template< typename T >
	QuaternionT< T >::QuaternionT( Point3f const & p_vector, Angle const & p_angle )
		: QuaternionT( NoInit() )
	{
		from_axis_angle( p_vector, p_angle );
	}

	template< typename T >
	QuaternionT< T >::QuaternionT( Point3d const & p_vector, Angle const & p_angle )
		: QuaternionT( NoInit() )
	{
		from_axis_angle( p_vector, p_angle );
	}

	template< typename T >
	QuaternionT< T >::QuaternionT( Angle const & p_pitch, Angle const & p_yaw, Angle const & p_roll )
		: QuaternionT( NoInit() )
	{
		from_euler( p_pitch, p_yaw, p_roll );
	}

	template< typename T >
	QuaternionT< T >::~QuaternionT()
	{
	}

	template< typename T >
	QuaternionT< T > & QuaternionT< T >::operator=( QuaternionT< T > const & p_rhs )
	{
		std::memcpy( buffer, p_rhs.buffer, sizeof( buffer ) );
		return *this;
	}

	template< typename T >
	QuaternionT< T > & QuaternionT< T >::operator=( QuaternionT< T > && p_rhs )
	{
		if ( this != &p_rhs )
		{
			std::memmove( buffer, p_rhs.buffer, sizeof( buffer ) );
		}

		return *this;
	}

	template< typename T >
	QuaternionT< T > & QuaternionT< T >::operator+=( QuaternionT< T > const & p_rhs )
	{
		x += p_rhs.x;
		y += p_rhs.y;
		z += p_rhs.z;
		w += p_rhs.w;
		point::normalise( *this );
		return *this;
	}

	template< typename T >
	QuaternionT< T > & QuaternionT< T >::operator-=( QuaternionT< T > const & p_rhs )
	{
		x -= p_rhs.x;
		y -= p_rhs.y;
		z -= p_rhs.z;
		w -= p_rhs.w;
		point::normalise( *this );
		return *this;
	}

	template< typename T >
	QuaternionT< T > & QuaternionT< T >::operator*=( QuaternionT< T > const & p_rhs )
	{
		double const l_x = x;
		double const l_y = y;
		double const l_z = z;
		double const l_w = w;
		x = T( l_w * p_rhs.x + l_x * p_rhs.w + l_y *  p_rhs.z - l_z * p_rhs.y );
		y = T( l_w * p_rhs.y + l_y * p_rhs.w + l_z *  p_rhs.x - l_x * p_rhs.z );
		z = T( l_w * p_rhs.z + l_z * p_rhs.w + l_x *  p_rhs.y - l_y * p_rhs.x );
		w = T( l_w * p_rhs.w - l_x * p_rhs.x - l_y *  p_rhs.y - l_z * p_rhs.z );
		point::normalise( *this );
		return *this;
	}

	template< typename T >
	QuaternionT< T > & QuaternionT< T >::operator*=( double p_rhs )
	{
		x = T( x * p_rhs );
		y = T( y * p_rhs );
		z = T( z * p_rhs );
		w = T( w * p_rhs );
		point::normalise( *this );
		return *this;
	}

	template< typename T >
	QuaternionT< T > & QuaternionT< T >::operator*=( float p_rhs )
	{
		x = T( x * p_rhs );
		y = T( y * p_rhs );
		z = T( z * p_rhs );
		w = T( w * p_rhs );
		point::normalise( *this );
		return *this;
	}

	template< typename T >
	Point3f & QuaternionT< T >::transform( Point3f const & p_vector, Point3f & p_result )const
	{
		Point3d u( x, y, z );
		Point3d uv( u ^ p_vector );
		Point3d uuv( u ^ uv );
		uv *= 2 * w;
		uuv *= 2;
		p_result = p_vector + uv + uuv;
		return p_result;
	}

	template< typename T >
	Point3d & QuaternionT< T >::transform( Point3d const & p_vector, Point3d & p_result )const
	{
		Point3d u( x, y, z );
		Point3d uv( u ^ p_vector );
		Point3d uuv( u ^ uv );
		uv *= 2 * w;
		uuv *= 2;
		p_result = p_vector + uv + uuv;
		return p_result;
	}

	template< typename T >
	void QuaternionT< T >::to_matrix( double * p_matrix )const
	{
		Matrix4x4d l_mtx = Matrix4x4d( p_matrix );
		to_matrix( l_mtx );
	}

	template< typename T >
	void QuaternionT< T >::to_matrix( float * p_matrix )const
	{
		Matrix4x4f l_mtx = Matrix4x4f( p_matrix );
		to_matrix( l_mtx );
	}

	template< typename T >
	void QuaternionT< T >::to_matrix( Matrix4x4d & p_matrix )const
	{
		matrix::set_rotate( p_matrix, *this );
	}

	template< typename T >
	void QuaternionT< T >::to_matrix( Matrix4x4f & p_matrix )const
	{
		matrix::set_rotate( p_matrix, *this );
	}

	template< typename T >
	void QuaternionT< T >::from_matrix( float const * p_matrix )
	{
		from_matrix( Matrix4x4f( p_matrix ) );
	}

	template< typename T >
	void QuaternionT< T >::from_matrix( double const * p_matrix )
	{
		from_matrix( Matrix4x4d( p_matrix ) );
	}

	template< typename T >
	void QuaternionT< T >::from_matrix( Matrix4x4f const & p_matrix )
	{
		matrix::get_rotate( p_matrix, *this );
	}

	template< typename T >
	void QuaternionT< T >::from_matrix( Matrix4x4d const & p_matrix )
	{
		matrix::get_rotate( p_matrix, *this );
	}

	template< typename T >
	void QuaternionT< T >::from_axis_angle( Point3f const & p_vector, Angle const & p_angle )
	{
		Angle l_halfAngle = p_angle * 0.5f;
		Point3f l_norm = point::get_normalised( p_vector ) * l_halfAngle.sin();
		x = T( l_norm[0] );
		y = T( l_norm[1] );
		z = T( l_norm[2] );
		w = T( l_halfAngle.cos() );
		point::normalise( *this );
	}

	template< typename T >
	void QuaternionT< T >::from_axis_angle( Point3d const & p_vector, Angle const & p_angle )
	{
		Angle l_halfAngle = p_angle * 0.5;
		Point3d l_norm = point::get_normalised( p_vector ) * l_halfAngle.sin();
		x = T( l_norm[0] );
		y = T( l_norm[1] );
		z = T( l_norm[2] );
		w = T( l_halfAngle.cos() );
		point::normalise( *this );
	}

	template< typename T >
	void QuaternionT< T >::to_axis_angle( Point3f & p_vector, Angle & p_angle )const
	{
		double const x = this->x;
		double const y = this->y;
		double const z = this->z;
		double const w = this->w;
		double l_rSqrLength = x * x + y * y + z * z;

		if ( l_rSqrLength > 0.0 )
		{
			p_angle = Angle::from_radians( 2.0 * acos( w ) );
			double l_rSin = p_angle.sin();
			p_vector[0] = float( x / l_rSin );
			p_vector[1] = float( y / l_rSin );
			p_vector[2] = float( z / l_rSin );
		}
		else
		{
			// angle is 0 (mod 2*pi), so any axis will do
			p_angle = Angle::from_radians( 0.0 );
			p_vector[0] = 1.0f;
			p_vector[1] = 0.0f;
			p_vector[2] = 0.0f;
		}

		point::normalise( p_vector );
	}

	template< typename T >
	void QuaternionT< T >::to_axis_angle( Point3d & p_vector, Angle & p_angle )const
	{
		double const x = this->x;
		double const y = this->y;
		double const z = this->z;
		double const w = this->w;
		double l_rSqrLength = x * x + y * y + z * z;

		if ( l_rSqrLength > 0.0 )
		{
			p_angle = Angle::from_radians( 2.0 * acos( w ) );
			double l_rSin = p_angle.sin();
			p_vector[0] = x / l_rSin;
			p_vector[1] = y / l_rSin;
			p_vector[2] = z / l_rSin;
		}
		else
		{
			// angle is 0 (mod 2*pi), so any axis will do
			p_angle = Angle::from_radians( 0.0 );
			p_vector[0] = 1.0;
			p_vector[1] = 0.0;
			p_vector[2] = 0.0;
		}

		point::normalise( p_vector );
	}

	template< typename T >
	void QuaternionT< T >::from_axes( Point3f const & p_x, Point3f const & p_y, Point3f const & p_z )
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
		from_matrix( l_mtxRot );
	}

	template< typename T >
	void QuaternionT< T >::from_axes( Point3d const & p_x, Point3d const & p_y, Point3d const & p_z )
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
		from_matrix( l_mtxRot );
	}

	template< typename T >
	void QuaternionT< T >::to_axes( Point3f & p_x, Point3f & p_y, Point3f & p_z )const
	{
		Matrix4x4f l_mtxRot;
		to_matrix( l_mtxRot );
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
	void QuaternionT< T >::to_axes( Point3d & p_x, Point3d & p_y, Point3d & p_z )const
	{
		Matrix4x4d l_mtxRot;
		to_matrix( l_mtxRot );
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
	void QuaternionT< T >::from_euler( Angle const & p_pitch, Angle const & p_yaw, Angle const & p_roll )
	{
		T l_c2 = cos( T( p_yaw.radians() * 0.5 ) );
		T l_s2 = sin( T( p_yaw.radians() * 0.5 ) );
		T l_c3 = cos( T( p_roll.radians() * 0.5 ) );
		T l_s3 = sin( T( p_roll.radians() * 0.5 ) );
		T l_c1 = cos( T( p_pitch.radians() * 0.5 ) );
		T l_s1 = sin( T( p_pitch.radians() * 0.5 ) );
		x = T( l_s2 * l_s3 * l_c1 ) + T( l_c2 * l_c3 * l_s1 );
		y = T( l_s2 * l_c3 * l_c1 ) + T( l_c2 * l_s3 * l_s1 );
		z = T( l_c2 * l_s3 * l_c1 ) - T( l_s2 * l_c3 * l_s1 );
		w = T( l_c2 * l_c3 * l_c1 ) - T( l_s2 * l_s3 * l_s1 );
		point::normalise( *this );
	}

	template< typename T >
	void QuaternionT< T >::to_euler( Angle & p_pitch, Angle & p_yaw, Angle & p_roll )
	{
		p_yaw = get_yaw();
		p_pitch = get_pitch();
		p_roll = get_roll();
	}

	template< typename T >
	Angle QuaternionT< T >::get_yaw()const
	{
		return Angle::from_radians( 2 * acos( w ) );
	}

	template< typename T >
	Angle QuaternionT< T >::get_pitch()const
	{
		T l_res1 = T( 2.0 ) * ( y * z + w * x );
		T l_res2 = w * w - x * x - y * y + z * z;
		return Angle::from_radians( atan2( l_res1, l_res2 ) );
	}

	template< typename T >
	Angle QuaternionT< T >::get_roll()const
	{
		T l_res1 = T( 2.0 ) * ( x * y + w * z );
		T l_res2 = w * w + x * x - y * y - z * z;
		return Angle::from_radians( atan2( l_res1, l_res2 ) );
	}

	template< typename T >
	void QuaternionT< T >::conjugate()
	{
		T w = this->w;
		point::negate( *this );
		this->w = w;
	}

	template< typename T >
	QuaternionT< T > QuaternionT< T >::get_conjugate()const
	{
		QuaternionT< T > l_return( *this );
		l_return.conjugate();
		return l_return;
	}

	template< typename T >
	double QuaternionT< T >::get_magnitude()const
	{
		return point::distance( *this );
	}

	template< typename T >
	QuaternionT< T > QuaternionT< T >::mix( QuaternionT< T > const & p_target, double p_factor )const
	{
		T l_cosTheta = point::dot( *this, p_target );

		// Perform a linear interpolation when cosTheta is close to 1 to avoid side effect of sin(angle) becoming a zero denominator
		if ( l_cosTheta > 1 - std::numeric_limits< T >::epsilon() )
		{
			// Linear interpolation
			return QuaternionT< T >(
					   mix_values( x, p_target.x, T( p_factor ) ),
					   mix_values( y, p_target.y, T( p_factor ) ),
					   mix_values( z, p_target.z, T( p_factor ) ),
					   mix_values( w, p_target.w, T( p_factor ) ) );
		}
		else
		{
			// Essential Mathematics, page 467
			T l_angle = acos( l_cosTheta );
			return ( sin( ( 1.0 - p_factor ) * l_angle ) * ( *this ) + sin( p_factor * l_angle ) * p_target ) / sin( l_angle );
		}
	}

	template< typename T >
	QuaternionT< T > QuaternionT< T >::mix( QuaternionT< T > const & p_target, float p_factor )const
	{
		T l_cosTheta = point::dot( *this, p_target );

		// Perform a linear interpolation when cosTheta is close to 1 to avoid side effect of sin(angle) becoming a zero denominator
		if ( l_cosTheta > 1 - std::numeric_limits< T >::epsilon() )
		{
			// Linear interpolation
			return QuaternionT< T >(
					   mix_values( x, p_target.x, T( p_factor ) ),
					   mix_values( y, p_target.y, T( p_factor ) ),
					   mix_values( z, p_target.z, T( p_factor ) ),
					   mix_values( w, p_target.w, T( p_factor ) ) );
		}
		else
		{
			// Essential Mathematics, page 467
			T l_angle = acos( l_cosTheta );
			return ( sin( ( 1.0 - p_factor ) * l_angle ) * ( *this ) + sin( p_factor * l_angle ) * p_target ) / sin( l_angle );
		}
	}

	template< typename T >
	QuaternionT< T > QuaternionT< T >::lerp( QuaternionT< T > const & p_target, double p_factor )const
	{
		// Lerp is only defined in [0, 1]
		assert( p_factor >= 0 );
		assert( p_factor <= 1 );

		return ( *this ) * ( 1.0 - p_factor ) + ( p_target * p_factor );
	}

	template< typename T >
	QuaternionT< T > QuaternionT< T >::lerp( QuaternionT< T > const & p_target, float p_factor )const
	{
		// Lerp is only defined in [0, 1]
		assert( p_factor >= 0 );
		assert( p_factor <= 1 );

		return ( *this ) * ( 1.0 - p_factor ) + ( p_target * p_factor );
	}

	template< typename T >
	QuaternionT< T > QuaternionT< T >::slerp( QuaternionT< T > const & p_target, double p_factor )const
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
					   mix_values( x, p_target.x, T( p_factor ) ),
					   mix_values( y, p_target.y, T( p_factor ) ),
					   mix_values( z, p_target.z, T( p_factor ) ),
					   mix_values( w, p_target.w, T( p_factor ) ) );
		}
		else
		{
			// Essential Mathematics, page 467
			T l_angle = acos( l_cosTheta );
			return ( sin( ( 1.0 - p_factor ) * l_angle ) * ( *this ) + sin( p_factor * l_angle ) * p_target ) / sin( l_angle );
		}
	}

	template< typename T >
	QuaternionT< T > QuaternionT< T >::slerp( QuaternionT< T > const & p_target, float p_factor )const
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
					   mix_values( x, p_target.x, T( p_factor ) ),
					   mix_values( y, p_target.y, T( p_factor ) ),
					   mix_values( z, p_target.z, T( p_factor ) ),
					   mix_values( w, p_target.w, T( p_factor ) ) );
		}
		else
		{
			// Essential Mathematics, page 467
			T l_angle = acos( l_cosTheta );
			return ( sin( ( 1.0 - p_factor ) * l_angle ) * ( *this ) + sin( p_factor * l_angle ) * p_target ) / sin( l_angle );
		}
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
	QuaternionT< T > operator+( QuaternionT< T > const & p_lhs, QuaternionT< T > const & p_rhs )
	{
		QuaternionT< T > l_return( p_lhs );
		l_return += p_rhs;
		return l_return;
	}

	template< typename T >
	QuaternionT< T > operator-( QuaternionT< T > const & p_lhs, QuaternionT< T > const & p_rhs )
	{
		QuaternionT< T > l_return( p_lhs );
		l_return -= p_rhs;
		return l_return;
	}

	template< typename T >
	QuaternionT< T > operator*( QuaternionT< T > const & p_lhs, QuaternionT< T > const & p_rhs )
	{
		QuaternionT< T > l_return( p_lhs );
		l_return *= p_rhs;
		return l_return;
	}

	template< typename T >
	QuaternionT< T > operator*( QuaternionT< T > const & p_lhs, double p_rhs )
	{
		QuaternionT< T > l_return( p_lhs );
		l_return *= p_rhs;
		return l_return;
	}

	template< typename T >
	QuaternionT< T > operator*( QuaternionT< T > const & p_lhs, float p_rhs )
	{
		QuaternionT< T > l_return( p_lhs );
		l_return *= p_rhs;
		return l_return;
	}

	template< typename T >
	QuaternionT< T > operator*( double p_lhs, QuaternionT< T > const & p_rhs )
	{
		QuaternionT< T > l_return( p_rhs );
		l_return *= p_lhs;
		return l_return;
	}

	template< typename T >
	QuaternionT< T > operator*( float p_lhs, QuaternionT< T > const & p_rhs )
	{
		QuaternionT< T > l_return( p_rhs );
		l_return *= p_lhs;
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
