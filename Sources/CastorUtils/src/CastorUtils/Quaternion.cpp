#include "Quaternion.h"
#include "RotationMatrix.h"
#include "Vector3f.h"

using namespace General::Math;

const Quaternion Quaternion :: Quat_Identity = Quaternion();
const Quaternion Quaternion :: Quat_Null = Quaternion( 0.0, 0.0, 0.0, 0.0);

Quaternion :: Quaternion()
	:	Point4D<float>( 0, 0, 0, 1)
{
}

Quaternion :: Quaternion( const Quaternion & p_q)
	:	Point4D<float>( p_q.x, p_q.y, p_q.z, p_q.w)
{
}

Quaternion :: ~Quaternion()
{
}

void Quaternion :: Conjugate()
{
	x = -x;
	y = -y;
	z = -z;
	Normalise();
}

Quaternion* Quaternion :: GetConjugate()
{
	Normalise();
	Quaternion *q = new Quaternion();
	q->w = w;
	q->x = -x;
	q->y = -y;
	q->z = -z;
	return q;
}

float Quaternion :: GetMagnitude()
{
    return sqrt( w * w + x * x + y * y + z * z);
}

void Quaternion :: Normalise()
{
	float norme_l = GetMagnitude();
	w /= norme_l;
	x /= norme_l;
	y /= norme_l;
	z /= norme_l;
}

void Quaternion :: ToRotationMatrix( RotationMatrix & p_matrix)
{
	// First row
	p_matrix.m_matrix[0][0] = 1.0f - 2.0f * ( y * y + z * z );
	p_matrix.m_matrix[0][1] = 2.0f * (x * y + z * w);
	p_matrix.m_matrix[0][2] = 2.0f * (x * z - y * w);
	p_matrix.m_matrix[0][3] = 0.0f;
	// Second row
	p_matrix.m_matrix[1][0] = 2.0f * ( x * y - z * w );
	p_matrix.m_matrix[1][1] = 1.0f - 2.0f * ( x * x + z * z );
	p_matrix.m_matrix[1][2] = 2.0f * (z * y + x * w );
	p_matrix.m_matrix[1][3] = 0.0f;
	// Third row
	p_matrix.m_matrix[2][0] = 2.0f * ( x * z + y * w );
	p_matrix.m_matrix[2][1] = 2.0f * ( y * z - x * w );
	p_matrix.m_matrix[2][2] = 1.0f - 2.0f * ( x * x + y * y );
	p_matrix.m_matrix[2][3] = 0.0f;
	// Fourth row
	p_matrix.m_matrix[3][0] = 0;
	p_matrix.m_matrix[3][1] = 0;
	p_matrix.m_matrix[3][2] = 0;
	p_matrix.m_matrix[3][3] = 1.0f;
}

void Quaternion :: ToRotationMatrix( float * p_matrix)const
{
	p_matrix[0] = 1.0f - 2.0f * ( y * y + z * z );
	p_matrix[1] = 2.0f * ( x * y - z * w );
	p_matrix[2] = 2.0f * ( x * z + y * w );
	p_matrix[3] = 0;
	p_matrix[4] = 2.0f * (x * y + z * w);
	p_matrix[5] = 1.0f - 2.0f * ( x * x + z * z );
	p_matrix[6] = 2.0f * ( y * z - x * w );
	p_matrix[7] = 0;
	p_matrix[8] = 2.0f * (x * z - y * w);
	p_matrix[9] = 2.0f * (z * y + x * w );
	p_matrix[10] = 1.0f - 2.0f * ( x * x + y * y );
	p_matrix[11] = 0;
	p_matrix[12] = 0.0f;
	p_matrix[13] = 0.0f;
	p_matrix[14] = 0.0f;
	p_matrix[15] = 1.0f;
}

void Quaternion :: FromRotationMatrix( RotationMatrix * p_matrix)
{
	float trace_l = p_matrix->GetTrace();

	if(trace_l > 0)
	{
		float s_l = 1.0f / (2.0f * sqrt(trace_l));

		x = (p_matrix->m_matrix[2][1] - p_matrix->m_matrix[1][2])*s_l; 
		y = (p_matrix->m_matrix[0][2] - p_matrix->m_matrix[2][0])*s_l; 
		z = (p_matrix->m_matrix[1][0] - p_matrix->m_matrix[0][1])*s_l; 
		w = 1.0f / (4.0f * s_l);
	}
	else
	{
		if(p_matrix->m_matrix[0][0] > p_matrix->m_matrix[1][1] && 
			p_matrix->m_matrix[0][0] > p_matrix->m_matrix[2][2])
		{
			float s_l = sqrt( 1+p_matrix->m_matrix[0][0]-p_matrix->m_matrix[1][1]-p_matrix->m_matrix[2][2])*2;

			x = 1.0f/(2.0f * s_l);
			y = (p_matrix->m_matrix[0][1] - p_matrix->m_matrix[1][0])/s_l;
			z = (p_matrix->m_matrix[0][2] - p_matrix->m_matrix[2][0])/s_l;
			w = (p_matrix->m_matrix[1][2] - p_matrix->m_matrix[2][1])/s_l;

		}
		else if(p_matrix->m_matrix[1][1] > p_matrix->m_matrix[0][0] && 
			p_matrix->m_matrix[1][1] > p_matrix->m_matrix[2][2])
		{
			float s_l = sqrt( 1-p_matrix->m_matrix[0][0]+p_matrix->m_matrix[1][1]-p_matrix->m_matrix[2][2])*2;
			x = (p_matrix->m_matrix[0][1] - p_matrix->m_matrix[1][0])/s_l;
			y = 1.0f/(2.0f * s_l);
			z = (p_matrix->m_matrix[1][2] - p_matrix->m_matrix[2][1])/s_l;
			w = (p_matrix->m_matrix[0][2] - p_matrix->m_matrix[2][0])/s_l;
		}
		else
		{
			float s_l = sqrt( 1-p_matrix->m_matrix[0][0]-p_matrix->m_matrix[1][1]+p_matrix->m_matrix[2][2])*2;
			x = (p_matrix->m_matrix[0][2] - p_matrix->m_matrix[2][0])/s_l;
			y = (p_matrix->m_matrix[1][2] - p_matrix->m_matrix[2][1])/s_l;
			z = 1.0f/(2.0f * s_l);
			w = (p_matrix->m_matrix[0][1] - p_matrix->m_matrix[1][0])/s_l;

		}
	}

	Normalise();
}

void Quaternion :: FromRotationMatrix( float * p_matrix)
{
	float l_trace = p_matrix[0] + p_matrix[5] + p_matrix[10];

	if (l_trace > 0.0f)
	{
		float l_s = 1.0f / (2.0f * sqrt( l_trace));

		x = (p_matrix[9] - p_matrix[6]) * l_s; 
		y = (p_matrix[2] - p_matrix[8]) * l_s; 
		z = (p_matrix[4] - p_matrix[1]) * l_s; 
		w = 1.0f / (4.0f * l_s);
	}
	else
	{
		if (p_matrix[0] > p_matrix[5] &&  p_matrix[0] > p_matrix[10])
		{
			float l_s = sqrt( 1 + p_matrix[0] - p_matrix[5] - p_matrix[10]) * 2.0f;

			x = 1.0f / (2.0f * l_s);
			y = (p_matrix[1] - p_matrix[4]) / l_s;
			z = (p_matrix[2] - p_matrix[8]) / l_s;
			w = (p_matrix[6] - p_matrix[9]) / l_s;

		}
		else if(p_matrix[5] > p_matrix[0] &&  p_matrix[5] > p_matrix[10])
		{
			float l_s = sqrt( 1 - p_matrix[0] + p_matrix[5] - p_matrix[10]) * 2;
			x = (p_matrix[1] - p_matrix[4]) / l_s;
			y = 1.0f / (2.0f * l_s);
			z = (p_matrix[6] - p_matrix[9]) / l_s;
			w = (p_matrix[2] - p_matrix[8]) / l_s;
		}
		else
		{
			float l_s = sqrt( 1 - p_matrix[0] - p_matrix[5] + p_matrix[10]) * 2;
			x = (p_matrix[2] - p_matrix[8]) / l_s;
			y = (p_matrix[6] - p_matrix[9]) / l_s;
			z = 1.0f / (2.0f * l_s);
			w = (p_matrix[1] - p_matrix[4]) / l_s;

		}
	}
	Normalise();
}

void Quaternion :: FromAxisAngle( const Vector3f & p_vector, float p_degrees)
{
	Vector3f l_normalised = p_vector.GetNormalised();
	float l_angle = (float)((p_degrees / 180.0f) * M_PI);
	float l_result = (float)sin( l_angle / 2.0f );
	// Calculate the x, y and z of the quaternion
	x = l_normalised.x * l_result;
	y = l_normalised.y * l_result;
	z = l_normalised.z * l_result;
	w = (float)cos( l_angle / 2.0f );
	Normalise();
}

void Quaternion :: ToAxisAngle( Vector3f & p_vector, float & p_angle)
{
	Normalise();

	p_angle = acos( w) * 2;

	p_vector.x = x;
	p_vector.y = y;
	p_vector.z = z;
	p_vector.Normalise();
}

float Quaternion :: GetYaw()
{
	return asin( -2 * (x * z - w * y));
}

float Quaternion :: GetPitch()
{
	return atan2( 2 * (y * z + w * x), w * w - x * x - y * y + z * z);
}

float Quaternion :: GetRoll()
{
	return atan2( 2 * (x * y + w * z), w * w + x * x - y * y - z * z);
}

Quaternion Quaternion :: operator +( const Quaternion & q) const
{
    return Quaternion( w + q.w, x + q.x, y + q.y, z + q.z);
}

Quaternion Quaternion :: operator -( const Quaternion & q) const
{
    return Quaternion( w - q.w, x - q.x, y - q.y, z - q.z);
}

Quaternion Quaternion :: operator *( const Quaternion & q)const
{
    Quaternion r;
    r.w = w * q.w - x * q.x - y * q.y - z * q.z;
    r.x = w * q.x + x * q.w + y * q.z - z * q.y;
    r.y = w * q.y + y * q.w + z * q.x - x * q.z;
    r.z = w * q.z + z * q.w + x * q.y - y * q.x;
    return r;
}

void Quaternion :: operator *=( const Quaternion & q)
{
	float l_x = x;
	float l_y = y;
	float l_z = z;
	float l_w = w;
    w = l_w*q.w - l_x*q.x - l_y*q.y - l_z*q.z;
    x = l_w*q.x + l_x*q.w + l_y*q.z - l_z*q.y;
    y = l_w*q.y + l_y*q.w + l_z*q.x - l_x*q.z;
    z = l_w*q.z + l_z*q.w + l_x*q.y - l_y*q.x;
	Normalise();
}

Quaternion & Quaternion :: operator =( const Quaternion & p_q)
{
	x = p_q.x;
	y = p_q.y;
	z = p_q.z;
	w = p_q.w;
	Normalise();
	return *this;
}

Quaternion Quaternion :: operator *( float fScalar) const
{
    return Quaternion( fScalar * w, fScalar * x, fScalar * y, fScalar * z);
}

Quaternion Quaternion :: operator -() const
{
    return Quaternion( -w, -x, -y, -z);
}

Vector3f Quaternion::operator *( const Vector3f & p_vector)const
{
	Vector3f uv, uuv;
	Vector3f u( x, y, z);
	uv = u ^ p_vector;
	uuv = u ^ uv;
	uv *= (2.0f * w);
	uuv *= 2.0f;

	return p_vector + uv + uuv;
}

bool Quaternion :: Write( File & p_file)const
{
	if ( ! p_file.Write<float>( x))
	{
		return false;
	}
	if ( ! p_file.Write<float>( y))
	{
		return false;
	}
	if ( ! p_file.Write<float>( z))
	{
		return false;
	}
	if ( ! p_file.Write<float>( w))
	{
		return false;
	}
	return true;
}

bool Quaternion :: Read( File & p_file)
{
	if ( ! p_file.Read<float>( x))
	{
		return false;
	}
	if ( ! p_file.Read<float>( y))
	{
		return false;
	}
	if ( ! p_file.Read<float>( z))
	{
		return false;
	}
	if ( ! p_file.Read<float>( w))
	{
		return false;
	}
	Normalise();
	return true;
}

float Quaternion :: Dot( const Quaternion & p_quat) const
{
    return  w * p_quat.w + x * p_quat.x + y * p_quat.y + z * p_quat.z;
}

Quaternion Quaternion :: Slerp( const Quaternion & p_target, float p_percent,
									bool p_shortestPath)
{
//	Slerp = q1((q1^-1)q2)^t;
    float fCos = Dot( p_target);
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
        float fSin = sqrt(1 - (fCos * fCos));
        float fAngle = atan2( fSin, fCos);
        float fInvSin = 1.0f / fSin;
        float fCoeff0 = sin( (1.0f - p_percent) * fAngle) * fInvSin;
        float fCoeff1 = sin( p_percent * fAngle) * fInvSin;

        return fCoeff0 * (*this) + fCoeff1 * rkT;
    }
    else
    {
        // There are two situations:
        // 1. "rkP" and "rkQ" are very close (fCos ~= +1), so we can do a linear
        //    interpolation safely.
        // 2. "rkP" and "rkQ" are almost inverse of each other (fCos ~= -1), there
        //    are an infinite number of possibilities interpolation. but we haven't
        //    have method to fix this case, so just use linear interpolation here.
        Quaternion t = (1.0f - p_percent) * (*this) + p_percent * rkT;
        // taking the complement requires renormalisation
        t.Normalise();
        return t;
    }
}

Quaternion General::Math :: operator * ( float fScalar, const Quaternion & rkQ)
{
    return Quaternion( fScalar * rkQ.w, fScalar * rkQ.x, fScalar * rkQ.y,
						 fScalar * rkQ.z);
}