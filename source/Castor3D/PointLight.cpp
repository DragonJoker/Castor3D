#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/PointLight.hpp"
#include "Castor3D/SceneNode.hpp"
#include "Castor3D/SceneFileParser.hpp"
#include "Castor3D/RenderSystem.hpp"
#include "Castor3D/Root.hpp"

using namespace Castor3D;

//*************************************************************************************************

bool Loader<PointLight> :: Write( const PointLight & p_light, File & p_file)
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
		l_bReturn = p_file.WriteLine( cuT( "}\n")) > 0;
	}

	return l_bReturn;
}

//*************************************************************************************************

PointLight :: PointLight()
	:	LightCategory( eLIGHT_TYPE_POINT)
	,	m_attenuation( 1.0f, 0.2f, 0.0f)
{
}

PointLight :: ~PointLight()
{
}

LightCategoryPtr PointLight :: Clone()
{
	return LightCategoryPtr( new PointLight);
}

void PointLight :: Render( LightRendererPtr p_pRenderer, ePRIMITIVE_TYPE p_displayMode)
{
	p_pRenderer->Enable();
	p_pRenderer->ApplyPosition();
	p_pRenderer->ApplyAmbient();
	p_pRenderer->ApplyDiffuse();
	p_pRenderer->ApplySpecular();
	p_pRenderer->ApplyConstantAtt(	m_attenuation[0]);
	p_pRenderer->ApplyLinearAtt(	m_attenuation[1]);
	p_pRenderer->ApplyQuadraticAtt(	m_attenuation[2]);
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
	m_attenuation.copy( p_attenuation);
}

BEGIN_SERIALISE_MAP( PointLight, LightCategory)
	ADD_ELEMENT( m_attenuation)
END_SERIALISE_MAP()

//*************************************************************************************************