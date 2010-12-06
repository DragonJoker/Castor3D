#include "PrecompiledHeader.h"

#include "light/Module_Light.h"

#include "light/SpotLight.h"
#include "render_system/RenderSystem.h"
#include "main/Root.h"

using namespace Castor3D;

SpotLight :: SpotLight( LightNodePtr p_pNode, const String & p_name)
	:	Light( p_pNode, p_name),
		m_attenuation( 1, 0, 0),
		m_exponent( 0),
		m_cutOff( 180)
{
}

SpotLight :: ~SpotLight()
{
}

void SpotLight :: Render( eDRAW_TYPE p_displayMode)
{
	m_pRenderer->Enable();
	m_pRenderer->ApplyPosition();
	m_pRenderer->ApplyOrientation();
	_initialise();
}

void SpotLight :: _initialise()
{
	Light::_initialise();
	m_pRenderer->ApplyConstantAtt(	m_attenuation[0]);
	m_pRenderer->ApplyLinearAtt(	m_attenuation[1]);
	m_pRenderer->ApplyQuadraticAtt(	m_attenuation[2]);
	m_pRenderer->ApplyExponent(		m_exponent);
	m_pRenderer->ApplyCutOff(		m_cutOff);
}

void SpotLight :: SetAttenuation( float * p_attenuation)
{
	m_attenuation[0] = p_attenuation[0];
	m_attenuation[1] = p_attenuation[1];
	m_attenuation[2] = p_attenuation[2];
}

void SpotLight :: SetAttenuation( float p_const, float p_linear,
								  float p_quadratic)
{
	m_attenuation[0] = p_const;
	m_attenuation[1] = p_linear;
	m_attenuation[2] = p_quadratic;
}

void SpotLight :: SetAttenuation( const Point3f & p_attenuation)
{
	m_attenuation[0] = p_attenuation[0];
	m_attenuation[1] = p_attenuation[1];
	m_attenuation[2] = p_attenuation[2];
}

void SpotLight :: SetExponent( float p_exponent)
{
	m_exponent = p_exponent;
}

void SpotLight :: SetCutOff( float p_cutOff)
{
	m_cutOff = p_cutOff;
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
		l_bReturn = p_pFile.WriteLine( "}\n");
	}

	return l_bReturn;
}