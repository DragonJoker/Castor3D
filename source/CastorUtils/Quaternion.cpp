#include "CastorUtils/PrecompiledHeader.h"

#include "CastorUtils/Quaternion.h"
#include "CastorUtils/Point.h"
#include "CastorUtils/Angle.h"
//#include "CastorUtils/FastMath.h"

#if CHECK_MEMORYLEAKS
#	include "CastorUtils/Memory.h"
using namespace Castor::Utils;
#endif

using namespace Castor::Math;
using namespace Castor::Utils;

const Quaternion Quaternion :: Identity = Quaternion();
const Quaternion Quaternion :: Null = Quaternion( 0.0, 0.0, 0.0, 0.0);

Quaternion :: Quaternion()
	:	Point4r( 0, 0, 0, 1)
{
}

Quaternion :: Quaternion( const Quaternion & p_q)
	:	Point4r( p_q)
{
}

Quaternion :: Quaternion( const Point4r & p_q)
	:	Point4r( p_q)
{
}

Quaternion :: ~Quaternion()
{
}

void Quaternion :: Conjugate()
{
	Reverse();
	Normalise();
}

QuaternionPtr Quaternion :: GetConjugate()
{
	Normalise();
	QuaternionPtr q( new Quaternion( * this));
	q->Reverse();
	q->m_coords[3] = m_coords[3];
	return q;
}

real Quaternion :: GetMagnitude()
{
	return Point4r::GetLength();
}

void Quaternion :: Normalise()
{
	Point4r::Normalise();
}

void Quaternion :: ToRotationMatrix( Matrix4x4r & p_matrix)const
{
	p_matrix.Identity();
	rotate( p_matrix, * this);
}

void Quaternion :: ToRotationMatrix( real * p_matrix)const
{
/**/
	real x = m_coords[0];
	real y = m_coords[1];
	real z = m_coords[2];
	real w = m_coords[3];

	p_matrix[0 * 4 + 0] = 1.0f - 2.0f * ( y * y + z * z );
	p_matrix[0 * 4 + 1] = 2.0f * ( x * y - z * w );
	p_matrix[0 * 4 + 2] = 2.0f * ( x * z + y * w );
	p_matrix[0 * 4 + 3] = 0;

	p_matrix[1 * 4 + 0] = 2.0f * (x * y + z * w);
	p_matrix[1 * 4 + 1] = 1.0f - 2.0f * ( x * x + z * z );
	p_matrix[1 * 4 + 2] = 2.0f * ( y * z - x * w );
	p_matrix[1 * 4 + 3] = 0;

	p_matrix[2 * 4 + 0] = 2.0f * (x * z - y * w);
	p_matrix[2 * 4 + 1] = 2.0f * (z * y + x * w );
	p_matrix[2 * 4 + 2] = 1.0f - 2.0f * ( x * x + y * y );
	p_matrix[2 * 4 + 3] = 0;

	p_matrix[3 * 4 + 0] = 0.0f;
	p_matrix[3 * 4 + 1] = 0.0f;
	p_matrix[3 * 4 + 2] = 0.0f;
	p_matrix[3 * 4 + 3] = 1;
/**/
/*
	real a = Angle::DegreesToRadians * m_coords[3];
	real c = cos( a);
	real s = sin( a);

	Point3r l_axis( m_coords[0], m_coords[1], m_coords[2]);
	Point3r l_temp = (real( 1) - c) * l_axis;

	p_matrix[0 * 4 + 0] = c + l_temp[0] * l_axis[0];
	p_matrix[0 * 4 + 1] = 0 + l_temp[0] * l_axis[1] + s * l_axis[2];
	p_matrix[0 * 4 + 2] = 0 + l_temp[0] * l_axis[2] - s * l_axis[1];
	p_matrix[0 * 4 + 3] = 0;

	p_matrix[1 * 4 + 0] = 0 + l_temp[1] * l_axis[0] - s * l_axis[2];
	p_matrix[1 * 4 + 1] = c + l_temp[1] * l_axis[1];
	p_matrix[1 * 4 + 2] = 0 + l_temp[1] * l_axis[2] + s * l_axis[0];
	p_matrix[1 * 4 + 3] = 0;

	p_matrix[2 * 4 + 0] = 0 + l_temp[2] * l_axis[0] + s * l_axis[1];
	p_matrix[2 * 4 + 1] = 0 + l_temp[2] * l_axis[1] - s * l_axis[0];
	p_matrix[2 * 4 + 2] = c + l_temp[2] * l_axis[2];
	p_matrix[2 * 4 + 3] = 0;

	p_matrix[3 * 4 + 0] = 0;
	p_matrix[3 * 4 + 1] = 0;
	p_matrix[3 * 4 + 2] = 0;
	p_matrix[3 * 4 + 3] = 1;
/**/
}

void Quaternion :: FromRotationMatrix( const Matrix4x4r & p_matrix)
{
	real x = m_coords[0];
	real y = m_coords[1];
	real z = m_coords[2];
	real w = m_coords[3];
	real trace_l = p_matrix.GetTrace();

	if(trace_l > 0)
	{
		real s_l = 1.0f / (2.0f * sqrt(trace_l));

		x = (p_matrix[1][2] - p_matrix[2][1])*s_l; 
		y = (p_matrix[2][0] - p_matrix[0][2])*s_l; 
		z = (p_matrix[0][1] - p_matrix[1][0])*s_l; 
		w = 1.0f / (4.0f * s_l);
	}
	else
	{
		if(p_matrix[0][0] > p_matrix[1][1] && 
			p_matrix[0][0] > p_matrix[2][2])
		{
			real s_l = sqrt( 1+p_matrix[0][0]-p_matrix[1][1]-p_matrix[2][2])*2;

			x = 1.0f/(2.0f * s_l);
			y = (p_matrix[1][0] - p_matrix[0][1])/s_l;
			z = (p_matrix[2][0] - p_matrix[0][2])/s_l;
			w = (p_matrix[2][1] - p_matrix[1][2])/s_l;

		}
		else if(p_matrix[1][1] > p_matrix[0][0] && 
			p_matrix[1][1] > p_matrix[2][2])
		{
			real s_l = sqrt( 1-p_matrix[0][0]+p_matrix[1][1]-p_matrix[2][2])*2;
			x = (p_matrix[1][0] - p_matrix[0][1])/s_l;
			y = 1.0f/(2.0f * s_l);
			z = (p_matrix[2][1] - p_matrix[1][2])/s_l;
			w = (p_matrix[2][0] - p_matrix[0][2])/s_l;
		}
		else
		{
			real s_l = sqrt( 1-p_matrix[0][0]-p_matrix[1][1]+p_matrix[2][2])*2;
			x = (p_matrix[2][0] - p_matrix[0][2])/s_l;
			y = (p_matrix[2][1] - p_matrix[1][2])/s_l;
			z = 1.0f/(2.0f * s_l);
			w = (p_matrix[1][0] - p_matrix[0][1])/s_l;

		}
	}

	m_coords[0] = x;
	m_coords[1] = y;
	m_coords[2] = z;
	m_coords[3] = w;

	Normalise();
}

void Quaternion :: FromRotationMatrix( real * p_matrix)
{
	real x = m_coords[0];
	real y = m_coords[1];
	real z = m_coords[2];
	real w = m_coords[3];
	real l_trace = p_matrix[0] + p_matrix[5] + p_matrix[10];

	if (l_trace > 0.0f)
	{
		real l_s = 1.0f / (2.0f * sqrt( l_trace));

		x = (p_matrix[9] - p_matrix[6]) * l_s; 
		y = (p_matrix[2] - p_matrix[8]) * l_s; 
		z = (p_matrix[4] - p_matrix[1]) * l_s; 
		w = 1.0f / (4.0f * l_s);
	}
	else
	{
		if (p_matrix[0] > p_matrix[5] &&  p_matrix[0] > p_matrix[10])
		{
			real l_s = sqrt( 1 + p_matrix[0] - p_matrix[5] - p_matrix[10]) * 2.0f;

			x = 1.0f / (2.0f * l_s);
			y = (p_matrix[1] - p_matrix[4]) / l_s;
			z = (p_matrix[2] - p_matrix[8]) / l_s;
			w = (p_matrix[6] - p_matrix[9]) / l_s;

		}
		else if(p_matrix[5] > p_matrix[0] &&  p_matrix[5] > p_matrix[10])
		{
			real l_s = sqrt( 1 - p_matrix[0] + p_matrix[5] - p_matrix[10]) * 2;
			x = (p_matrix[1] - p_matrix[4]) / l_s;
			y = 1.0f / (2.0f * l_s);
			z = (p_matrix[6] - p_matrix[9]) / l_s;
			w = (p_matrix[2] - p_matrix[8]) / l_s;
		}
		else
		{
			real l_s = sqrt( 1 - p_matrix[0] - p_matrix[5] + p_matrix[10]) * 2;
			x = (p_matrix[2] - p_matrix[8]) / l_s;
			y = (p_matrix[6] - p_matrix[9]) / l_s;
			z = 1.0f / (2.0f * l_s);
			w = (p_matrix[1] - p_matrix[4]) / l_s;

		}
	}

	m_coords[0] = x;
	m_coords[1] = y;
	m_coords[2] = z;
	m_coords[3] = w;

	Normalise();
}

void Quaternion :: FromAxisAngle( const Point3r & p_vector, const Angle & p_angle)
{
	Point3r l_normalised = p_vector.GetNormalised();
	real l_angle = p_angle.Radians();
	real l_result = sin( l_angle / real( 2.0));
	// Calculate the x, y and z of the quaternion
	m_coords[0] = l_normalised[0] * l_result;
	m_coords[1] = l_normalised[1] * l_result;
	m_coords[2] = l_normalised[2] * l_result;
	m_coords[3] = cos( l_angle / real( 2.0));
	Normalise();
}

void Quaternion :: ToAxisAngle( Point3r & p_vector, Angle & p_angle)
{
	Normalise();

	real x = m_coords[0];
	real y = m_coords[1];
	real z = m_coords[2];
	real w = m_coords[3];

	p_angle = acos( w) * 2;

	p_vector[0] = x;
	p_vector[1] = y;
	p_vector[2] = z;
	p_vector.Normalise();
}

Angle Quaternion :: GetYaw()
{
	real x = m_coords[0];
	real y = m_coords[1];
	real z = m_coords[2];
	real w = m_coords[3];
	return asin( -2 * (x * z - w * y));
}

Angle Quaternion :: GetPitch()
{
	real x = m_coords[0];
	real y = m_coords[1];
	real z = m_coords[2];
	real w = m_coords[3];
	return atan2( 2 * (y * z + w * x), w * w - x * x - y * y + z * z);
}

Angle Quaternion :: GetRoll()
{
	real x = m_coords[0];
	real y = m_coords[1];
	real z = m_coords[2];
	real w = m_coords[3];
	return atan2( 2 * (x * y + w * z), w * w + x * x - y * y - z * z);
}

Quaternion Quaternion :: operator +( const Quaternion & q) const
{
	real x = m_coords[0];
	real y = m_coords[1];
	real z = m_coords[2];
	real w = m_coords[3];
	real qx = q[0];
	real qy = q[1];
	real qz = q[2];
	real qw = q[3];
    return Quaternion( w + qw, x + qx, y + qy, z + qz);
}

Quaternion Quaternion :: operator -( const Quaternion & q) const
{
	real x = m_coords[0];
	real y = m_coords[1];
	real z = m_coords[2];
	real w = m_coords[3];
	real qx = q[0];
	real qy = q[1];
	real qz = q[2];
	real qw = q[3];
    return Quaternion( w - qw, x - qx, y - qy, z - qz);
}

Quaternion Quaternion :: operator *( real rScalar) const
{
	real x = m_coords[0];
	real y = m_coords[1];
	real z = m_coords[2];
	real w = m_coords[3];
	return Quaternion( rScalar * w, rScalar * x, rScalar * y, rScalar * z);
}

Quaternion Quaternion :: operator *( const Quaternion & q)const
{
	real x = m_coords[0];
	real y = m_coords[1];
	real z = m_coords[2];
	real w = m_coords[3];
	real qx = q[0];
	real qy = q[1];
	real qz = q[2];
	real qw = q[3];

    Quaternion r;
    r[3] = w * qw - x * qx - y * qy - z * qz;
    r[0] = w * qx + x * qw + y * qz - z * qy;
    r[1] = w * qy + y * qw + z * qx - x * qz;
    r[2] = w * qz + z * qw + x * qy - y * qx;
    return r;
}

void Quaternion :: operator *=( const Quaternion & q)
{
	real x = m_coords[0];
	real y = m_coords[1];
	real z = m_coords[2];
	real w = m_coords[3];
	real l_x = x;
	real l_y = y;
	real l_z = z;
	real l_w = w;
	real qx = q[0];
	real qy = q[1];
	real qz = q[2];
	real qw = q[3];

    w = l_w * qw - l_x * qx - l_y * qy - l_z * qz;
    x = l_w * qx + l_x * qw + l_y * qz - l_z * qy;
    y = l_w * qy + l_y * qw + l_z * qx - l_x * qz;
	z = l_w * qz + l_z * qw + l_x * qy - l_y * qx;

	m_coords[0] = x;
	m_coords[1] = y;
	m_coords[2] = z;
	m_coords[3] = w;
	Normalise();
}

Quaternion & Quaternion :: operator =( const Quaternion & p_q)
{
	Point4r::operator =( p_q);
	Normalise();
	return *this;
}

Quaternion Quaternion :: operator -() const
{
	real x = m_coords[0];
	real y = m_coords[1];
	real z = m_coords[2];
	real w = m_coords[3];
    return Quaternion( -w, -x, -y, -z);
}

Point3r Quaternion::operator *( const Point3r & p_vector)const
{
	real x = m_coords[0];
	real y = m_coords[1];
	real z = m_coords[2];
	real w = m_coords[3];

	Point3r uv, uuv;
	Point3r u( x, y, z);
	uv = u ^ p_vector;
	uuv = u ^ uv;
	uv *= (2.0f * w);
	uuv *= 2.0f;

	return p_vector + uv + uuv;
}

bool Quaternion :: Write( File & p_file)const
{
	real x = m_coords[0];
	real y = m_coords[1];
	real z = m_coords[2];
	real w = m_coords[3];

	if ( ! p_file.Write<real>( x))
	{
		return false;
	}
	if ( ! p_file.Write<real>( y))
	{
		return false;
	}
	if ( ! p_file.Write<real>( z))
	{
		return false;
	}
	if ( ! p_file.Write<real>( w))
	{
		return false;
	}
	return true;
}

bool Quaternion :: Read( File & p_file)
{
	real x;
	real y;
	real z;
	real w;

	if ( ! p_file.Read<real>( x))
	{
		return false;
	}
	if ( ! p_file.Read<real>( y))
	{
		return false;
	}
	if ( ! p_file.Read<real>( z))
	{
		return false;
	}
	if ( ! p_file.Read<real>( w))
	{
		return false;
	}

	m_coords[0] = x;
	m_coords[1] = y;
	m_coords[2] = z;
	m_coords[3] = w;
	Normalise();

	return true;
}

real Quaternion :: Dot( const Quaternion & p_quat) const
{
	return Point4r::Dot( p_quat);
}

Quaternion Quaternion :: Slerp( const Quaternion & p_target, real p_percent, bool p_shortestPath)
{
//	Slerp = q1((q1^-1)q2)^t;
    real fCos = Dot( p_target);
    Quaternion rkT;

    // Do we need to invert rotation?
    if (fCos < 0.0f && p_shortestPath)
    {
        fCos = -fCos;
        rkT = -p_target;
    }
    else
    {
        rkT = p_target;
    }

    if (abs( fCos) < 1 - 1e-03)
    {
        // Standard case (slerp)
        real fSin = sqrt(1 - (fCos * fCos));
        real fAngle = atan2( fSin, fCos);
        real fInvSin = 1.0f / fSin;
        real fCoeff0 = sin( (1.0f - p_percent) * fAngle) * fInvSin;
        real fCoeff1 = sin( p_percent * fAngle) * fInvSin;

        return ( fCoeff0 * (* this) + fCoeff1 * rkT);
    }
    else
    {
        // There are two situations:
        // 1. "rkP" and "rkQ" are very close (fCos ~= +1), so we can do a linear
        //    interpolation safely.
        // 2. "rkP" and "rkQ" are almost inverse of each other (fCos ~= -1), there
        //    are an infinite number of possibilities interpolation. but we haven't
        //    have method to fix this case, so just use linear interpolation here.
        Quaternion t = ( (1.0f - p_percent) * (* this) + p_percent * rkT);
        // taking the complement requires renormalisation
        t.Normalise();
        return t;
    }
}

Quaternion Castor::Math :: operator *( real rScalar, const Quaternion & rkQ)
{
	Quaternion l_qReturn = rkQ * rScalar;
    return l_qReturn;
}