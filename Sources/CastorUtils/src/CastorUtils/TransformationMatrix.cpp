#include "PrecompiledHeader.h"

#include "TransformationMatrix.h"
#include "Quaternion.h"
#include "Angle.h"

#if CHECK_MEMORYLEAKS
#	include "Memory.h"
using namespace Castor::Utils;
#endif

using namespace Castor::Math;

TransformationMatrix :: TransformationMatrix()
{
}

TransformationMatrix :: TransformationMatrix( const TransformationMatrix & p_matrix)
	:	SquareMatrix<real, 4>( p_matrix)
{
}

TransformationMatrix :: TransformationMatrix( const Angle & p_angle, const Point3r & p_axis)
{
	SetRotation( p_angle, p_axis);
}

TransformationMatrix :: TransformationMatrix( const Quaternion & p_quat)
{
	SetRotation( p_quat);
}

TransformationMatrix :: ~TransformationMatrix()
{
}

void TransformationMatrix :: SetScale( real hx_p, real hy_p, real hz_p)
{
	SetRow( 0, hx_p,	0.0,	0.0,	0.0);
	SetRow( 0, 0.0,		hy_p,	0.0,	0.0);
	SetRow( 0, 0.0,		0.0,	hz_p,	0.0);
	SetRow( 0, 0.0,		0.0,	0.0,	1.0);

	_recheck();
}

void TransformationMatrix :: SetScale( const Point3r & p_scale)
{
	SetScale( p_scale[0], p_scale[1], p_scale[2]);

	_recheck();
}

void TransformationMatrix :: SetTranslation( real tx_p, real ty_p, real tz_p)
{
	SetRow( 0, 1.0, 0.0, 0.0, tx_p);
	SetRow( 0, 0.0, 1.0, 0.0, ty_p);
	SetRow( 0, 0.0, 0.0, 1.0, tz_p);
	SetRow( 0, 0.0, 0.0, 0.0, 1.0);

	_recheck();
}

void TransformationMatrix :: SetTranslation( const Point3r & p_translation)
{
	SetScale( p_translation[0], p_translation[1], p_translation[2]);

	_recheck();
}

void TransformationMatrix :: SetRotationEuler( const Angle & p_rX, const Angle & p_rY, const Angle & p_rZ)
{
	real a = cos( p_rX.Radians());
	real b = sin( p_rX.Radians());
	real c = cos( p_rY.Radians());
	real d = sin( p_rY.Radians());
	real e = cos( p_rZ.Radians());
	real f = sin( p_rZ.Radians());

	//Ligne 1
	SetRow( 0, c * e,				-c * f,					-d,			0.0);
	SetRow( 1, -b * d * e + a * f,	-b * d * f + a * e,		-b * c,		0.0);
	SetRow( 2, a * d * e + b * f,	-a * d * f + b * e,		a * c,		0.0);
	SetRow( 0, 0.0,					0.0,					0.0,		1.0);

	_recheck();
}

void TransformationMatrix :: SetRotation( const Angle & p_angle, const Point3r & p_axis)
{
	real x = p_axis[0];
	real y = p_axis[1];
	real z = p_axis[2];

	real l_rLength = sqrt( x*x + y*y + z*z);

	if ( ! value::is_null( l_rLength))
	{
		x /= l_rLength;
		y /= l_rLength;
		z /= l_rLength;

		real c = cos( p_angle.Radians());
		real s = sin( p_angle.Radians());
		real t = 1 - c;

		real l_xSquared = x * x;
		real l_ySquared = y * y;
		real l_zSquared = z * z;

		real A = (x * y) * t;
		real B = (x * z) * t;
		real C = (y * z) * t;

		real xs = (x * s);
		real ys = (y * s); 
		real zs = (z * s);

		SetRow( 0, l_xSquared * t + c,	A - zs,				B + ys,				0.0);
		SetRow( 1, A + zs,				l_ySquared * t + c,	C - xs,				0.0);
		SetRow( 2, B - ys,				C + xs,				l_zSquared * t + c,	0.0);
		SetRow( 3, 0.0,					0.0,				0.0,				1.0);

		_recheck();
	}
}

void TransformationMatrix :: SetRotation( const Quaternion & p_quat)
{
	p_quat.ToRotationMatrix( * this);
}

void TransformationMatrix :: Rotate( const Angle & p_angle, const Point3r & p_axis)
{
	TransformationMatrix l_mRotation( p_angle, p_axis);

	Matrix4x4r::operator =( l_mRotation * (* this));
}

void TransformationMatrix :: Rotate( const Quaternion & p_quat)
{
	TransformationMatrix l_mRotation( p_quat);

	Matrix4x4r::operator =( l_mRotation * (* this));
}

void TransformationMatrix :: Yaw( const Angle & p_angle)
{
	Matrix4x4r l_mRotation;
	double c,s;

	c = cos( -p_angle.Radians());
	s = sin( -p_angle.Radians());

	l_mRotation.SetRow( 0,   c, 0.0,   s, 0.0);
	l_mRotation.SetRow( 1, 0.0, 1.0, 0.0, 0.0);
	l_mRotation.SetRow( 2,  -s, 0.0,   c, 0.0);
	l_mRotation.SetRow( 3, 0.0, 0.0, 0.0, 1.0);
	l_mRotation *= (Matrix4x4r)( * this);

	Matrix4x4r::operator =( l_mRotation);
}

void TransformationMatrix :: Pitch( const Angle & p_angle)
{
	Matrix4x4r l_mRotation;
	double c,s;

	c = cos( -p_angle.Radians());
	s = sin( -p_angle.Radians());

	l_mRotation.SetRow( 0,  1.0, 0.0, 0.0, 0.0);
	l_mRotation.SetRow( 1,  0.0,   c,  -s, 0.0);
	l_mRotation.SetRow( 2,  0.0,   s,   c, 0.0);
	l_mRotation.SetRow( 3,  0.0, 0.0, 0.0, 1.0);
	l_mRotation *= (Matrix4x4r)( * this);

	Matrix4x4r::operator =( l_mRotation);
}

void TransformationMatrix :: Roll( const Angle & p_angle)
{
	Matrix4x4r l_mRotation;
	double c,s;

	c = cos( -p_angle.Radians());
	s = sin( -p_angle.Radians());

	l_mRotation.SetRow( 0,   c,  -s, 0.0, 0.0);
	l_mRotation.SetRow( 1,   s,   c, 0.0, 0.0);
	l_mRotation.SetRow( 2, 0.0, 0.0, 1.0, 0.0);
	l_mRotation.SetRow( 3, 0.0, 0.0, 0.0, 1.0);
	l_mRotation *= (Matrix4x4r)( * this);

	Matrix4x4r::operator =( l_mRotation);
}

void TransformationMatrix :: Scale( real p_hx, real p_hy, real p_hz)
{
	TransformationMatrix l_mScale;
	l_mScale.SetScale( p_hx, p_hy, p_hz);

	Matrix4x4r::operator =( l_mScale * (* this));
}

void TransformationMatrix :: Scale( const Point3r & p_scale)
{
	TransformationMatrix l_mScale;
	l_mScale.SetScale( p_scale);

	Matrix4x4r::operator =( l_mScale * (* this));
}

void TransformationMatrix :: Translate( real p_tx, real p_ty, real p_tz)
{
	TransformationMatrix l_mTranslation;
	l_mTranslation.SetTranslation( p_tx, p_ty, p_tz);

	Matrix4x4r::operator =( l_mTranslation * (* this));
}

void TransformationMatrix :: Translate( const Point3r & p_translation)
{
	TransformationMatrix l_mTranslation;
	l_mTranslation.SetTranslation( p_translation);

	Matrix4x4r::operator =( l_mTranslation * (* this));
}

TransformationMatrix TransformationMatrix :: operator *( const TransformationMatrix & p_matrix)
{
	TransformationMatrix l_mReturn;
	real l_tSomme;

	for (size_t i = 0 ; i < 4 ; i++)
	{
		for (size_t j = 0 ; j < 4 ; j++)
		{
			l_tSomme = 0;

			for (size_t k = 0 ; k < 4 ; k++)
			{
				l_tSomme += m_matrix[i * 4 + k] * p_matrix[k * 4 + j];
			}

			if (value::is_null( l_tSomme))
			{
				l_mReturn[i * 4 + j] = 0;
			}
			else
			{
				l_mReturn[i * 4 + j] = l_tSomme;
			}
		}
	}

	return l_mReturn;
}

void TransformationMatrix :: operator *=( const TransformationMatrix & p_matrix)
{
	TransformationMatrix l_mTmp( * this);
	real l_tSomme;

	for (size_t i = 0 ; i < 4 ; i++)
	{
		for (size_t j = 0 ; j < 4 ; j++)
		{
			l_tSomme = 0;

			for (size_t k = 0 ; k < 4 ; k++)
			{
				l_tSomme += l_mTmp[i * 4 + k] * p_matrix[k * 4 + j];
			}

			if (value::is_null( l_tSomme))
			{
				m_matrix[i * 4 + j] = 0;
			}
			else
			{
				m_matrix[i * 4 + j] = l_tSomme;
			}
		}
	}
}

Point3r TransformationMatrix :: operator *( Point3rPtr p_vertex)
{
	real x = (real)(m_matrix[0 * 4 + 0] * p_vertex->m_coords[0] + m_matrix[0 * 4 + 1] * p_vertex->m_coords[1] + m_matrix[0 * 4 + 2] * p_vertex->m_coords[2] + m_matrix[0 * 4 + 3] * 1);
	real y = (real)(m_matrix[1 * 4 + 0] * p_vertex->m_coords[0] + m_matrix[1 * 4 + 1] * p_vertex->m_coords[1] + m_matrix[1 * 4 + 2] * p_vertex->m_coords[2] + m_matrix[1 * 4 + 3] * 1);
	real z = (real)(m_matrix[2 * 4 + 0] * p_vertex->m_coords[0] + m_matrix[2 * 4 + 1] * p_vertex->m_coords[1] + m_matrix[2 * 4 + 2] * p_vertex->m_coords[2] + m_matrix[2 * 4 + 3] * 1);

	Point3r l_vertex( x, y, z);
	return l_vertex;
}

Point3r TransformationMatrix :: operator *( const Point3r & p_vertex)
{
	real x = (real)(m_matrix[0 * 4 + 0] * p_vertex[0] + m_matrix[0 * 4 + 1] * p_vertex[1] + m_matrix[0 * 4 + 2] * p_vertex[2] + m_matrix[0 * 4 + 3] * 1);
	real y = (real)(m_matrix[1 * 4 + 0] * p_vertex[0] + m_matrix[1 * 4 + 1] * p_vertex[1] + m_matrix[1 * 4 + 2] * p_vertex[2] + m_matrix[1 * 4 + 3] * 1);
	real z = (real)(m_matrix[2 * 4 + 0] * p_vertex[0] + m_matrix[2 * 4 + 1] * p_vertex[1] + m_matrix[2 * 4 + 2] * p_vertex[2] + m_matrix[2 * 4 + 3] * 1);

	Point3r l_vertex( x, y, z);
	return l_vertex;
}