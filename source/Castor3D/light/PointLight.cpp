#include "Castor3D/PrecompiledHeader.h"

#include "Castor3D/light/PointLight.h"
#include "Castor3D/scene/SceneNode.h"
#include "Castor3D/render_system/RenderSystem.h"
#include "Castor3D/main/Root.h"

using namespace Castor3D;

PointLight :: PointLight( SceneNodePtr p_pNode, const String & p_name)
	:	Light( p_pNode, p_name)
	,	m_attenuation( 1.0f, 0.2f, 0.0f)
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
}

void PointLight :: SetAttenuation( float p_const, float p_linear,
								   float p_quadratic)
{
	m_attenuation[0] = p_const;
	m_attenuation[1] = p_linear;
	m_attenuation[2] = p_quadratic;
}

void PointLight :: SetAttenuation( const Point3f & p_attenuation)
{
	m_attenuation = p_attenuation;
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