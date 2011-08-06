#include "CastorUtils/PrecompiledHeader.hpp"

#include "CastorUtils/Quaternion.hpp"
#include "CastorUtils/Point.hpp"
#include "CastorUtils/Angle.hpp"
#include "CastorUtils/Matrix.hpp"
#include "CastorUtils/TransformationMatrix.hpp"

#if CHECK_MEMORYLEAKS
#	include "CastorUtils/Memory.hpp"
using namespace Castor::Utils;
#endif

using namespace Castor;
using namespace Castor::Math;
using namespace Castor::Utils;
using namespace Castor::Resources;

//*************************************************************************************************

bool Loader<Quaternion> :: Load( Quaternion & p_object, File & p_file)
{
	bool l_bReturn = true;

	for (size_t i = 0 ; i < 4 && l_bReturn ; i++)
	{
		l_bReturn = p_file.Read( p_object[i]) == sizeof( real);
	}

	return l_bReturn;
}

bool Loader<Quaternion> :: Save( Quaternion const & p_object, File & p_file)
{
	bool l_bReturn = true;

	for (size_t i = 0 ; i < 4 && l_bReturn ; i++)
	{
		l_bReturn = p_file.Write( p_object[i]) == sizeof( real);
	}

	return l_bReturn;
}	

bool Loader<Quaternion> :: Read( Quaternion & p_object, File & p_file)
{
	String l_strWord;
	bool l_bReturn = p_file.ReadLine( l_strWord, 1024, cuT( " \r\n;\t")) > 0 && l_strWord.is_floating();
	xchar l_cDump;

	if (l_bReturn)
	{
		p_object[0] = l_strWord.to_float();
		p_file.ReadChar( l_cDump);
		l_bReturn = p_file.ReadLine( l_strWord, 1024, cuT( " \r\n;\t")) > 0 && l_strWord.is_floating();
	}

	if (l_bReturn)
	{
		p_object[1] = l_strWord.to_float();
		p_file.ReadChar( l_cDump);
		l_bReturn = p_file.ReadLine( l_strWord, 1024, cuT( " \r\n;\t")) > 0 && l_strWord.is_floating();
	}

	if (l_bReturn)
	{
		p_object[2] = l_strWord.to_float();
		p_file.ReadChar( l_cDump);
		l_bReturn = p_file.ReadLine( l_strWord, 1024, cuT( " \r\n;\t")) > 0 && l_strWord.is_floating();
	}

	if (l_bReturn)
	{
		p_object[3] = l_strWord.to_float();
	}

	return l_bReturn;
}

bool Loader<Quaternion> :: Write( Quaternion const & p_object, File & p_file)
{
	String l_strWord;
	bool l_bReturn = p_file.Print( 1024, cuT( "%f %f %f %f"), p_object[0], p_object[1], p_object[2]) > 0;
	return l_bReturn;
}

//*************************************************************************************************

const Quaternion Quaternion :: Identity = Quaternion();
const Quaternion Quaternion :: Null = Quaternion( 0.0, 0.0, 0.0, 0.0);

Quaternion :: Quaternion()
	:	Point4r( 0, 0, 0, 1)
{
}

Quaternion :: Quaternion( Quaternion const & p_q)
	:	Point4r( p_q[0], p_q[1], p_q[2], p_q[3])
{
}

Quaternion :: Quaternion( Point4r const & p_q)
	:	Point4r( p_q[0], p_q[1], p_q[2], p_q[3])
{
}

Quaternion :: ~Quaternion()
{
}

void Quaternion :: Conjugate()
{
	negate();
	Normalise();
}

QuaternionPtr Quaternion :: GetConjugate()
{
	Normalise();
	QuaternionPtr q( new Quaternion( * this));
	q->negate();
	q->at( 3) = at( 3);
	return q;
}

real Quaternion :: GetMagnitude()
{
	return float( Point4r::length());
}

void Quaternion :: Normalise()
{
	Point4r::normalise();
}

void Quaternion :: ToRotationMatrix( Matrix4x4r & p_matrix)const
{
	p_matrix.SetIdentity();
	MtxUtils::rotate( p_matrix, * this);
}

void Quaternion :: ToRotationMatrix( real * p_matrix)const
{
/**/
	real x = at( 0);
	real y = at( 1);
	real z = at( 2);
	real w = at( 3);

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
	real a = Angle::DegreesToRadians * at( 3);
	real c = cos( a);
	real s = sin( a);

	Point3r l_axis( at( 0), at( 1), at( 2));
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

void Quaternion :: FromRotationMatrix( Matrix4x4r const & p_matrix)
{
	real x = at( 0);
	real y = at( 1);
	real z = at( 2);
	real w = at( 3);
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

	at( 0) = x;
	at( 1) = y;
	at( 2) = z;
	at( 3) = w;

	Normalise();
}

void Quaternion :: FromRotationMatrix( real * p_matrix)
{
	real x = at( 0);
	real y = at( 1);
	real z = at( 2);
	real w = at( 3);
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

	at( 0) = x;
	at( 1) = y;
	at( 2) = z;
	at( 3) = w;

	Normalise();
}

void Quaternion :: FromAxisAngle( Point3r const & p_vector, Angle const & p_angle)
{
	Point3r l_normalised;
	l_normalised.copy( p_vector.get_normalised());
	real l_angle = p_angle.Radians();
	real l_result = sin( l_angle / real( 2.0));
	// Calculate the x, y and z of the quaternion
	at( 0) = l_normalised[0] * l_result;
	at( 1) = l_normalised[1] * l_result;
	at( 2) = l_normalised[2] * l_result;
	at( 3) = cos( l_angle / real( 2.0));
	Normalise();
}

void Quaternion :: ToAxisAngle( Point3r & p_vector, Angle & p_angle)const
{
	real x = at( 0);
	real y = at( 1);
	real z = at( 2);
	real w = at( 3);

	p_angle = Angle::FromRadians( acos( w) * 2);

	p_vector[0] = x;
	p_vector[1] = y;
	p_vector[2] = z;
	p_vector.normalise();
}

Angle Quaternion :: GetYaw()const
{
	real x = at( 0);
	real y = at( 1);
	real z = at( 2);
	real w = at( 3);
	return Angle::FromRadians( asin( -2 * (x * z - w * y)));
}

Angle Quaternion :: GetPitch()const
{
	real x = at( 0);
	real y = at( 1);
	real z = at( 2);
	real w = at( 3);
	return Angle::FromRadians( atan2( 2 * (y * z + w * x), w * w - x * x - y * y + z * z));
}

Angle Quaternion :: GetRoll()const
{
	real x = at( 0);
	real y = at( 1);
	real z = at( 2);
	real w = at( 3);
	return Angle::FromRadians( atan2( 2 * (x * y + w * z), w * w + x * x - y * y - z * z));
}

Quaternion & Quaternion :: operator =( Quaternion const & p_q)
{
	copy( p_q);
	Normalise();
	return *this;
}

Quaternion & Quaternion :: operator +=( Quaternion const & p_q)
{
	at( 0) += p_q[0];
	at( 1) += p_q[1];
	at( 2) += p_q[2];
	at( 3) += p_q[3];
	Normalise();
    return * this;
}

Quaternion & Quaternion :: operator -=( Quaternion const & p_q)
{
	at( 0) -= p_q[0];
	at( 1) -= p_q[1];
	at( 2) -= p_q[2];
	at( 3) -= p_q[3];
	Normalise();
    return * this;
}

Quaternion & Quaternion :: operator *=( Quaternion const & p_q)
{
	real x = at( 0);
	real y = at( 1);
	real z = at( 2);
	real w = at( 3);
	real l_x = x;
	real l_y = y;
	real l_z = z;
	real l_w = w;
	real qx = p_q[0];
	real qy = p_q[1];
	real qz = p_q[2];
	real qw = p_q[3];

    w = l_w * qw - l_x * qx - l_y * qy - l_z * qz;
    x = l_w * qx + l_x * qw + l_y * qz - l_z * qy;
    y = l_w * qy + l_y * qw + l_z * qx - l_x * qz;
	z = l_w * qz + l_z * qw + l_x * qy - l_y * qx;

	at( 0) = x;
	at( 1) = y;
	at( 2) = z;
	at( 3) = w;
	Normalise();

	return * this;
}

Quaternion & Quaternion :: operator *=( real p_rScalar)
{
	at( 0) *= p_rScalar;
	at( 1) *= p_rScalar;
	at( 2) *= p_rScalar;
	at( 3) *= p_rScalar;
	Normalise();
	return * this;
}

real Quaternion :: Dot( Quaternion const & p_quat) const
{
	return Point4r::dot( p_quat);
}

void Quaternion :: Transform( Point3r const & p_vector, Point3r & p_ptResult)
{
	real x = at( 0);
	real y = at( 1);
	real z = at( 2);
	real w = at( 3);

	Point3r uv, uuv;
	Point3r u( x, y, z);
	uv.copy( u ^ p_vector);
	uuv.copy( u ^ uv);
	uv *= (2.0f * w);
	uuv *= 2.0f;

	p_ptResult.copy( p_vector + uv + uuv);
}

bool Quaternion :: Save( File & p_file)const
{
	real x = at( 0);
	real y = at( 1);
	real z = at( 2);
	real w = at( 3);

	if ( ! p_file.Write( x))
	{
		return false;
	}
	if ( ! p_file.Write( y))
	{
		return false;
	}
	if ( ! p_file.Write( z))
	{
		return false;
	}
	if ( ! p_file.Write( w))
	{
		return false;
	}
	return true;
}

bool Quaternion :: Load( File & p_file)
{
	real x;
	real y;
	real z;
	real w;

	if ( ! p_file.Read( x))
	{
		return false;
	}
	if ( ! p_file.Read( y))
	{
		return false;
	}
	if ( ! p_file.Read( z))
	{
		return false;
	}
	if ( ! p_file.Read( w))
	{
		return false;
	}

	at( 0) = x;
	at( 1) = y;
	at( 2) = z;
	at( 3) = w;
	Normalise();

	return true;
}

Quaternion Quaternion :: Slerp( Quaternion const & p_target, real p_percent, bool p_shortestPath)
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

Quaternion Castor::Math :: operator	+( Quaternion const & p_qA, Quaternion const & p_qB)
{
	Quaternion l_qReturn( p_qA);
	l_qReturn += p_qB;
	return l_qReturn;
}

Quaternion Castor::Math :: operator	-( Quaternion const & p_qA, Quaternion const & p_qB)
{
	Quaternion l_qReturn( p_qA);
	l_qReturn -= p_qB;
	return l_qReturn;
}

Quaternion Castor::Math :: operator	*( Quaternion const & p_qA, Quaternion const & p_qB)
{
	Quaternion l_qReturn( p_qA);
	l_qReturn *= p_qB;
	return l_qReturn;
}

Quaternion Castor::Math :: operator	*( Quaternion const & p_q, real p_rScalar)
{
	Quaternion l_qReturn( p_q);
	l_qReturn *= p_rScalar;
	return l_qReturn;
}

Quaternion Castor::Math :: operator *( real p_rScalar, Quaternion const & p_q)
{
	Quaternion l_qReturn( p_q);
	l_qReturn *= p_rScalar;
	return l_qReturn;
}

Quaternion Castor::Math :: operator	-( Quaternion const & p_q)
{
	Quaternion l_qReturn( p_q);
	l_qReturn.negate();
	return l_qReturn;
}

//*************************************************************************************************