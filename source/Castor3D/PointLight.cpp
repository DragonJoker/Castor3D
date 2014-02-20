#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/PointLight.hpp"
#include "Castor3D/Vertex.hpp"

using namespace Castor3D;
using namespace Castor;

//*************************************************************************************************

bool PointLight::TextLoader :: operator ()( PointLight const & p_light, TextFile & p_file)
{
	bool l_bReturn = LightCategory::TextLoader()( p_light, p_file);

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
		l_bReturn = p_file.WriteText( cuT( "}\n")) > 0;
	}

	return l_bReturn;
}

//*************************************************************************************************

PointLight :: PointLight()
	:	LightCategory	( eLIGHT_TYPE_POINT	)
	,	m_attenuation	( 1.0f, 0.0f, 0.0f	)
{
}

PointLight :: ~PointLight()
{
}

LightCategorySPtr PointLight :: Create()
{
	return std::make_shared< PointLight >();
}

void PointLight :: Render( LightRendererSPtr p_pRenderer )
{
	if( p_pRenderer )
	{
		p_pRenderer->Enable();
		p_pRenderer->ApplyPosition();
		p_pRenderer->ApplyAmbient();
		p_pRenderer->ApplyDiffuse();
		p_pRenderer->ApplySpecular();
		p_pRenderer->ApplyConstantAtt(	m_attenuation[0]);
		p_pRenderer->ApplyLinearAtt(	m_attenuation[1]);
		p_pRenderer->ApplyQuadraticAtt(	m_attenuation[2]);
		p_pRenderer->Bind();
	}
}

void PointLight :: Render( LightRendererSPtr p_pRenderer, ShaderProgramBase * p_pProgram )
{
	if( p_pRenderer )
	{
		p_pRenderer->EnableShader( p_pProgram );
		p_pRenderer->ApplyPositionShader();
		p_pRenderer->ApplyAmbientShader();
		p_pRenderer->ApplyDiffuseShader();
		p_pRenderer->ApplySpecularShader();
		p_pRenderer->ApplyConstantAttShader(	m_attenuation[0]);
		p_pRenderer->ApplyLinearAttShader(		m_attenuation[1]);
		p_pRenderer->ApplyQuadraticAttShader(	m_attenuation[2]);
	}
}

void PointLight :: SetAttenuation( float p_const, float p_linear, float p_quadratic )
{
	m_attenuation[0] = p_const;
	m_attenuation[1] = p_linear;
	m_attenuation[2] = p_quadratic;
}

void PointLight :: SetAttenuation( Point3f const & p_attenuation)
{
	SetAttenuation( p_attenuation[0], p_attenuation[1], p_attenuation[2] );
}

void PointLight :: SetAttenuation( float const * p_attenuation)
{
	SetAttenuation( p_attenuation[0], p_attenuation[1], p_attenuation[2] );
}

//*************************************************************************************************
