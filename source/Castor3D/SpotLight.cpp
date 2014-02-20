#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/SpotLight.hpp"
#include "Castor3D/Vertex.hpp"

using namespace Castor3D;
using namespace Castor;

//*************************************************************************************************

bool SpotLight::TextLoader :: operator ()( SpotLight const & p_light, TextFile & p_file )
{
	bool l_bReturn = LightCategory::TextLoader()( p_light, p_file );

	if (l_bReturn)
	{
		l_bReturn = p_file.Print( 256, cuT( "\tattenuation ")) > 0;
	}

	if (l_bReturn)
	{
		l_bReturn = Point3f::TextLoader()( p_light.GetAttenuation(), p_file);
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
		l_bReturn = p_file.WriteText( cuT( "}\n")) > 0;
	}

	return l_bReturn;
}

//*************************************************************************************************

SpotLight :: SpotLight()
	:	LightCategory	( eLIGHT_TYPE_SPOT	)
	,	m_attenuation	( 1, 0, 0			)
	,	m_exponent		( 0					)
	,	m_cutOff		( 180				)
{
}

SpotLight :: ~SpotLight()
{
}

LightCategorySPtr SpotLight :: Create()
{
	return std::make_shared< SpotLight >();
}

void SpotLight :: Render( LightRendererSPtr p_pRenderer )
{
	if( p_pRenderer )
	{
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
		p_pRenderer->Bind();
	}
}

void SpotLight :: Render( LightRendererSPtr p_pRenderer, ShaderProgramBase * p_pProgram )
{
	if( p_pRenderer )
	{
		p_pRenderer->EnableShader( p_pProgram );
		p_pRenderer->ApplyPositionShader();
		p_pRenderer->ApplyOrientationShader();
		p_pRenderer->ApplyAmbientShader();
		p_pRenderer->ApplyDiffuseShader();
		p_pRenderer->ApplySpecularShader();
		p_pRenderer->ApplyConstantAttShader(	m_attenuation[0]);
		p_pRenderer->ApplyLinearAttShader(		m_attenuation[1]);
		p_pRenderer->ApplyQuadraticAttShader(	m_attenuation[2]);
		p_pRenderer->ApplyExponentShader(		m_exponent);
		p_pRenderer->ApplyCutOffShader(			m_cutOff);
	}
}

void SpotLight :: SetAttenuation( float p_const, float p_linear, float p_quadratic )
{
	m_attenuation[0] = p_const;
	m_attenuation[1] = p_linear;
	m_attenuation[2] = p_quadratic;
}

void SpotLight :: SetAttenuation( Point3f const & p_attenuation )
{
	SetAttenuation( p_attenuation[0], p_attenuation[1], p_attenuation[2] );
}

void SpotLight :: SetAttenuation( float const * p_attenuation )
{
	SetAttenuation( p_attenuation[0], p_attenuation[1], p_attenuation[2] );
}

void SpotLight :: SetExponent( float p_exponent)
{
	m_exponent = p_exponent;
}

void SpotLight :: SetCutOff( float p_cutOff)
{
	m_cutOff = p_cutOff;
}

//*************************************************************************************************
