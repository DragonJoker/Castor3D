#include "Quaternion.hpp"
#include "Point.hpp"
#include "Angle.hpp"
#include "Matrix.hpp"
#include "TransformationMatrix.hpp"

namespace Castor
{
	namespace
	{
		template< typename T >
		void QuaternionToRotationMatrix( Quaternion const & p_quat, T * p_matrix )
		{
			double const x = double( p_quat.x() );
			double const y = double( p_quat.y() );
			double const z = double( p_quat.z() );
			double const w = double( p_quat.w() );
			double fTx  = x + x;
			double fTy  = y + y;
			double fTz  = z + z;
			double fTwx = fTx * w;
			double fTwy = fTy * w;
			double fTwz = fTz * w;
			double fTxx = fTx * x;
			double fTxy = fTy * x;
			double fTxz = fTz * x;
			double fTyy = fTy * y;
			double fTyz = fTz * y;
			double fTzz = fTz * z;
			p_matrix[ 0] = T( 1 - ( fTyy + fTzz ) );
			p_matrix[ 1] = T( fTxy + fTwz );
			p_matrix[ 2] = T( fTxz - fTwy );
			p_matrix[ 3] = T( 0 );
			p_matrix[ 4] = T( fTxy - fTwz );
			p_matrix[ 5] = T( 1 - ( fTxx + fTzz ) );
			p_matrix[ 6] = T( fTyz + fTwx );
			p_matrix[ 7] = T( 0 );
			p_matrix[ 8] = T( fTxz + fTwy );
			p_matrix[ 9] = T( fTyz - fTwx );
			p_matrix[10] = T( 1 - ( fTxx + fTyy ) );
			p_matrix[11] = T( 0 );
			p_matrix[12] = 0;
			p_matrix[13] = 0;
			p_matrix[14] = 0;
			p_matrix[15] = 1;
		}
		template< typename T >
		void RotationMatrixToQuaternion( Quaternion & p_quat, SquareMatrix< T, 4 > const & p_matrix )
		{
			double l_dTrace = double( p_matrix[0][0] + p_matrix[1][1] + p_matrix[2][2] );
			double l_dRoot;

			if ( l_dTrace > 0 )
			{
				// |w| > 1/2, may as well choose w > 1/2
				l_dRoot = std::sqrt( l_dTrace + 1 );  // 2w
				p_quat[3] = double( 0.5 * l_dRoot );
				l_dRoot = 0.5 / l_dRoot;  // 1/(4w)
				p_quat[0] = double( p_matrix[2][1] - p_matrix[1][2] ) * l_dRoot;
				p_quat[1] = double( p_matrix[0][2] - p_matrix[2][0] ) * l_dRoot;
				p_quat[2] = double( p_matrix[1][0] - p_matrix[0][1] ) * l_dRoot;
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
				double * l_apkQuat[3] = { &p_quat[0], &p_quat[1], &p_quat[2] };
				*l_apkQuat[i] = 0.5 * l_dRoot;
				l_dRoot = 0.5 / l_dRoot;
				*l_apkQuat[j] = double( p_matrix[j][i] + p_matrix[i][j] ) * l_dRoot;
				*l_apkQuat[k] = double( p_matrix[k][i] + p_matrix[i][k] ) * l_dRoot;
				p_quat[3] = double( p_matrix[k][j] - p_matrix[j][k] ) * l_dRoot;
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
			y  = number;
			i  = * ( long * ) &y;                       // evil floating point bit level hacking
			i  = 0x5f3759df - ( i >> 1 );               // what the fuck?
			y  = * ( float * ) &i;
			y  = y * ( threehalfs - ( x2 * y * y ) );   // 1st iteration
			y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed
			return y;
		}
#else
		inline float rsqrt( float number )
		{
			return 1 / sqrt( number );
		}
#endif
	}

	Quaternion::Quaternion( double p_x, double p_y, double p_z, double p_w )
		: Coords4d( m_data.buffer )
	{
		x() = p_x;
		y() = p_y;
		z() = p_z;
		w() = p_w;
	}

	Quaternion::Quaternion()
		: Coords4d( m_data.buffer )
	{
		x() = 0;
		y() = 0;
		z() = 0;
		w() = 1;
	}

	Quaternion::Quaternion( Quaternion const & p_q )
		: Coords4d( m_data.buffer )
	{
		x() = p_q.x();
		y() = p_q.y();
		z() = p_q.z();
		w() = p_q.w();
	}

	Quaternion::Quaternion( Quaternion && p_q )
		: Coords4d( m_data.buffer )
	{
		x() = p_q.x();
		y() = p_q.y();
		z() = p_q.z();
		w() = p_q.w();
		p_q.x() = 0;
		p_q.y() = 0;
		p_q.z() = 0;
		p_q.w() = 0;
	}

	Quaternion::Quaternion( double const * p_q )
		: Coords4d( m_data.buffer )
	{
		x() = p_q[0];
		y() = p_q[1];
		z() = p_q[2];
		w() = p_q[3];
	}

	Quaternion::Quaternion( Point4f const & p_ptValues )
		: Coords4d( m_data.buffer )
	{
		x() = p_ptValues[0];
		y() = p_ptValues[1];
		z() = p_ptValues[2];
		w() = p_ptValues[3];
	}

	Quaternion::Quaternion( Point4d const & p_ptValues )
		: Coords4d( m_data.buffer )
	{
		x() = p_ptValues[0];
		y() = p_ptValues[1];
		z() = p_ptValues[2];
		w() = p_ptValues[3];
	}

	Quaternion::Quaternion( Point3f const & p_vector, Angle const & p_angle )
		: Coords4d( m_data.buffer )
	{
		FromAxisAngle( p_vector, p_angle );
	}

	Quaternion::Quaternion( Point3d const & p_vector, Angle const & p_angle )
		: Coords4d( m_data.buffer )
	{
		FromAxisAngle( p_vector, p_angle );
	}

	Quaternion::Quaternion( Angle const & p_yaw, Angle const & p_pitch, Angle const & p_roll )
	{
		FromEulerAngles( p_yaw, p_pitch, p_roll );
	}

	Quaternion::~Quaternion()
	{
	}

	Quaternion & Quaternion::operator =( Quaternion const & p_q )
	{
		x() = p_q.x();
		y() = p_q.y();
		z() = p_q.z();
		w() = p_q.w();
		return *this;
	}

	Quaternion & Quaternion::operator =( Quaternion && p_q )
	{
		if ( this != &p_q )
		{
			x() = p_q.x();
			y() = p_q.y();
			z() = p_q.z();
			w() = p_q.w();
			p_q.x() = 0;
			p_q.y() = 0;
			p_q.z() = 0;
			p_q.w() = 0;
		}

		return *this;
	}

	Quaternion & Quaternion::operator +=( Quaternion const & p_q )
	{
		x() += p_q.x();
		y() += p_q.y();
		z() += p_q.z();
		w() += p_q.w();
		//point::normalise( *this );
		return * this;
	}

	Quaternion & Quaternion::operator -=( Quaternion const & p_q )
	{
		x() -= p_q.x();
		y() -= p_q.y();
		z() -= p_q.z();
		w() -= p_q.w();
		//point::normalise( *this );
		return * this;
	}

	Quaternion & Quaternion::operator *=( Quaternion const & p_q )
	{
		double const l_x = x();
		double const l_y = y();
		double const l_z = z();
		double const l_w = w();
		double const l_qx = p_q.x();
		double const l_qy = p_q.y();
		double const l_qz = p_q.z();
		double const l_qw = p_q.w();
		x() = l_w * l_qx + l_x * l_qw + l_y * l_qz - l_z * l_qy;
		y() = l_w * l_qy + l_y * l_qw + l_z * l_qx - l_x * l_qz;
		z() = l_w * l_qz + l_z * l_qw + l_x * l_qy - l_y * l_qx;
		w() = l_w * l_qw - l_x * l_qx - l_y * l_qy - l_z * l_qz;
		return * this;
	}

	Quaternion & Quaternion::operator *=( double p_rScalar )
	{
		x() *= p_rScalar;
		y() *= p_rScalar;
		z() *= p_rScalar;
		w() *= p_rScalar;
		//point::normalise( *this );
		return * this;
	}

	Point3f & Quaternion::Transform( Point3f const & p_vector, Point3f & p_ptResult )const
	{
		Point3d u( x(), y(), z() );
		Point3d uv( u ^ p_vector );
		Point3d uuv( u ^ uv );
		uv *= 2 * w();
		uuv *= 2;
		p_ptResult = p_vector + uv + uuv;
		return p_ptResult;
	}

	Point3d & Quaternion::Transform( Point3d const & p_vector, Point3d & p_ptResult )const
	{
		Point3d u( x(), y(), z() );
		Point3d uv( u ^ p_vector );
		Point3d uuv( u ^ uv );
		uv *= 2 * w();
		uuv *= 2;
		p_ptResult = p_vector + uv + uuv;
		return p_ptResult;
	}

	void Quaternion::ToRotationMatrix( double * p_matrix )const
	{
		QuaternionToRotationMatrix( *this, p_matrix );
	}

	void Quaternion::ToRotationMatrix( float * p_matrix )const
	{
		QuaternionToRotationMatrix( *this, p_matrix );
	}

	void Quaternion::FromRotationMatrix( Matrix4x4f const & p_matrix )
	{
		RotationMatrixToQuaternion( *this, p_matrix );
	}

	void Quaternion::FromRotationMatrix( Matrix4x4d const & p_matrix )
	{
		RotationMatrixToQuaternion( *this, p_matrix );
	}

	void Quaternion::FromAxisAngle( Point3f const & p_vector, Angle const & p_angle )
	{
		Angle l_halfAngle = p_angle * 0.5f;
		Point3f l_norm = point::get_normalised( p_vector ) * l_halfAngle.Sin();
		x() = l_norm[0];
		y() = l_norm[1];
		z() = l_norm[2];
		w() = l_halfAngle.Cos();
	}

	void Quaternion::FromAxisAngle( Point3d const & p_vector, Angle const & p_angle )
	{
		Angle l_halfAngle = p_angle * 0.5;
		Point3d l_norm = point::get_normalised( p_vector ) * l_halfAngle.Sin();
		x() = l_norm[0];
		y() = l_norm[1];
		z() = l_norm[2];
		w() = l_halfAngle.Cos();
	}

	void Quaternion::ToAxisAngle( Point3f & p_vector, Angle & p_angle )const
	{
		double const & x = m_data.quaternion.x;
		double const & y = m_data.quaternion.y;
		double const & z = m_data.quaternion.z;
		double const & w = m_data.quaternion.w;
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

	void Quaternion::ToAxisAngle( Point3d & p_vector, Angle & p_angle )const
	{
		double const & x = m_data.quaternion.x;
		double const & y = m_data.quaternion.y;
		double const & z = m_data.quaternion.z;
		double const & w = m_data.quaternion.w;
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

	void Quaternion::FromAxes( Point3f const & p_x, Point3f const & p_y, Point3f const & p_z )
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

	void Quaternion::FromAxes( Point3d const & p_x, Point3d const & p_y, Point3d const & p_z )
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

	void Quaternion::ToAxes( Point3f & p_x, Point3f & p_y, Point3f & p_z )const
	{
		Matrix4x4d l_mtxRot;
		ToRotationMatrix( l_mtxRot );
		p_x[0] = float( l_mtxRot[0][0] );
		p_x[1] = float( l_mtxRot[1][0] );
		p_x[2] = float( l_mtxRot[2][0] );
		p_y[0] = float( l_mtxRot[0][1] );
		p_y[1] = float( l_mtxRot[1][1] );
		p_y[2] = float( l_mtxRot[2][1] );
		p_z[0] = float( l_mtxRot[0][2] );
		p_z[1] = float( l_mtxRot[1][2] );
		p_z[2] = float( l_mtxRot[2][2] );
	}

	void Quaternion::ToAxes( Point3d & p_x, Point3d & p_y, Point3d & p_z )const
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

	void Quaternion::FromEulerAngles( Angle const & p_pitch, Angle const & p_yaw, Angle const & p_roll )
	{
		double c1 = cos( p_pitch.Radians() / 2 );
		double s1 = sin( p_pitch.Radians() / 2 );
		double c2 = cos( p_yaw.Radians() / 2 );
		double s2 = sin( p_yaw.Radians() / 2 );
		double c3 = cos( p_roll.Radians() / 2 );
		double s3 = sin( p_roll.Radians() / 2 );
		x() = float( s1 * c2 * c3 ) - float( c1 * s2 * s3 );
		y() = float( c1 * s2 * c3 ) + float( s1 * c2 * s3 );
		z() = float( c1 * c2 * s3 ) - float( s1 * s2 * c3 );
		w() = float( c1 * c2 * c3 ) + float( s1 * s2 * s3 );
	}

	void Quaternion::ToEulerAngles( Angle & p_pitch, Angle & p_yaw, Angle & p_roll )
	{
		p_yaw = GetYaw();
		p_pitch = GetPitch();
		p_roll = GetRoll();
	}

	Angle Quaternion::GetYaw()const
	{
		return Angle::FromRadians( asin( -2 * ( x() * z() - w() * y() ) ) );
	}

	Angle Quaternion::GetPitch()const
	{
		double l_res1 = 2.0 * ( y() * z() + w() * x() );
		double l_res2 = w() * w() - x() * x() - y() * y() + z() * z();
		return Angle::FromRadians( atan2( l_res1, l_res2 ) );
	}

	Angle Quaternion::GetRoll()const
	{
		double l_res1 = 2 * ( x() * y() + w() * z() );
		double l_res2 = w() * w() + x() * x() - y() * y() - z() * z();
		return Angle::FromRadians( atan2( l_res1, l_res2 ) );
	}

	void Quaternion::Conjugate()
	{
		double w = m_data.quaternion.w;
		point::negate( *this );
		m_data.quaternion.w = w;
	}

	Quaternion Quaternion::GetConjugate()const
	{
		Quaternion l_qReturn( *this );
		l_qReturn.Conjugate();
		return l_qReturn;
	}

	double Quaternion::GetMagnitude()const
	{
		return point::distance( *this );
	}

	double mix( double p_a, double p_b, double p_f )
	{
		return p_a + ( p_f * ( p_b - p_a ) );
	}

	Quaternion Quaternion::Mix( Quaternion const & p_target, double p_factor )const
	{
		double l_cosTheta = point::dot( *this, p_target );

		// Perform a linear interpolation when cosTheta is close to 1 to avoid side effect of sin(angle) becoming a zero denominator
		if ( l_cosTheta > 1 - std::numeric_limits< double >::epsilon() )
		{
			// Linear interpolation
			return Quaternion(
				mix( x(), p_target.x(), p_factor ),
				mix( y(), p_target.y(), p_factor ),
				mix( z(), p_target.z(), p_factor ),
				mix( w(), p_target.w(), p_factor ) );
		}
		else
		{
			// Essential Mathematics, page 467
			double l_angle = acos( l_cosTheta );
			return ( sin( ( 1.0 - p_factor ) * l_angle ) * ( *this ) + sin( p_factor * l_angle ) * p_target ) / sin( l_angle );
		}
	}

	Quaternion Quaternion::Lerp( Quaternion const & p_target,  double p_factor )const
	{
		// Lerp is only defined in [0, 1]
		assert( p_factor >= 0 );
		assert( p_factor <= 1 );

		return ( *this ) * ( 1.0 - p_factor ) + ( p_target * p_factor );
	}

	Quaternion Quaternion::Slerp( Quaternion const & p_target, double p_factor )const
	{
		//	Slerp = q1((q1^-1)q2)^t;
		double l_cosTheta = point::dot( *this, p_target );
		Quaternion l_target( p_target );

		// Do we need to invert rotation?
		if ( l_cosTheta < 0 )
		{
			l_cosTheta = -l_cosTheta;
			l_target = -p_target;
		}

		// Perform a linear interpolation when cosTheta is close to 1 to avoid side effect of sin(angle) becoming a zero denominator
		if ( l_cosTheta > 1 - std::numeric_limits< double >::epsilon() )
		{
			// Linear interpolation
			return Quaternion(
				mix( x(), p_target.x(), p_factor ),
				mix( y(), p_target.y(), p_factor ),
				mix( z(), p_target.z(), p_factor ),
				mix( w(), p_target.w(), p_factor ) );
		}
		else
		{
			// Essential Mathematics, page 467
			double l_angle = acos( l_cosTheta );
			return ( sin( ( 1.0 - p_factor ) * l_angle ) * ( *this ) + sin( p_factor * l_angle ) * p_target ) / sin( l_angle );
		}
	}

	Quaternion Quaternion::Identity()
	{
		return Quaternion( 0, 0, 0, 1 );
	}

	Quaternion Quaternion::Null()
	{
		return Quaternion( 0, 0, 0, 0 );
	}

	Quaternion operator +( Quaternion const & p_qA, Quaternion const & p_qB )
	{
		Quaternion l_qReturn( p_qA );
		l_qReturn += p_qB;
		return l_qReturn;
	}

	Quaternion operator -( Quaternion const & p_qA, Quaternion const & p_qB )
	{
		Quaternion l_qReturn( p_qA );
		l_qReturn -= p_qB;
		return l_qReturn;
	}

	Quaternion operator *( Quaternion const & p_qA, Quaternion const & p_qB )
	{
		Quaternion l_qReturn( p_qA );
		l_qReturn *= p_qB;
		return l_qReturn;
	}

	Quaternion operator *( Quaternion const & p_q, double p_rScalar )
	{
		Quaternion l_qReturn( p_q );
		l_qReturn *= p_rScalar;
		return l_qReturn;
	}

	Quaternion operator *( double p_rScalar, Quaternion const & p_q )
	{
		Quaternion l_qReturn( p_q );
		l_qReturn *= p_rScalar;
		return l_qReturn;
	}

	Quaternion operator -( Quaternion const & p_q )
	{
		Quaternion l_qReturn( p_q );
		l_qReturn.w() = -l_qReturn.w();
		return l_qReturn;
	}
}
