#include "PrecompiledHeader.h"

#include "RotationMatrix.h"
#include "Quaternion.h"

using namespace Castor::Math;

RotationMatrix :: RotationMatrix()
{
}

RotationMatrix :: ~RotationMatrix()
{
}

void RotationMatrix :: LoadRotationX( real angle_p)
{
	//Ligne 1
	m_matrix[0][0] = 1;
	m_matrix[0][1] = 0;
	m_matrix[0][2] = 0;
	m_matrix[0][3] = 0;

	//Ligne 2
	m_matrix[1][0] = 0;
	m_matrix[1][1] = cos( angle_p);
	m_matrix[1][2] = -sin( angle_p);
	m_matrix[1][3] = 0;

	//Ligne 3
	m_matrix[2][0] = 0;
	m_matrix[2][1] = -m_matrix[1][2];
	m_matrix[2][2] = m_matrix[1][1];
	m_matrix[2][3] = 0;

	//Ligne 4
	m_matrix[3][0] = 0;
	m_matrix[3][1] = 0;
	m_matrix[3][2] = 0;
	m_matrix[3][3] = 1;
}

void RotationMatrix :: LoadRotationY( real angle_p)
{
	//Ligne 1
	m_matrix[0][0] = cos(angle_p);
	m_matrix[0][1] = 0;
	m_matrix[0][2] = -sin(angle_p);
	m_matrix[0][3] = 0;

	//Ligne 2
	m_matrix[1][0] = 0;
	m_matrix[1][1] = 1;
	m_matrix[1][2] = 0;
	m_matrix[1][3] = 0;

	//Ligne 3
	m_matrix[2][0] = -m_matrix[0][2];
	m_matrix[2][1] = 0;
	m_matrix[2][2] = m_matrix[0][0];
	m_matrix[2][3] = 0;

	//Ligne 4
	m_matrix[3][0] = 0;
	m_matrix[3][1] = 0;
	m_matrix[3][2] = 0;
	m_matrix[3][3] = 1;

}

void RotationMatrix :: LoadRotationZ( real angle_p)
{
	//Ligne 1
	m_matrix[0][0] = cos( angle_p);
	m_matrix[0][1] = -sin( angle_p);
	m_matrix[0][2] = 0;
	m_matrix[0][3] = 0;

	//Ligne 2
	m_matrix[1][0] = -m_matrix[0][1];
	m_matrix[1][1] = m_matrix[0][0];
	m_matrix[1][2] = 0;
	m_matrix[1][3] = 0;

	//Ligne 3
	m_matrix[2][0] = 0;
	m_matrix[2][1] = 0;
	m_matrix[2][2] = 1;
	m_matrix[2][3] = 0;

	//Ligne 4
	m_matrix[3][0] = 0;
	m_matrix[3][1] = 0;
	m_matrix[3][2] = 0;
	m_matrix[3][3] = 1;
}

void RotationMatrix :: LoadScale( real hx_p, real hy_p, real hz_p)
{
	//Ligne 1
	m_matrix[0][0] = hx_p;
	m_matrix[0][1] = 0;
	m_matrix[0][2] = 0;
	m_matrix[0][3] = 0;

	//Ligne 2
	m_matrix[1][0] = 0;
	m_matrix[1][1] = hy_p;
	m_matrix[1][2] = 0;
	m_matrix[1][3] = 0;

	//Ligne 3
	m_matrix[2][0] = 0;
	m_matrix[2][1] = 0;
	m_matrix[2][2] = hz_p;
	m_matrix[2][3] = 0;

	//Ligne 4
	m_matrix[3][0] = 0;
	m_matrix[3][1] = 0;
	m_matrix[3][2] = 0;
	m_matrix[3][3] = 1;
}

void RotationMatrix :: LoadTranslation( real tx_p, real ty_p, real tz_p)
{
	//Ligne 1
	m_matrix[0][0] = 1;
	m_matrix[0][1] = 0;
	m_matrix[0][2] = 0;
	m_matrix[0][3] = tx_p;

	//Ligne 2
	m_matrix[1][0] = 0;
	m_matrix[1][1] = 1;
	m_matrix[1][2] = 0;
	m_matrix[1][3] = ty_p;

	//Ligne 3
	m_matrix[2][0] = 0;
	m_matrix[2][1] = 0;
	m_matrix[2][2] = 1;
	m_matrix[2][3] = tz_p;

	//Ligne 4
	m_matrix[3][0] = 0;
	m_matrix[3][1] = 0;
	m_matrix[3][2] = 0;
	m_matrix[3][3] = 1;
}

void RotationMatrix :: LoadRotationEuler( real rx_p, real ry_p, real rz_p)
{
	real a = cos( rx_p);
	real b = sin( rx_p);
	real c = cos( ry_p);
	real d = sin( ry_p);
	real e = cos( rz_p);
	real f = sin( rz_p);

	//Ligne 1
	m_matrix[0][0] = c * e;
	m_matrix[0][1] = -c * f;
	m_matrix[0][2] = -d;
	m_matrix[0][3] = 0;

	//Ligne 2
	m_matrix[1][0] = -b * d * e + a * f;
	m_matrix[1][1] = -b * d * f + a * e;
	m_matrix[1][2] = -b * c;
	m_matrix[1][3] = 0;

	//Ligne 3
	m_matrix[2][0] = a * d * e + b * f;
	m_matrix[2][1] = -a * d * f + b * e;
	m_matrix[2][2] = a * c;
	m_matrix[2][3] = 0;

	//Ligne 4
	m_matrix[3][0] = 0;
	m_matrix[3][1] = 0;
	m_matrix[3][2] = 0;
	m_matrix[3][3] = 1;
}

void RotationMatrix :: LoadRotation( real p_angle, const Point3r & p_axis)
{
	real c = cos( p_angle);
	real s = sin( p_angle);

	real l_xSquared = (p_axis[0] * p_axis[0]);
	real l_ySquared = (p_axis[1] * p_axis[1]);
	real l_zSquared = (p_axis[2] * p_axis[2]);

	real l_oneMinusC = (1-c);

	real A = (p_axis[0] * p_axis[1]) * l_oneMinusC;
	real B = (p_axis[0] * p_axis[2]) * l_oneMinusC;
	real C = (p_axis[1] * p_axis[2]) * l_oneMinusC;

	real xs = (p_axis[0] * s);
	real ys = (p_axis[1] * s); 
	real zs = (p_axis[2] * s);


	//Ligne 1
	m_matrix[0][0] = l_xSquared + c * (1 - l_xSquared);
	m_matrix[0][1] = A - zs;
	m_matrix[0][2] = B + ys;
	m_matrix[0][3] = 0;

	//Ligne 2
	m_matrix[1][0] = A + zs; 
	m_matrix[1][1] = l_ySquared + c * (1 - l_ySquared);
	m_matrix[1][2] = C - xs;
	m_matrix[1][3] = 0;

	//Ligne 3
	m_matrix[2][0] = B - ys;
	m_matrix[2][1] = C + xs;
	m_matrix[2][2] = l_zSquared + c * (1 - l_zSquared);
	m_matrix[2][3] = 0;

	//Ligne 4
	m_matrix[3][0] = 0;
	m_matrix[3][1] = 0;
	m_matrix[3][2] = 0;
	m_matrix[3][3] = 1;	
}

void RotationMatrix :: LoadRotation( const Quaternion & p_quat)
{
	p_quat.ToRotationMatrix( * this);
}

void RotationMatrix :: Multiply( Point3rPtr p_vertex)
{
	real x = (real)(m_matrix[0][0] * p_vertex->m_coords[0] + m_matrix[0][1] * p_vertex->m_coords[1] + m_matrix[0][2] * p_vertex->m_coords[2] + m_matrix[0][3] * 1);
	real y = (real)(m_matrix[1][0] * p_vertex->m_coords[0] + m_matrix[1][1] * p_vertex->m_coords[1] + m_matrix[1][2] * p_vertex->m_coords[2] + m_matrix[1][3] * 1);
	real z = (real)(m_matrix[2][0] * p_vertex->m_coords[0] + m_matrix[2][1] * p_vertex->m_coords[1] + m_matrix[2][2] * p_vertex->m_coords[2] + m_matrix[2][3] * 1);

	p_vertex->m_coords[0] = x;
	p_vertex->m_coords[1] = y;
	p_vertex->m_coords[2] = z;
}