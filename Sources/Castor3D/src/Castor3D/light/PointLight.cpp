#include "PrecompiledHeader.h"

#include "light/Module_Light.h"

#include "light/PointLight.h"
#include "scene/SceneNode.h"
#include "render_system/RenderSystem.h"
#include "main/Root.h"

using namespace Castor3D;

PointLight :: PointLight( const String & p_name)
	:	Light( p_name),
		m_attenuation( 1.0f, 0.2f, 0.0f)
{
}

PointLight :: ~PointLight()
{
}

void PointLight :: _initialise()
{
	Light::_initialise();
	m_pRenderer->ApplyConstantAtt(	m_attenuation[0]);
	m_pRenderer->ApplyLinearAtt(	m_attenuation[1]);
	m_pRenderer->ApplyQuadraticAtt(	m_attenuation[2]);
}

void PointLight :: SetAttenuation( float * p_attenuation)
{
	m_attenuation[0] = p_attenuation[0];
	m_attenuation[1] = p_attenuation[1];
	m_attenuation[2] = p_attenuation[2];
	m_pRenderer->ApplyConstantAtt(	m_attenuation[0]);
	m_pRenderer->ApplyLinearAtt(	m_attenuation[1]);
	m_pRenderer->ApplyQuadraticAtt(	m_attenuation[2]);
}

void PointLight :: SetAttenuation( float p_const, float p_linear,
								   float p_quadratic)
{
	m_attenuation[0] = p_const;
	m_attenuation[1] = p_linear;
	m_attenuation[2] = p_quadratic;
	m_pRenderer->ApplyConstantAtt(	m_attenuation[0]);
	m_pRenderer->ApplyLinearAtt(	m_attenuation[1]);
	m_pRenderer->ApplyQuadraticAtt(	m_attenuation[2]);
}

void PointLight :: SetAttenuation( const Point<float, 3> & p_attenuation)
{
	m_attenuation = p_attenuation;
	m_pRenderer->ApplyConstantAtt(	m_attenuation[0]);
	m_pRenderer->ApplyLinearAtt(	m_attenuation[1]);
	m_pRenderer->ApplyQuadraticAtt(	m_attenuation[2]);
}

bool PointLight :: Write( Castor::Utils::File & p_pFile)const
{
	bool l_bReturn = p_pFile.WriteLine( "light " + m_name + "\n{\n");

	if (l_bReturn)
	{
		l_bReturn = p_pFile.WriteLine( "\ttype point_light\n");
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
		l_bReturn = p_pFile.WriteLine( "}\n");
	}

	return l_bReturn;
}