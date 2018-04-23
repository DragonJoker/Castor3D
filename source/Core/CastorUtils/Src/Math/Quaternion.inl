#include "TransformationMatrix.hpp"

namespace castor
{
	//*************************************************************************************************

	namespace
	{
		template< typename T >
		inline T mixValues( T p_a, T p_b, T p_f )
		{
			return p_a + ( p_f * ( p_b - p_a ) );
		}
	}

	//*************************************************************************************************

	template< typename T >
	QuaternionT< T >::TextLoader::TextLoader()
		: castor::TextLoader< QuaternionT< T > >()
	{
	}

	template< typename T >
	bool QuaternionT< T >::TextLoader::operator()( QuaternionT< T > & p_object, TextFile & p_file )
	{
		String strWord;
		Point3< T > axis;
		Angle angle;

		for ( uint32_t i = 0; i < 3; ++i )
		{
			if ( p_file.readLine( strWord, 1024, cuT( " \r\n;\t" ) ) > 0 )
			{
				StringStream streamWord( strWord );
				streamWord >> axis[i];
			}

			xchar cDump;
			p_file.readChar( cDump );
		}

		if ( p_file.readLine( strWord, 1024, cuT( " \r\n;\t" ) ) > 0 )
		{
			real degrees;
			StringStream streamWord( strWord );
			streamWord >> degrees;
			angle.degrees( degrees );
		}

		p_object.fromAxisAngle( axis, angle );
		return true;
	}

	//*************************************************************************************************

	template< typename T >
	QuaternionT< T >::TextWriter::TextWriter( String const & p_tabs )
		: castor::TextWriter< QuaternionT< T > >( p_tabs )
	{
	}


	template< typename T >
	bool QuaternionT< T >::TextWriter::operator()( QuaternionT< T > const & p_object, TextFile & p_file )
	{
		StringStream streamWord{ makeStringStream() };
		Point3< T > axis;
		Angle angle;
		p_object.toAxisAngle( axis, angle );

		for ( uint32_t i = 0; i < 3; ++i )
		{
			if ( !streamWord.str().empty() )
			{
				streamWord << cuT( " " );
			}

			streamWord << axis[i];
		}

		streamWord << cuT( " " ) << angle.degrees();
		bool result = p_file.print( 1024, cuT( "%s%s" ), this->m_tabs.c_str(), streamWord.str().c_str() ) > 0;
		castor::TextWriter< QuaternionT< T > >::checkError( result, "Quaternion value" );
		return result;
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
		double const x = quat.x;
		double const y = quat.y;
		double const z = quat.z;
		double const w = quat.w;
		quat.x = T( w * p_rhs.quat.x + x * p_rhs.quat.w + y *  p_rhs.quat.z - z * p_rhs.quat.y );
		quat.y = T( w * p_rhs.quat.y + y * p_rhs.quat.w + z *  p_rhs.quat.x - x * p_rhs.quat.z );
		quat.z = T( w * p_rhs.quat.z + z * p_rhs.quat.w + x *  p_rhs.quat.y - y * p_rhs.quat.x );
		quat.w = T( w * p_rhs.quat.w - x * p_rhs.quat.x - y *  p_rhs.quat.y - z * p_rhs.quat.z );
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
	QuaternionT< T > QuaternionT< T >::fromMatrix( float const * p_matrix )
	{
		return fromMatrix( Matrix4x4f( p_matrix ) );
	}

	template< typename T >
	QuaternionT< T > QuaternionT< T >::fromMatrix( double const * p_matrix )
	{
		return fromMatrix( Matrix4x4d( p_matrix ) );
	}

	template< typename T >
	QuaternionT< T > QuaternionT< T >::fromMatrix( Matrix4x4f const & p_matrix )
	{
		QuaternionT< T > result;
		matrix::getRotate( p_matrix, result );
		return result;
	}

	template< typename T >
	QuaternionT< T > QuaternionT< T >::fromMatrix( Matrix4x4d const & p_matrix )
	{
		QuaternionT< T > result;
		matrix::getRotate( p_matrix, result );
		return result;
	}

	template< typename T >
	QuaternionT< T > QuaternionT< T >::fromAxisAngle( Point3f const & p_vector, Angle const & p_angle )
	{
		QuaternionT< T > result;
		Angle halfAngle = p_angle * 0.5f;
		auto norm = point::getNormalised( p_vector ) * halfAngle.sin();
		result.quat.x = T( norm[0] );
		result.quat.y = T( norm[1] );
		result.quat.z = T( norm[2] );
		result.quat.w = T( halfAngle.cos() );
		point::normalise( result );
		return result;
	}

	template< typename T >
	QuaternionT< T > QuaternionT< T >::fromAxisAngle( Point3d const & p_vector, Angle const & p_angle )
	{
		QuaternionT< T > result;
		Angle halfAngle = p_angle * 0.5;
		auto norm = point::getNormalised( p_vector ) * double( halfAngle.sin() );
		result.quat.x = T( norm[0] );
		result.quat.y = T( norm[1] );
		result.quat.z = T( norm[2] );
		result.quat.w = T( halfAngle.cos() );
		point::normalise( result );
		return result;
	}

	template< typename T >
	QuaternionT< T > QuaternionT< T >::fromAxes( Point3f const & p_x, Point3f const & p_y, Point3f const & p_z )
	{
		Matrix4x4f mtxRot;
		mtxRot[0][0] = p_x[0];
		mtxRot[1][0] = p_x[1];
		mtxRot[2][0] = p_x[2];
		mtxRot[0][1] = p_y[0];
		mtxRot[1][1] = p_y[1];
		mtxRot[2][1] = p_y[2];
		mtxRot[0][2] = p_z[0];
		mtxRot[1][2] = p_z[1];
		mtxRot[2][2] = p_z[2];
		return fromMatrix( mtxRot );
	}

	template< typename T >
	QuaternionT< T > QuaternionT< T >::fromAxes( Point3d const & p_x, Point3d const & p_y, Point3d const & p_z )
	{
		Matrix4x4d mtxRot;
		mtxRot[0][0] = p_x[0];
		mtxRot[1][0] = p_x[1];
		mtxRot[2][0] = p_x[2];
		mtxRot[0][1] = p_y[0];
		mtxRot[1][1] = p_y[1];
		mtxRot[2][1] = p_y[2];
		mtxRot[0][2] = p_z[0];
		mtxRot[1][2] = p_z[1];
		mtxRot[2][2] = p_z[2];
		return fromMatrix( mtxRot );
	}

	template< typename T >
	Point3f & QuaternionT< T >::transform( Point3f const & p_vector, Point3f & p_result )const
	{
		Point3d u( quat.x, quat.y, quat.z );
		Point3d uv( castor::point::cross( u, p_vector ) );
		Point3d uuv( castor::point::cross( u, uv ) );
		uv *= 2 * quat.w;
		uuv *= 2;
		p_result = p_vector + uv + uuv;
		return p_result;
	}

	template< typename T >
	Point3d & QuaternionT< T >::transform( Point3d const & p_vector, Point3d & p_result )const
	{
		Point3d u( quat.x, quat.y, quat.z );
		Point3d uv( castor::point::cross( u, p_vector ) );
		Point3d uuv( castor::point::cross( u, uv ) );
		uv *= 2 * quat.w;
		uuv *= 2;
		p_result = p_vector + uv + uuv;
		return p_result;
	}

	template< typename T >
	void QuaternionT< T >::toMatrix( double * p_matrix )const
	{
		Matrix4x4d mtx = Matrix4x4d( p_matrix );
		toMatrix( mtx );
	}

	template< typename T >
	void QuaternionT< T >::toMatrix( float * p_matrix )const
	{
		Matrix4x4f mtx = Matrix4x4f( p_matrix );
		toMatrix( mtx );
	}

	template< typename T >
	void QuaternionT< T >::toMatrix( Matrix4x4d & p_matrix )const
	{
		matrix::setRotate( p_matrix, *this );
	}

	template< typename T >
	void QuaternionT< T >::toMatrix( Matrix4x4f & p_matrix )const
	{
		matrix::setRotate( p_matrix, *this );
	}

	template< typename T >
	void QuaternionT< T >::toAxisAngle( Point3f & p_vector, Angle & p_angle )const
	{
		T const x = quat.x;
		T const y = quat.y;
		T const z = quat.z;
		T const w = quat.w;
		T s = sqrt( T{ 1.0 } - ( w * w ) );

		if ( std::abs( s ) < std::numeric_limits< T >::epsilon() )
		{
			// angle is 0 (mod 2*pi), so any axis will do
			p_angle = Angle::fromRadians( 0.0 );
			p_vector[0] = T{ 1 };
			p_vector[1] = T{ 0 };
			p_vector[2] = T{ 0 };
		}
		else
		{
			p_angle.acos( w );
			p_angle *= 2;
			p_vector[0] = x / s;
			p_vector[1] = y / s;
			p_vector[2] = z / s;
		}

		point::normalise( p_vector );
	}

	template< typename T >
	void QuaternionT< T >::toAxisAngle( Point3d & p_vector, Angle & p_angle )const
	{
		T const x = quat.x;
		T const y = quat.y;
		T const z = quat.z;
		T const w = quat.w;
		T s = sqrt( T{ 1.0 } - ( w * w ) );

		if ( std::abs( s ) < std::numeric_limits< T >::epsilon() )
		{
			// angle is 0 (mod 2*pi), so any axis will do
			p_angle = Angle::fromRadians( 0.0 );
			p_vector[0] = T{ 1 };
			p_vector[1] = T{ 0 };
			p_vector[2] = T{ 0 };
		}
		else
		{
			p_angle.acos( w );
			p_angle *= 2;
			p_vector[0] = x / s;
			p_vector[1] = y / s;
			p_vector[2] = z / s;
		}

		point::normalise( p_vector );
	}

	template< typename T >
	void QuaternionT< T >::toAxes( Point3f & p_x, Point3f & p_y, Point3f & p_z )const
	{
		Matrix4x4f mtxRot;
		toMatrix( mtxRot );
		p_x[0] = mtxRot[0][0];
		p_x[1] = mtxRot[1][0];
		p_x[2] = mtxRot[2][0];
		p_y[0] = mtxRot[0][1];
		p_y[1] = mtxRot[1][1];
		p_y[2] = mtxRot[2][1];
		p_z[0] = mtxRot[0][2];
		p_z[1] = mtxRot[1][2];
		p_z[2] = mtxRot[2][2];
	}

	template< typename T >
	void QuaternionT< T >::toAxes( Point3d & p_x, Point3d & p_y, Point3d & p_z )const
	{
		Matrix4x4d mtxRot;
		toMatrix( mtxRot );
		p_x[0] = mtxRot[0][0];
		p_x[1] = mtxRot[1][0];
		p_x[2] = mtxRot[2][0];
		p_y[0] = mtxRot[0][1];
		p_y[1] = mtxRot[1][1];
		p_y[2] = mtxRot[2][1];
		p_z[0] = mtxRot[0][2];
		p_z[1] = mtxRot[1][2];
		p_z[2] = mtxRot[2][2];
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
	QuaternionT< T > QuaternionT< T >::mix( QuaternionT< T > const & p_target, double p_factor )const
	{
		T cosTheta = point::dot( *this, p_target );

		// Perform a linear interpolation when cosTheta is close to 1 to avoid side effect of sin(angle) becoming a zero denominator
		if ( cosTheta > 1 - std::numeric_limits< T >::epsilon() )
		{
			// Linear interpolation
			return QuaternionT< T >(
				mixValues( quat.x, p_target.quat.x, T( p_factor ) ),
				mixValues( quat.y, p_target.quat.y, T( p_factor ) ),
				mixValues( quat.z, p_target.quat.z, T( p_factor ) ),
				mixValues( quat.w, p_target.quat.w, T( p_factor ) ) );
		}
		else
		{
			// Essential Mathematics, page 467
			T angle = acos( cosTheta );
			return ( sin( ( 1.0 - p_factor ) * angle ) * ( *this ) + sin( p_factor * angle ) * p_target ) / T( sin( angle ) );
		}
	}

	template< typename T >
	QuaternionT< T > QuaternionT< T >::mix( QuaternionT< T > const & p_target, float p_factor )const
	{
		T cosTheta = point::dot( *this, p_target );

		// Perform a linear interpolation when cosTheta is close to 1 to avoid side effect of sin(angle) becoming a zero denominator
		if ( cosTheta > 1 - std::numeric_limits< T >::epsilon() )
		{
			// Linear interpolation
			return QuaternionT< T >(
				mixValues( quat.x, p_target.quat.x, T( p_factor ) ),
				mixValues( quat.y, p_target.quat.y, T( p_factor ) ),
				mixValues( quat.z, p_target.quat.z, T( p_factor ) ),
				mixValues( quat.w, p_target.quat.w, T( p_factor ) ) );
		}
		else
		{
			// Essential Mathematics, page 467
			T angle = acos( cosTheta );
			return QuaternionT{ ( sin( ( 1.0 - p_factor ) * angle ) * ( *this ) + sin( p_factor * angle ) * p_target ) / T( sin( angle ) ) };
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
		T cosTheta = point::dot( *this, p_target );
		QuaternionT< T > target( p_target );

		// do we need to invert rotation?
		if ( cosTheta < 0 )
		{
			cosTheta = -cosTheta;
			target.quat.x = -target.quat.x;
			target.quat.y = -target.quat.y;
			target.quat.z = -target.quat.z;
			target.quat.w = -target.quat.w;
		}

		// Calculate coefficients
		T sclp, sclq;

		if ( ( T( 1.0 ) - cosTheta ) > 0.0001 ) // 0.0001 -> some epsillon
		{
			// Standard case (slerp)
			T omega, sinom;
			omega = acos( cosTheta ); // extract theta from dot product's cos theta
			sinom = sin( omega );
			sclp = T( sin( ( 1.0 - p_factor ) * omega ) / sinom );
			sclq = T( sin( p_factor * omega ) / sinom );
		}
		else
		{
			// Very close, do linear interp (because it's faster)
			sclp = T( 1.0 - p_factor );
			sclq = T( p_factor );
		}

		target.quat.x = sclp * quat.x + sclq * target.quat.x;
		target.quat.y = sclp * quat.y + sclq * target.quat.y;
		target.quat.z = sclp * quat.z + sclq * target.quat.z;
		target.quat.w = sclp * quat.w + sclq * target.quat.w;
		return target;
	}

	template< typename T >
	QuaternionT< T > QuaternionT< T >::slerp( QuaternionT< T > const & p_target, float p_factor )const
	{
		//	Slerp = q1((q1^-1)q2)^t;
		T cosTheta = point::dot( *this, p_target );
		QuaternionT< T > target( p_target );

		// do we need to invert rotation?
		if ( cosTheta < 0 )
		{
			cosTheta = -cosTheta;
			target.quat.x = -target.quat.x;
			target.quat.y = -target.quat.y;
			target.quat.z = -target.quat.z;
			target.quat.w = -target.quat.w;
		}

		// Calculate coefficients
		T sclp, sclq;

		if ( ( T( 1.0 ) - cosTheta ) > 0.0001 ) // 0.0001 -> some epsillon
		{
			// Standard case (slerp)
			T omega, sinom;
			omega = acos( cosTheta ); // extract theta from dot product's cos theta
			sinom = sin( omega );
			sclp = T( sin( ( 1.0 - p_factor ) * omega ) / sinom );
			sclq = T( sin( p_factor * omega ) / sinom );
		}
		else
		{
			// Very close, do linear interp (because it's faster)
			sclp = T( 1.0 - p_factor );
			sclq = T( p_factor );
		}

		target.quat.x = sclp * quat.x + sclq * target.quat.x;
		target.quat.y = sclp * quat.y + sclq * target.quat.y;
		target.quat.z = sclp * quat.z + sclq * target.quat.z;
		target.quat.w = sclp * quat.w + sclq * target.quat.w;
		return target;
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
		QuaternionT< T > result( p_lhs );
		result += p_rhs;
		return result;
	}

	template< typename T >
	QuaternionT< T > operator-( QuaternionT< T > const & p_lhs, QuaternionT< T > const & p_rhs )
	{
		QuaternionT< T > result( p_lhs );
		result -= p_rhs;
		return result;
	}

	template< typename T >
	QuaternionT< T > operator*( QuaternionT< T > const & p_lhs, QuaternionT< T > const & p_rhs )
	{
		QuaternionT< T > result( p_lhs );
		result *= p_rhs;
		return result;
	}

	template< typename T >
	QuaternionT< T > operator*( QuaternionT< T > const & p_lhs, double p_rhs )
	{
		QuaternionT< T > result( p_lhs );
		result *= p_rhs;
		return result;
	}

	template< typename T >
	QuaternionT< T > operator*( QuaternionT< T > const & p_lhs, float p_rhs )
	{
		QuaternionT< T > result( p_lhs );
		result *= p_rhs;
		return result;
	}

	template< typename T >
	QuaternionT< T > operator*( double p_lhs, QuaternionT< T > const & p_rhs )
	{
		QuaternionT< T > result( p_rhs );
		result *= p_lhs;
		return result;
	}

	template< typename T >
	QuaternionT< T > operator*( float p_lhs, QuaternionT< T > const & p_rhs )
	{
		QuaternionT< T > result( p_rhs );
		result *= p_lhs;
		return result;
	}

	template< typename T >
	QuaternionT< T > operator-( QuaternionT< T > const & p_q )
	{
		QuaternionT< T > result( p_q );
		result.w = -result.w;
		return result;
	}
}
