#include "TransformationMatrix.hpp"

namespace Castor
{
	//*************************************************************************************************

	namespace
	{
		template< typename T >
		inline T mix_values( T p_a, T p_b, T p_f )
		{
			return p_a + ( p_f * ( p_b - p_a ) );
		}
	}

	//*************************************************************************************************

	template< typename T >
	QuaternionT< T >::TextLoader::TextLoader()
		: Castor::TextLoader< QuaternionT< T > >()
	{
	}

	template< typename T >
	bool QuaternionT< T >::TextLoader::operator()( QuaternionT< T > & p_object, TextFile & p_file )
	{
		String l_strWord;
		Point3< T > l_axis;
		Angle l_angle;

		for ( uint32_t i = 0; i < 3; ++i )
		{
			if ( p_file.ReadLine( l_strWord, 1024, cuT( " \r\n;\t" ) ) > 0 )
			{
				StringStream l_streamWord( l_strWord );
				l_streamWord >> l_axis[i];
			}

			xchar l_cDump;
			p_file.ReadChar( l_cDump );
		}

		if ( p_file.ReadLine( l_strWord, 1024, cuT( " \r\n;\t" ) ) > 0 )
		{
			real l_degrees;
			StringStream l_streamWord( l_strWord );
			l_streamWord >> l_degrees;
			l_angle.degrees( l_degrees );
		}

		p_object.from_axis_angle( l_axis, l_angle );
		return true;
	}

	//*************************************************************************************************

	template< typename T >
	QuaternionT< T >::TextWriter::TextWriter( String const & p_tabs )
		: Castor::TextWriter< QuaternionT< T > >( p_tabs )
	{
	}


	template< typename T >
	bool QuaternionT< T >::TextWriter::operator()( QuaternionT< T > const & p_object, TextFile & p_file )
	{
		StringStream l_streamWord;
		Point3< T > l_axis;
		Angle l_angle;
		p_object.to_axis_angle( l_axis, l_angle );

		for ( uint32_t i = 0; i < 3; ++i )
		{
			if ( !l_streamWord.str().empty() )
			{
				l_streamWord << cuT( " " );
			}

			l_streamWord << l_axis[i];
		}

		l_streamWord << cuT( " " ) << l_angle.degrees();
		bool l_result = p_file.Print( 1024, cuT( "%s%s" ), this->m_tabs.c_str(), l_streamWord.str().c_str() ) > 0;
		Castor::TextWriter< QuaternionT< T > >::CheckError( l_result, "Quaternion value" );
		return l_result;
	}

	//*************************************************************************************************

	template< typename T >
	QuaternionT< T >::QuaternionT( NoInit const & )
		: Coords4< T >( buffer )
	{
	}

	template< typename T >
	QuaternionT< T >::QuaternionT( T p_x, T p_y, T p_z, T p_w )
		: QuaternionT( NoInit() )
	{
		quat.x = p_x;
		quat.y = p_y;
		quat.z = p_z;
		quat.w = p_w;
		point::normalise( *this );
	}

	template< typename T >
	QuaternionT< T >::QuaternionT()
		: QuaternionT( 0, 0, 0, 1 )
	{
	}

	template< typename T >
	QuaternionT< T >::QuaternionT( QuaternionT< T > const & p_q )
		: QuaternionT( p_q.quat.x, p_q.quat.y, p_q.quat.z, p_q.quat.w )
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
		: QuaternionT( T( p_values[0] ), T( p_values[1] ), T( p_values[2] ), T( p_values[3] ) )
	{
	}

	template< typename T >
	QuaternionT< T >::QuaternionT( float const * p_values )
		: QuaternionT( T( p_values[0] ), T( p_values[1] ), T( p_values[2] ), T( p_values[3] ) )
	{
	}

	template< typename T >
	QuaternionT< T >::QuaternionT( Point4f const & p_values )
		: QuaternionT( T( p_values[0] ), T( p_values[1] ), T( p_values[2] ), T( p_values[3] ) )
	{
	}

	template< typename T >
	QuaternionT< T >::QuaternionT( Point4d const & p_values )
		: QuaternionT( T( p_values[0] ), T( p_values[1] ), T( p_values[2] ), T( p_values[3] ) )
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
		quat.x += p_rhs.quat.x;
		quat.y += p_rhs.quat.y;
		quat.z += p_rhs.quat.z;
		quat.w += p_rhs.quat.w;
		point::normalise( *this );
		return *this;
	}

	template< typename T >
	QuaternionT< T > & QuaternionT< T >::operator-=( QuaternionT< T > const & p_rhs )
	{
		quat.x -= p_rhs.quat.x;
		quat.y -= p_rhs.quat.y;
		quat.z -= p_rhs.quat.z;
		quat.w -= p_rhs.quat.w;
		point::normalise( *this );
		return *this;
	}

	template< typename T >
	QuaternionT< T > & QuaternionT< T >::operator*=( QuaternionT< T > const & p_rhs )
	{
		double const l_x = quat.x;
		double const l_y = quat.y;
		double const l_z = quat.z;
		double const l_w = quat.w;
		quat.x = T( l_w * p_rhs.quat.x + l_x * p_rhs.quat.w + l_y *  p_rhs.quat.z - l_z * p_rhs.quat.y );
		quat.y = T( l_w * p_rhs.quat.y + l_y * p_rhs.quat.w + l_z *  p_rhs.quat.x - l_x * p_rhs.quat.z );
		quat.z = T( l_w * p_rhs.quat.z + l_z * p_rhs.quat.w + l_x *  p_rhs.quat.y - l_y * p_rhs.quat.x );
		quat.w = T( l_w * p_rhs.quat.w - l_x * p_rhs.quat.x - l_y *  p_rhs.quat.y - l_z * p_rhs.quat.z );
		point::normalise( *this );
		return *this;
	}

	template< typename T >
	QuaternionT< T > & QuaternionT< T >::operator*=( double p_rhs )
	{
		quat.x = T( quat.x * p_rhs );
		quat.y = T( quat.y * p_rhs );
		quat.z = T( quat.z * p_rhs );
		quat.w = T( quat.w * p_rhs );
		point::normalise( *this );
		return *this;
	}

	template< typename T >
	QuaternionT< T > & QuaternionT< T >::operator*=( float p_rhs )
	{
		quat.x = T( quat.x * p_rhs );
		quat.y = T( quat.y * p_rhs );
		quat.z = T( quat.z * p_rhs );
		quat.w = T( quat.w * p_rhs );
		point::normalise( *this );
		return *this;
	}

	template< typename T >
	Point3f & QuaternionT< T >::transform( Point3f const & p_vector, Point3f & p_result )const
	{
		Point3d u( quat.x, quat.y, quat.z );
		Point3d uv( u ^ p_vector );
		Point3d uuv( u ^ uv );
		uv *= 2 * quat.w;
		uuv *= 2;
		p_result = p_vector + uv + uuv;
		return p_result;
	}

	template< typename T >
	Point3d & QuaternionT< T >::transform( Point3d const & p_vector, Point3d & p_result )const
	{
		Point3d u( quat.x, quat.y, quat.z );
		Point3d uv( u ^ p_vector );
		Point3d uuv( u ^ uv );
		uv *= 2 * quat.w;
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
		auto l_norm = point::get_normalised( p_vector ) * l_halfAngle.sin();
		quat.x = T( l_norm[0] );
		quat.y = T( l_norm[1] );
		quat.z = T( l_norm[2] );
		quat.w = T( l_halfAngle.cos() );
		point::normalise( *this );
	}

	template< typename T >
	void QuaternionT< T >::from_axis_angle( Point3d const & p_vector, Angle const & p_angle )
	{
		Angle l_halfAngle = p_angle * 0.5;
		auto l_norm = point::get_normalised( p_vector ) * l_halfAngle.sin();
		quat.x = T( l_norm[0] );
		quat.y = T( l_norm[1] );
		quat.z = T( l_norm[2] );
		quat.w = T( l_halfAngle.cos() );
		point::normalise( *this );
	}

	template< typename T >
	void QuaternionT< T >::to_axis_angle( Point3f & p_vector, Angle & p_angle )const
	{
		double const l_x = quat.x;
		double const l_y = quat.y;
		double const l_z = quat.z;
		double const l_w = quat.w;
		double l_rSqrLength = l_x * l_x + l_y * l_y + l_z * l_z;

		if ( l_rSqrLength > 0.0 )
		{
			p_angle = Angle::from_radians( 2.0 * acos( l_w ) );
			double l_rSin = p_angle.sin();
			p_vector[0] = float( l_x / l_rSin );
			p_vector[1] = float( l_y / l_rSin );
			p_vector[2] = float( l_z / l_rSin );
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
		double const l_x = quat.x;
		double const l_y = quat.y;
		double const l_z = quat.z;
		double const l_w = quat.w;
		double l_rSqrLength = l_x * l_x + l_y * l_y + l_z * l_z;

		if ( l_rSqrLength > 0.0 )
		{
			p_angle = Angle::from_radians( 2.0 * acos( l_w ) );
			double l_rSin = p_angle.sin();
			p_vector[0] = l_x / l_rSin;
			p_vector[1] = l_y / l_rSin;
			p_vector[2] = l_z / l_rSin;
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
		T l_cx = cos( T( p_pitch.radians() * 0.5 ) );
		T l_sx = sin( T( p_pitch.radians() * 0.5 ) );
		T l_cy = cos( T( p_yaw.radians() * 0.5 ) );
		T l_sy = sin( T( p_yaw.radians() * 0.5 ) );
		T l_cz = cos( T( p_roll.radians() * 0.5 ) );
		T l_sz = sin( T( p_roll.radians() * 0.5 ) );
		quat.w = T( l_cx * l_cy * l_cz ) + T( l_sx * l_sy * l_sz );
		quat.x = T( l_sx * l_cy * l_cz ) - T( l_cx * l_sy * l_sz );
		quat.y = T( l_cx * l_sy * l_cz ) + T( l_sx * l_cy * l_sz );
		quat.z = T( l_cx * l_cy * l_sz ) - T( l_sx * l_sy * l_cz );
	}

	template< typename T >
	void QuaternionT< T >::to_euler( Angle & p_pitch, Angle & p_yaw, Angle & p_roll )const
	{
		p_yaw = get_yaw();
		p_pitch = get_pitch();
		p_roll = get_roll();
	}

	template< typename T >
	Angle QuaternionT< T >::get_yaw()const
	{
		//return Angle::from_radians( 2 * acos( quat.w ) );
		return Angle::from_radians( asin( T( -2 ) * ( quat.x * quat.z - quat.w * quat.y ) ) );
	}

	template< typename T >
	Angle QuaternionT< T >::get_pitch()const
	{
		T l_res1 = T( 2.0 ) * ( quat.y * quat.z + quat.w * quat.x );
		T l_res2 = quat.w * quat.w - quat.x * quat.x - quat.y * quat.y + quat.z * quat.z;
		return Angle::from_radians( atan2( l_res1, l_res2 ) );
	}

	template< typename T >
	Angle QuaternionT< T >::get_roll()const
	{
		T l_res1 = T( 2.0 ) * ( quat.x * quat.y + quat.w * quat.z );
		T l_res2 = quat.w * quat.w + quat.x * quat.x - quat.y * quat.y - quat.z * quat.z;
		return Angle::from_radians( atan2( l_res1, l_res2 ) );
	}

	template< typename T >
	void QuaternionT< T >::conjugate()
	{
		quat.x = -quat.x;
		quat.y = -quat.y;
		quat.z = -quat.z;
	}

	template< typename T >
	QuaternionT< T > QuaternionT< T >::get_conjugate()const
	{
		QuaternionT< T > l_result( *this );
		l_result.conjugate();
		return l_result;
	}

	template< typename T >
	double QuaternionT< T >::get_magnitude()const
	{
		return point::length( *this );
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
					   mix_values( quat.x, p_target.quat.x, T( p_factor ) ),
					   mix_values( quat.y, p_target.quat.y, T( p_factor ) ),
					   mix_values( quat.z, p_target.quat.z, T( p_factor ) ),
					   mix_values( quat.w, p_target.quat.w, T( p_factor ) ) );
		}
		else
		{
			// Essential Mathematics, page 467
			T l_angle = acos( l_cosTheta );
			return ( sin( ( 1.0 - p_factor ) * l_angle ) * ( *this ) + sin( p_factor * l_angle ) * p_target ) / T( sin( l_angle ) );
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
					   mix_values( quat.x, p_target.quat.x, T( p_factor ) ),
					   mix_values( quat.y, p_target.quat.y, T( p_factor ) ),
					   mix_values( quat.z, p_target.quat.z, T( p_factor ) ),
					   mix_values( quat.w, p_target.quat.w, T( p_factor ) ) );
		}
		else
		{
			// Essential Mathematics, page 467
			T l_angle = acos( l_cosTheta );
			return ( sin( ( 1.0 - p_factor ) * l_angle ) * ( *this ) + sin( p_factor * l_angle ) * p_target ) / T( sin( l_angle ) );
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
			l_target.quat.x = -l_target.quat.x;
			l_target.quat.y = -l_target.quat.y;
			l_target.quat.z = -l_target.quat.z;
			l_target.quat.w = -l_target.quat.w;
		}

		// Calculate coefficients
		T l_sclp, l_sclq;

		if ( ( T( 1.0 ) - l_cosTheta ) > 0.0001 ) // 0.0001 -> some epsillon
		{
			// Standard case (slerp)
			T l_omega, l_sinom;
			l_omega = acos( l_cosTheta ); // extract theta from dot product's cos theta
			l_sinom = sin( l_omega );
			l_sclp = T( sin( ( 1.0 - p_factor ) * l_omega ) / l_sinom );
			l_sclq = T( sin( p_factor * l_omega ) / l_sinom );
		}
		else
		{
			// Very close, do linear interp (because it's faster)
			l_sclp = T( 1.0 - p_factor );
			l_sclq = T( p_factor );
		}

		l_target.quat.x = l_sclp * quat.x + l_sclq * l_target.quat.x;
		l_target.quat.y = l_sclp * quat.y + l_sclq * l_target.quat.y;
		l_target.quat.z = l_sclp * quat.z + l_sclq * l_target.quat.z;
		l_target.quat.w = l_sclp * quat.w + l_sclq * l_target.quat.w;
		return l_target;
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
			l_target.quat.x = -l_target.quat.x;
			l_target.quat.y = -l_target.quat.y;
			l_target.quat.z = -l_target.quat.z;
			l_target.quat.w = -l_target.quat.w;
		}

		// Calculate coefficients
		T l_sclp, l_sclq;

		if ( ( T( 1.0 ) - l_cosTheta ) > 0.0001 ) // 0.0001 -> some epsillon
		{
			// Standard case (slerp)
			T l_omega, l_sinom;
			l_omega = acos( l_cosTheta ); // extract theta from dot product's cos theta
			l_sinom = sin( l_omega );
			l_sclp = T( sin( ( 1.0 - p_factor ) * l_omega ) / l_sinom );
			l_sclq = T( sin( p_factor * l_omega ) / l_sinom );
		}
		else
		{
			// Very close, do linear interp (because it's faster)
			l_sclp = T( 1.0 - p_factor );
			l_sclq = T( p_factor );
		}

		l_target.quat.x = l_sclp * quat.x + l_sclq * l_target.quat.x;
		l_target.quat.y = l_sclp * quat.y + l_sclq * l_target.quat.y;
		l_target.quat.z = l_sclp * quat.z + l_sclq * l_target.quat.z;
		l_target.quat.w = l_sclp * quat.w + l_sclq * l_target.quat.w;
		return l_target;
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
		QuaternionT< T > l_result( p_lhs );
		l_result += p_rhs;
		return l_result;
	}

	template< typename T >
	QuaternionT< T > operator-( QuaternionT< T > const & p_lhs, QuaternionT< T > const & p_rhs )
	{
		QuaternionT< T > l_result( p_lhs );
		l_result -= p_rhs;
		return l_result;
	}

	template< typename T >
	QuaternionT< T > operator*( QuaternionT< T > const & p_lhs, QuaternionT< T > const & p_rhs )
	{
		QuaternionT< T > l_result( p_lhs );
		l_result *= p_rhs;
		return l_result;
	}

	template< typename T >
	QuaternionT< T > operator*( QuaternionT< T > const & p_lhs, double p_rhs )
	{
		QuaternionT< T > l_result( p_lhs );
		l_result *= p_rhs;
		return l_result;
	}

	template< typename T >
	QuaternionT< T > operator*( QuaternionT< T > const & p_lhs, float p_rhs )
	{
		QuaternionT< T > l_result( p_lhs );
		l_result *= p_rhs;
		return l_result;
	}

	template< typename T >
	QuaternionT< T > operator*( double p_lhs, QuaternionT< T > const & p_rhs )
	{
		QuaternionT< T > l_result( p_rhs );
		l_result *= p_lhs;
		return l_result;
	}

	template< typename T >
	QuaternionT< T > operator*( float p_lhs, QuaternionT< T > const & p_rhs )
	{
		QuaternionT< T > l_result( p_rhs );
		l_result *= p_lhs;
		return l_result;
	}

	template< typename T >
	QuaternionT< T > operator-( QuaternionT< T > const & p_q )
	{
		QuaternionT< T > l_result( p_q );
		l_result.w = -l_result.w;
		return l_result;
	}
}
