#include "PrecompiledHeader.h"

#include "light/Module_Light.h"

#include "light/SpotLight.h"
#include "render_system/RenderSystem.h"
#include "main/Root.h"

using namespace Castor3D;

SpotLight :: SpotLight( const String & p_name)
	:	Light( p_name),
		m_matrix( new real[16]),
		m_attenuation( 1, 0, 0)
{
	m_exponent = 0.0;
	m_cutOff = 180.0;
}

SpotLight :: ~SpotLight()
{
	delete [] m_matrix;
}

void SpotLight :: Apply( eDRAW_TYPE p_displayMode)
{
	Light::Apply( p_displayMode);
	m_orientation.ToRotationMatrix( m_matrix);
	m_pRenderer->ApplyOrientation(	m_matrix);
}

void SpotLight :: _initialise()
{
	Light::_initialise();
	m_pRenderer->ApplyConstantAtt(		m_attenuation[0]);
	m_pRenderer->ApplyLinearAtt(		m_attenuation[1]);
	m_pRenderer->ApplyQuadraticAtt(		m_attenuation[2]);
	m_pRenderer->ApplyExponent(			m_exponent);
	m_pRenderer->ApplyCutOff(			m_cutOff);
}

void SpotLight :: Yaw( real p_angle)
{
	Quaternion l_tmp( Point3r( 0.0, 1.0, 0.0), p_angle);
	m_orientation *= l_tmp;
}

void SpotLight :: Pitch( real p_angle)
{
	Quaternion l_tmp( Point3r( 1.0, 0.0, 0.0), p_angle);
	m_orientation *= l_tmp;
}

void SpotLight :: Roll( real p_angle)
{
	Quaternion l_tmp( Point3r( 0.0, 0.0, 1.0), p_angle);
	m_orientation *= l_tmp;
}

void SpotLight :: Rotate( const Quaternion & p_quat)
{
	m_orientation *= p_quat;
}

void SpotLight :: SetAttenuation( float * p_attenuation)
{
	m_attenuation[0] = p_attenuation[0];
	m_attenuation[1] = p_attenuation[1];
	m_attenuation[2] = p_attenuation[2];
	m_pRenderer->ApplyConstantAtt(		m_attenuation[0]);
	m_pRenderer->ApplyLinearAtt(		m_attenuation[1]);
	m_pRenderer->ApplyQuadraticAtt(		m_attenuation[2]);
}

void SpotLight :: SetAttenuation( float p_const, float p_linear,
								  float p_quadratic)
{
	m_attenuation[0] = p_const;
	m_attenuation[1] = p_linear;
	m_attenuation[2] = p_quadratic;
	m_pRenderer->ApplyConstantAtt(		m_attenuation[0]);
	m_pRenderer->ApplyLinearAtt(		m_attenuation[1]);
	m_pRenderer->ApplyQuadraticAtt(		m_attenuation[2]);
}

void SpotLight :: SetAttenuation( const Point<float, 3> & p_attenuation)
{
	m_attenuation[0] = p_attenuation[0];
	m_attenuation[1] = p_attenuation[1];
	m_attenuation[2] = p_attenuation[2];
	m_pRenderer->ApplyConstantAtt(		m_attenuation[0]);
	m_pRenderer->ApplyLinearAtt(		m_attenuation[1]);
	m_pRenderer->ApplyQuadraticAtt(		m_attenuation[2]);
}

void SpotLight :: SetExponent( float p_exponent)
{
	m_exponent = p_exponent;
	m_pRenderer->ApplyExponent( m_exponent);
}

void SpotLight :: SetCutOff( float p_cutOff)
{
	m_cutOff = p_cutOff;
	m_pRenderer->ApplyCutOff( m_cutOff); 
}

real * SpotLight :: Get4x4RotationMatrix()
{
	m_orientation.ToRotationMatrix( m_matrix);
	return m_matrix;
}

bool SpotLight :: Write( Castor::Utils::File & p_pFile)const
{
	bool l_bReturn = p_pFile.WriteLine( "light " + m_name + "\n{\n");

	if (l_bReturn)
	{
		l_bReturn = p_pFile.WriteLine( "\ttype spot_light\n");
	}

	if (l_bReturn)
	{
		l_bReturn = Light::Write( p_pFile);
	}

	if (l_bReturn)
	{
		l_bReturn = p_pFile.Print( 256, "\tattenuation %f %f %f %f\n", m_attenuation[0], m_attenuation[1], m_attenuation[2]);
	}

	if (l_bReturn)
	{
		l_bReturn = p_pFile.Print( 256, "\texponent %f\n", m_exponent);
	}

	if (l_bReturn)
	{
		l_bReturn = p_pFile.Print( 256, "\tcut_off %f\n", m_cutOff);
	}

	if (l_bReturn)
	{
		l_bReturn = p_pFile.Print( 256, "\torientation %f %f %f %f\n", m_orientation[0], m_orientation[1], m_orientation[2], m_orientation[3]);
	}

	if (l_bReturn)
	{
		l_bReturn = p_pFile.WriteLine( "}\n");
	}

	return l_bReturn;
}