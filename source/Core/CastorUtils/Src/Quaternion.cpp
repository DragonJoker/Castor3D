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
			p_matrix[ 1] = T( fTxy - fTwz );
			p_matrix[ 2] = T( fTxz + fTwy );
			p_matrix[ 3] = T( 0 );
			p_matrix[ 4] = T( fTxy + fTwz );
			p_matrix[ 5] = T( 1 - ( fTxx + fTzz ) );
			p_matrix[ 6] = T( fTyz - fTwx );
			p_matrix[ 7] = T( 0 );
			p_matrix[ 8] = T( fTxz - fTwy );
			p_matrix[ 9] = T( fTyz + fTwx );
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
			double l_dTrace = real( p_matrix[0][0] + p_matrix[1][1] + p_matrix[2][2] );
			double l_dRoot;

			if ( l_dTrace > 0 )
			{
				// |w| > 1/2, may as well choose w > 1/2
				l_dRoot = std::sqrt( l_dTrace + 1 );  // 2w
				p_quat[3] = real( 0.5 * l_dRoot );
				l_dRoot = 0.5 / l_dRoot;  // 1/(4w)
				p_quat[0] = real( double( p_matrix[2][1] - p_matrix[1][2] ) * l_dRoot );
				p_quat[1] = real( double( p_matrix[0][2] - p_matrix[2][0] ) * l_dRoot );
				p_quat[2] = real( double( p_matrix[1][0] - p_matrix[0][1] ) * l_dRoot );
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
				real * l_apkQuat[3] = { &p_quat[0], &p_quat[1], &p_quat[2] };
				*l_apkQuat[i] = real( 0.5 * l_dRoot );
				l_dRoot = 0.5 / l_dRoot;
				p_quat[3] = real( double( p_matrix[k][j] - p_matrix[j][k] ) * l_dRoot );
				*l_apkQuat[j] = real( double( p_matrix[j][i] + p_matrix[i][j] ) * l_dRoot );
				*l_apkQuat[k] = real( double( p_matrix[k][i] + p_matrix[i][k] ) * l_dRoot );
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

	Quaternion::Quaternion( real p_x, real p_y, real p_z, real p_w )
		:	Coords4r( m_data.buffer )
	{
		m_data.quaternion.x = p_x;
		m_data.quaternion.y = p_y;
		m_data.quaternion.z = p_z;
		m_data.quaternion.w = p_w;
	}

	Quaternion::Quaternion()
		:	Coords4r( m_data.buffer	)
	{
		m_data.quaternion.x = 0;
		m_data.quaternion.y = 0;
		m_data.quaternion.z = 0;
		m_data.quaternion.w = 1;
	}

	Quaternion::Quaternion( Quaternion const & p_q )
		:	Coords4r( m_data.buffer	)
	{
		m_data.quaternion.x = p_q.x();
		m_data.quaternion.y = p_q.y();
		m_data.quaternion.z = p_q.z();
		m_data.quaternion.w = p_q.w();
	}

	Quaternion::Quaternion( Quaternion && p_q )
		:	Coords4r( m_data.buffer	)
	{
		m_data.quaternion.x = p_q.x();
		m_data.quaternion.y = p_q.y();
		m_data.quaternion.z = p_q.z();
		m_data.quaternion.w = p_q.w();
		p_q.m_data.quaternion.x = 0;
		p_q.m_data.quaternion.y = 0;
		p_q.m_data.quaternion.z = 0;
		p_q.m_data.quaternion.w = 0;
	}

	Quaternion::Quaternion( real const * p_q )
		:	Coords4r( m_data.buffer	)
	{
		m_data.quaternion.x = p_q[0];
		m_data.quaternion.y = p_q[1];
		m_data.quaternion.z = p_q[2];
		m_data.quaternion.w = p_q[3];
	}

	Quaternion::Quaternion( Point4r const & p_ptValues )
		:	Coords4r( m_data.buffer	)
	{
		m_data.quaternion.x = p_ptValues[0];
		m_data.quaternion.y = p_ptValues[1];
		m_data.quaternion.z = p_ptValues[2];
		m_data.quaternion.w = p_ptValues[3];
	}

	Quaternion::Quaternion( Point3r const & p_vector, Angle const & p_angle )
		:	Coords4r( m_data.buffer	)
	{
		FromAxisAngle( p_vector, p_angle );
	}

	Quaternion::~Quaternion()
	{
	}

	Quaternion & Quaternion::operator =( Quaternion const & p_q )
	{
		m_data.quaternion.x = p_q.x();
		m_data.quaternion.y = p_q.y();
		m_data.quaternion.z = p_q.z();
		m_data.quaternion.w = p_q.w();
		return *this;
	}

	Quaternion & Quaternion::operator =( Quaternion && p_q )
	{
		if ( this != &p_q )
		{
			m_data.quaternion.x = p_q.x();
			m_data.quaternion.y = p_q.y();
			m_data.quaternion.z = p_q.z();
			m_data.quaternion.w = p_q.w();
			p_q.m_data.quaternion.x = 0;
			p_q.m_data.quaternion.y = 0;
			p_q.m_data.quaternion.z = 0;
			p_q.m_data.quaternion.w = 0;
		}

		return *this;
	}

	Quaternion & Quaternion::operator +=( Quaternion const & p_q )
	{
		m_data.quaternion.x += p_q.m_data.quaternion.x;
		m_data.quaternion.y += p_q.m_data.quaternion.y;
		m_data.quaternion.z += p_q.m_data.quaternion.z;
		m_data.quaternion.w += p_q.m_data.quaternion.w;
		point::normalise( *this );
		return * this;
	}

	Quaternion & Quaternion::operator -=( Quaternion const & p_q )
	{
		m_data.quaternion.x -= p_q.m_data.quaternion.x;
		m_data.quaternion.y -= p_q.m_data.quaternion.y;
		m_data.quaternion.z -= p_q.m_data.quaternion.z;
		m_data.quaternion.w -= p_q.m_data.quaternion.w;
		point::normalise( *this );
		return * this;
	}

	Quaternion & Quaternion::operator *=( Quaternion const & p_q )
	{
		double const l_x = m_data.quaternion.x;
		double const l_y = m_data.quaternion.y;
		double const l_z = m_data.quaternion.z;
		double const l_w = m_data.quaternion.w;
		double const l_qx = p_q.m_data.quaternion.x;
		double const l_qy = p_q.m_data.quaternion.y;
		double const l_qz = p_q.m_data.quaternion.z;
		double const l_qw = p_q.m_data.quaternion.w;
		m_data.quaternion.x = real( l_w * l_qx + l_x * l_qw + l_y * l_qz - l_z * l_qy );
		m_data.quaternion.y = real( l_w * l_qy + l_y * l_qw + l_z * l_qx - l_x * l_qz );
		m_data.quaternion.z = real( l_w * l_qz + l_z * l_qw + l_x * l_qy - l_y * l_qx );
		m_data.quaternion.w = real( l_w * l_qw - l_x * l_qx - l_y * l_qy - l_z * l_qz );
		return * this;
	}

	Quaternion & Quaternion::operator *=( real p_rScalar )
	{
		m_data.quaternion.x *= p_rScalar;
		m_data.quaternion.y *= p_rScalar;
		m_data.quaternion.z *= p_rScalar;
		m_data.quaternion.w *= p_rScalar;
		point::normalise( *this );
		return * this;
	}

	Point3r & Quaternion::Transform( Point3r const & p_vector, Point3r & p_ptResult )const
	{
		/*
			Point3r l_ptTmp( p_vector );
			double l_dDist = point::distance( l_ptTmp );
			point::normalise( l_ptTmp );

			Quaternion l_qVec, l_qRes;
			l_qVec[0] = l_ptTmp[0];
			l_qVec[1] = l_ptTmp[1];
			l_qVec[2] = l_ptTmp[2];
			l_qVec[3] = real( 0.0 );

			l_qRes = l_qVec * GetConjugate();
			l_qRes = *this * l_qRes;

			p_ptResult[0] = l_qRes[0];
			p_ptResult[1] = l_qRes[1];
			p_ptResult[2] = l_qRes[2];

			p_ptResult *= l_dDist;
		/**/
		/*
			real a = at( 3 );
			real b = at( 0 );
			real c = at( 1 );
			real d = at( 2 );
			real t2 =   a*b;
			real t3 =   a*c;
			real t4 =   a*d;
			real t5 =  -b*b;
			real t6 =   b*c;
			real t7 =   b*d;
			real t8 =  -c*c;
			real t9 =   c*d;
			real t10 = -d*d;
			p_ptResult[0] = 2*( (t8 + t10)*p_vector[0] + (t6 -  t4)*p_vector[1] + (t3 + t7)*p_vector[2] ) + p_vector[0];
			p_ptResult[1] = 2*( (t4 +  t6)*p_vector[0] + (t5 + t10)*p_vector[1] + (t9 - t2)*p_vector[2] ) + p_vector[1];
			p_ptResult[2] = 2*( (t7 -  t3)*p_vector[0] + (t2 +  t9)*p_vector[1] + (t5 + t8)*p_vector[2] ) + p_vector[2];
		/**/
		/**/
		Point3r u( x(), y(), z() );
		Point3r uv( u ^ p_vector );
		Point3r uuv( u ^ uv );
		uv *= 2 * w();
		uuv *= 2;
		p_ptResult = p_vector + uv + uuv;
		/**/
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

	void Quaternion::FromAxisAngle( Point3r const & p_vector, Angle const & p_angle )
	{
		Angle l_halfAngle = p_angle * real( 0.5 );
		Point3r l_norm = point::get_normalised( p_vector ) * l_halfAngle.Sin();
		m_data.quaternion.x = l_norm[0];
		m_data.quaternion.y = l_norm[1];
		m_data.quaternion.z = l_norm[2];
		m_data.quaternion.w = l_halfAngle.Cos();
	}

	void Quaternion::ToAxisAngle( Point3r & p_vector, Angle & p_angle )const
	{
		real const & x = m_data.quaternion.x;
		real const & y = m_data.quaternion.y;
		real const & z = m_data.quaternion.z;
		real const & w = m_data.quaternion.w;
		real l_rSqrLength = x * x + y * y + z * z;

		if ( l_rSqrLength > 0.0 )
		{
			p_angle = Angle::FromRadians( real( 2.0 ) * acos( w ) );
			real l_rSin = p_angle.Sin();
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

	void Quaternion::FromAxes( Point3r const & p_x, Point3r const & p_y, Point3r const & p_z )
	{
		Matrix4x4r l_mtxRot;
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

	void Quaternion::ToAxes( Point3r & p_x, Point3r & p_y, Point3r & p_z )const
	{
		Matrix4x4r l_mtxRot;
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

	Angle Quaternion::GetYaw()const
	{
		real x = m_data.quaternion.x;
		real y = m_data.quaternion.y;
		real z = m_data.quaternion.z;
		real w = m_data.quaternion.w;
		return Angle::FromRadians( asin( -2 * ( x * z - w * y ) ) );
	}

	Angle Quaternion::GetPitch()const
	{
		real x = m_data.quaternion.x;
		real y = m_data.quaternion.y;
		real z = m_data.quaternion.z;
		real w = m_data.quaternion.w;
		return Angle::FromRadians( atan2( 2 * ( y * z + w * x ), w * w - x * x - y * y + z * z ) );
	}

	Angle Quaternion::GetRoll()const
	{
		real x = m_data.quaternion.x;
		real y = m_data.quaternion.y;
		real z = m_data.quaternion.z;
		real w = m_data.quaternion.w;
		return Angle::FromRadians( atan2( 2 * ( x * y + w * z ), w * w + x * x - y * y - z * z ) );
	}

	void Quaternion::Conjugate()
	{
		real w = at( 3 );
		point::negate( *this );
		at( 3 ) = w;
	}

	Quaternion Quaternion::GetConjugate()const
	{
		Quaternion l_qReturn( *this );
		l_qReturn.Conjugate();
		return l_qReturn;
	}

	real Quaternion::GetMagnitude()const
	{
		return real( point::distance( *this ) );
	}

	Quaternion Quaternion::Slerp( Quaternion const & p_target, real p_percent, bool p_shortestPath )const
	{
		//	Slerp = q1((q1^-1)q2)^t;
		real fCos = point::dot( *this, p_target );
		Quaternion rkT;

		// Do we need to invert rotation?
		if ( fCos < 0.0f && p_shortestPath )
		{
			fCos = -fCos;
			rkT = -p_target;
		}
		else
		{
			rkT = p_target;
		}

		if ( abs( fCos ) < 1 - 1e-03 )
		{
			// Standard case (slerp)
			real fSin = sqrt( 1 - ( fCos * fCos ) );
			real fAngle = atan2( fSin, fCos );
			real fInvSin = 1.0f / fSin;
			real fCoeff0 = sin( ( 1.0f - p_percent ) * fAngle ) * fInvSin;
			real fCoeff1 = sin( p_percent * fAngle ) * fInvSin;
			return ( fCoeff0 * ( * this ) + fCoeff1 * rkT );
		}
		else
		{
			// There are two situations:
			// 1. "rkP" and "rkQ" are very close (fCos ~= +1), so we can do a linear
			//    interpolation safely.
			// 2. "rkP" and "rkQ" are almost inverse of each other (fCos ~= -1), there
			//    are an infinite number of possibilities interpolation. but we haven't
			//    have method to fix this case, so just use linear interpolation here.
			Quaternion t = ( ( 1.0f - p_percent ) * ( * this ) + p_percent * rkT );
			// taking the complement requires renormalisation
			point::normalise( t );
			return t;
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

	Quaternion operator *( Quaternion const & p_q, real p_rScalar )
	{
		Quaternion l_qReturn( p_q );
		l_qReturn *= p_rScalar;
		return l_qReturn;
	}

	Quaternion operator *( real p_rScalar, Quaternion const & p_q )
	{
		Quaternion l_qReturn( p_q );
		l_qReturn *= p_rScalar;
		return l_qReturn;
	}

	Quaternion operator -( Quaternion const & p_q )
	{
		Quaternion l_qReturn( p_q );
		l_qReturn[3] = -l_qReturn[3];
		return l_qReturn;
	}
}
