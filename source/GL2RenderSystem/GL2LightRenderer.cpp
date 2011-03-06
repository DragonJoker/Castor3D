#include "Gl2RenderSystem/PrecompiledHeader.h"

#include "Gl2RenderSystem/Gl2LightRenderer.h"
#include "Gl2RenderSystem/Gl2RenderSystem.h"
#include "Gl2RenderSystem/Gl2ShaderProgram.h"

using namespace Castor3D;

Gl2LightRenderer :: Gl2LightRenderer( SceneManager * p_pSceneManager)
	:	GlLightRenderer( p_pSceneManager)
	,	m_bChanged( true)
	,	m_iGlIndex( GL_INVALID_INDEX)
	,	m_iGlPreviousIndex( GL_INVALID_INDEX)
{
}

void Gl2LightRenderer :: Initialise()
{
	m_bChanged = true;
	m_iGlPreviousIndex = GL_INVALID_INDEX;

	if (RenderSystem::UseShaders() && Gl2RenderSystem::GetSingletonPtr()->GetCurrentShaderProgram() != NULL)
	{
		m_pfnEnableFunction			= PStateFunction(					& Gl2LightRenderer::_enable);
		m_pfnDisableFunction		= PStateFunction(					& Gl2LightRenderer::_disable);
		m_pfnAmbientFunction		= PColourFunction(					& Gl2LightRenderer::_applyAmbient);
		m_pfnDiffuseFunction		= PColourFunction(					& Gl2LightRenderer::_applyDiffuse);
		m_pfnSpecularFunction		= PColourFunction(					& Gl2LightRenderer::_applySpecular);
		m_pfnOrientationFunction	= POrientationFunction(				& Gl2LightRenderer::_applyOrientation);
		m_pfnPositionFunction		= PPositionFunction(				& Gl2LightRenderer::_applyPosition);
		m_pfnConstantAttFunction	= PAttenuationComponentFunction(	& Gl2LightRenderer::_applyConstantAtt);
		m_pfnLinearAttFunction		= PAttenuationComponentFunction(	& Gl2LightRenderer::_applyLinearAtt);
		m_pfnQuadraticAttFunction	= PAttenuationComponentFunction(	& Gl2LightRenderer::_applyQuadraticAtt);
		m_pfnExponentFunction		= PSpotComponentFunction(			& Gl2LightRenderer::_applyExponent);
		m_pfnCutOffFunction			= PSpotComponentFunction(			& Gl2LightRenderer::_applyCutOff);
	}
	else
	{
		GlLightRenderer::Initialise();
	}
}

void Gl2LightRenderer :: _enable()
{
	Gl2ShaderProgram * l_pProgram = Gl2RenderSystem::GetSingletonPtr()->GetCurrentShaderProgram();
	m_iGlIndex = l_pProgram->AssignLight();

	if (m_iGlIndex != GL_INVALID_INDEX && (m_bChanged || m_iGlIndex != m_iGlPreviousIndex))
	{
		l_pProgram->SetLightAmbient( m_iGlIndex, m_pAmbient);
		l_pProgram->SetLightDiffuse( m_iGlIndex, m_pDiffuse);
		l_pProgram->SetLightSpecular( m_iGlIndex, m_pSpecular);
		l_pProgram->SetLightPosition( m_iGlIndex, m_pPosition);
		l_pProgram->SetLightOrientation( m_iGlIndex, m_pOrientation);
		l_pProgram->SetLightAttenuation( m_iGlIndex, m_pAttenuation);
		l_pProgram->SetLightExponent( m_iGlIndex, m_pExponent);
		l_pProgram->SetLightCutOff( m_iGlIndex, m_pCutOff);
		SetGlPreviousIndex( m_iGlIndex);
	}
}

void Gl2LightRenderer :: _disable()
{
	if (m_iGlIndex != GL_INVALID_INDEX)
	{
		Gl2RenderSystem::GetSingletonPtr()->GetCurrentShaderProgram()->FreeLight( m_iGlIndex);
	}
}

void Gl2LightRenderer :: _applyAmbient( const Colour & p_ambient)
{
	m_bChanged = m_pAmbient != p_ambient;
	m_pAmbient = p_ambient;
}

void Gl2LightRenderer :: _applyDiffuse( const Colour & p_diffuse)
{
	m_bChanged = m_pDiffuse != p_diffuse;
	m_pDiffuse = p_diffuse;
}

void Gl2LightRenderer :: _applySpecular( const Colour & p_specular)
{
	m_bChanged = m_pSpecular != p_specular;
	m_pSpecular = p_specular;
}

void Gl2LightRenderer :: _applyPosition( const Point4f & p_position)
{
	m_bChanged = m_pPosition != p_position;
	m_pPosition = p_position;
}

void Gl2LightRenderer :: _applyOrientation( const Matrix4x4r & p_orientation)
{
	m_bChanged = m_pOrientation != p_orientation;
	m_pOrientation = p_orientation;
}

void Gl2LightRenderer :: _applyConstantAtt( float p_constant)
{
	m_bChanged = m_pAttenuation[0] != p_constant;
	m_pAttenuation[0] = p_constant;
}

void Gl2LightRenderer :: _applyLinearAtt( float p_linear)
{
	m_bChanged = m_pAttenuation[1] != p_linear;
	m_pAttenuation[1] = p_linear;
}

void Gl2LightRenderer :: _applyQuadraticAtt( float p_quadratic)
{
	m_bChanged = m_pAttenuation[2] != p_quadratic;
	m_pAttenuation[2] = p_quadratic;
}

void Gl2LightRenderer :: _applyExponent( float p_exponent)
{
	m_bChanged = m_pExponent != p_exponent;
	m_pExponent = p_exponent;
}

void Gl2LightRenderer :: _applyCutOff( float p_cutOff)
{
	m_bChanged = m_pCutOff != p_cutOff;
	m_pCutOff = p_cutOff;
}
