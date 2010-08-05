#include "RotationMatrix.h"
#include "Vector3f.h"

using namespace General::Math;

RotationMatrix :: RotationMatrix()
{
}

RotationMatrix :: ~RotationMatrix()
{
}

void RotationMatrix :: Initialise()
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			m_matrix[i][j] = 0.0f;
		}
	}
}

void RotationMatrix :: LoadIdentity()
{
	Initialise();

	for (int i = 0 ; i < 4 ; i++)
	{
		m_matrix[i][i] = 1;
	}
}

void RotationMatrix :: LoadRotationX( float angle_p)
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

void RotationMatrix :: LoadRotationY( float angle_p)
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

void RotationMatrix :: LoadRotationZ( float angle_p)
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

void RotationMatrix :: LoadScale( float hx_p, float hy_p, float hz_p)
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

void RotationMatrix :: LoadTranslation( float tx_p, float ty_p, float tz_p)
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

void RotationMatrix :: LoadRotationEuler( float rx_p, float ry_p, float rz_p)
{
	float a = cos( rx_p);
	float b = sin( rx_p);
	float c = cos( ry_p);
	float d = sin( ry_p);
	float e = cos( rz_p);
	float f = sin( rz_p);

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

void RotationMatrix :: LoadRotation( float p_angle, const Vector3f & p_axis)
{
	float c = cos( p_angle);
	float s = sin( p_angle);

	float l_xSquared = (p_axis.x * p_axis.x);
	float l_ySquared = (p_axis.y * p_axis.y);
	float l_zSquared = (p_axis.z * p_axis.z);

	float l_oneMinusC = (1-c);

	float A = (p_axis.x * p_axis.y) * l_oneMinusC;
	float B = (p_axis.x * p_axis.z) * l_oneMinusC;
	float C = (p_axis.y * p_axis.z) * l_oneMinusC;

	float xs = (p_axis.x * s);
	float ys = (p_axis.y * s); 
	float zs = (p_axis.z * s);


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

void RotationMatrix :: Multiply( Vector3f * p_vertex)
{
	float x = (float)(m_matrix[0][0] * p_vertex->x + m_matrix[0][1] * p_vertex->y + m_matrix[0][2] * p_vertex->z + m_matrix[0][3] * 1);
	float y = (float)(m_matrix[1][0] * p_vertex->x + m_matrix[1][1] * p_vertex->y + m_matrix[1][2] * p_vertex->z + m_matrix[1][3] * 1);
	float z = (float)(m_matrix[2][0] * p_vertex->x + m_matrix[2][1] * p_vertex->y + m_matrix[2][2] * p_vertex->z + m_matrix[2][3] * 1);

	p_vertex->x = x;
	p_vertex->y = y;
	p_vertex->z = z;
}

RotationMatrix * RotationMatrix :: GetTranspose()
{
	RotationMatrix * l_result = new RotationMatrix();
 
	for (int i = 0 ; i < 4 ; i++)
	{
		for (int j = 0 ; j < 4 ; j++)
		{
			l_result->m_matrix[j][i] = m_matrix[i][j];
		}
	}

	return l_result;
}

float RotationMatrix::GetTrace()
{
	return m_matrix[0][0] + m_matrix[1][1] + m_matrix[2][2];
}