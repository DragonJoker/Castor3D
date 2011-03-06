#include "Castor3D/PrecompiledHeader.h"

#include "Castor3D/light/SpotLight.h"
#include "Castor3D/render_system/RenderSystem.h"
#include "Castor3D/scene/SceneFileParser.h"
#include "Castor3D/main/Root.h"

using namespace Castor3D;

SpotLight :: SpotLight( Scene * p_pScene, SceneNodePtr p_pNode, const String & p_name)
	:	Light( p_pScene, p_pNode, p_name, eSpot)
	,	m_attenuation( 1, 0, 0)
	,	m_exponent( 0)
	,	m_cutOff( 180)
{
}

SpotLight :: ~SpotLight()
{
}

void SpotLight :: Render( ePRIMITIVE_TYPE p_displayMode)
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
	bool l_bReturn = Light::Write( p_pFile);

	if (l_bReturn)
	{
		l_bReturn = p_pFile.Print( 256, "\tattenuation %f %f %f\n", m_attenuation[0], m_attenuation[1], m_attenuation[2]) > 0;
	}

	if (l_bReturn)
	{
		l_bReturn = p_pFile.Print( 256, "\texponent %f\n", m_exponent) > 0;
	}

	if (l_bReturn)
	{
		l_bReturn = p_pFile.Print( 256, "\tcut_off %f\n", m_cutOff) > 0;
	}

	if (l_bReturn)
	{
		l_bReturn = p_pFile.WriteLine( "}\n") > 0;
	}

	return l_bReturn;
}

bool SpotLight :: Save( File & p_file)const
{
	bool l_bReturn = Light::Save( p_file);

	if (l_bReturn)
	{
		l_bReturn = m_attenuation.Save( p_file);
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.Write( m_exponent) == sizeof( float);
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.Write( m_cutOff) == sizeof( float);
	}

	return l_bReturn;
}

bool SpotLight :: Load( File & p_file)
{
	bool l_bReturn = Light::Load( p_file);

	if (l_bReturn)
	{
		l_bReturn = m_attenuation.Load( p_file);
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.Read( m_exponent) == sizeof( float);
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.Read( m_cutOff) == sizeof( float);
	}

	return l_bReturn;
}