#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/SpotLight.hpp"
#include "Castor3D/RenderSystem.hpp"
#include "Castor3D/SceneFileParser.hpp"
#include "Castor3D/Root.hpp"

using namespace Castor3D;

//*************************************************************************************************

bool Loader<SpotLight> :: Write( const SpotLight & p_light, File & p_file)
{
	bool l_bReturn = Loader<LightCategory>::Write( p_light, p_file);

	if (l_bReturn)
	{
		l_bReturn = p_file.Print( 256, cuT( "\tattenuation ")) > 0;
	}

	if (l_bReturn)
	{
		l_bReturn = Loader<Point3f>::Write( p_light.GetAttenuation(), p_file);
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.Print( 256, cuT( "\n\texponent %f\n"), p_light.GetExponent()) > 0;
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.Print( 256, cuT( "\tcut_off %f\n"), p_light.GetCutOff()) > 0;
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.WriteLine( cuT( "}\n")) > 0;
	}

	return l_bReturn;
}

//*************************************************************************************************

SpotLight :: SpotLight()
	:	LightCategory( eLIGHT_TYPE_SPOT)
	,	m_attenuation( 1, 0, 0)
	,	m_exponent( 0)
	,	m_cutOff( 180)
{
}

SpotLight :: ~SpotLight()
{
}

LightCategoryPtr SpotLight :: Clone()
{
	return LightCategoryPtr( new SpotLight);
}

void SpotLight :: Render( LightRendererPtr p_pRenderer, ePRIMITIVE_TYPE p_displayMode)
{
	CASTOR_TRACK;
	p_pRenderer->Enable();
	p_pRenderer->ApplyPosition();
	p_pRenderer->ApplyOrientation();
	p_pRenderer->ApplyAmbient();
	p_pRenderer->ApplyDiffuse();
	p_pRenderer->ApplySpecular();
	p_pRenderer->ApplyConstantAtt(	m_attenuation[0]);
	p_pRenderer->ApplyLinearAtt(	m_attenuation[1]);
	p_pRenderer->ApplyQuadraticAtt(	m_attenuation[2]);
	p_pRenderer->ApplyExponent(		m_exponent);
	p_pRenderer->ApplyCutOff(		m_cutOff);
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

BEGIN_SERIALISE_MAP( SpotLight, LightCategory)
	ADD_ELEMENT( m_attenuation)
	ADD_ELEMENT( m_exponent)
	ADD_ELEMENT( m_cutOff)
END_SERIALISE_MAP()

//*************************************************************************************************