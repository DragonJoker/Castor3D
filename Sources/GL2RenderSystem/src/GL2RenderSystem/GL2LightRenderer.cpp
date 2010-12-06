#include "PrecompiledHeader.h"

#include "GL2LightRenderer.h"
#include "GL2RenderSystem.h"
#include "GL2ShaderProgram.h"

using namespace Castor3D;

GL2LightRenderer :: GL2LightRenderer()
	:	GLLightRenderer(),
		m_bChanged( true),
		m_iGLIndex( GL_INVALID_INDEX),
		m_iGLPreviousIndex( GL_INVALID_INDEX)
{
}

void GL2LightRenderer :: Initialise()
{
	m_bChanged = true;
	m_iGLPreviousIndex = GL_INVALID_INDEX;

	if (RenderSystem::UseShaders() && RenderSystem::GetSingletonPtr()->GetCurrentShaderProgram() != NULL)
	{
		m_pfnEnableFunction			= PStateFunction(					& GL2LightRenderer::_enable);
		m_pfnDisableFunction		= PStateFunction(					& GL2LightRenderer::_disable);
		m_pfnAmbientFunction		= PColourFunction(					& GL2LightRenderer::_applyAmbient);
		m_pfnDiffuseFunction		= PColourFunction(					& GL2LightRenderer::_applyDiffuse);
		m_pfnSpecularFunction		= PColourFunction(					& GL2LightRenderer::_applySpecular);
		m_pfnOrientationFunction	= POrientationFunction(				& GL2LightRenderer::_applyOrientation);
		m_pfnPositionFunction		= PPositionFunction(				& GL2LightRenderer::_applyPosition);
		m_pfnConstantAttFunction	= PAttenuationComponentFunction(	& GL2LightRenderer::_applyConstantAtt);
		m_pfnLinearAttFunction		= PAttenuationComponentFunction(	& GL2LightRenderer::_applyLinearAtt);
		m_pfnQuadraticAttFunction	= PAttenuationComponentFunction(	& GL2LightRenderer::_applyQuadraticAtt);
		m_pfnExponentFunction		= PSpotComponentFunction(			& GL2LightRenderer::_applyExponent);
		m_pfnCutOffFunction			= PSpotComponentFunction(			& GL2LightRenderer::_applyCutOff);
	}
	else
	{
		GLLightRenderer::Initialise();
	}
}

void GL2LightRenderer :: _enable()
{
	GL2ShaderProgram * l_pProgram = ((GL2ShaderProgram *)RenderSystem::GetSingletonPtr()->GetCurrentShaderProgram());
	m_iGLIndex = l_pProgram->AssignLight();

	if (m_iGLIndex != GL_INVALID_INDEX && (m_bChanged || m_iGLIndex != m_iGLPreviousIndex))
	{
		l_pProgram->SetLightAmbient( m_iGLIndex, m_pAmbient);
		l_pProgram->SetLightDiffuse( m_iGLIndex, m_pDiffuse);
		l_pProgram->SetLightSpecular( m_iGLIndex, m_pSpecular);
		l_pProgram->SetLightPosition( m_iGLIndex, m_pPosition);
		l_pProgram->SetLightOrientation( m_iGLIndex, m_pOrientation);
		l_pProgram->SetLightAttenuation( m_iGLIndex, m_pAttenuation);
		l_pProgram->SetLightExponent( m_iGLIndex, m_pExponent);
		l_pProgram->SetLightCutOff( m_iGLIndex, m_pCutOff);
		SetGLPreviousIndex( m_iGLIndex);
	}
}

void GL2LightRenderer :: _disable()
{
	if (m_iGLIndex != GL_INVALID_INDEX)
	{
		((GL2ShaderProgram *)RenderSystem::GetSingletonPtr()->GetCurrentShaderProgram())->FreeLight( m_iGLIndex);
	}
}

void GL2LightRenderer :: _applyAmbient( const Colour & p_ambient)
{
	m_bChanged = m_pAmbient != p_ambient;
	m_pAmbient = p_ambient;
}

void GL2LightRenderer :: _applyDiffuse( const Colour & p_diffuse)
{
	m_bChanged = m_pDiffuse != p_diffuse;
	m_pDiffuse = p_diffuse;
}

void GL2LightRenderer :: _applySpecular( const Colour & p_specular)
{
	m_bChanged = m_pSpecular != p_specular;
	m_pSpecular = p_specular;
}

void GL2LightRenderer :: _applyPosition( const Point4f & p_position)
{
	m_bChanged = m_pPosition != p_position;
	m_pPosition = p_position;
}

void GL2LightRenderer :: _applyOrientation( const Matrix4x4r & p_orientation)
{
	m_bChanged = m_pOrientation != p_orientation;
	m_pOrientation = p_orientation;
}

void GL2LightRenderer :: _applyConstantAtt( float p_constant)
{
	m_bChanged = m_pAttenuation[0] != p_constant;
	m_pAttenuation[0] = p_constant;
}

void GL2LightRenderer :: _applyLinearAtt( float p_linear)
{
	m_bChanged = m_pAttenuation[1] != p_linear;
	m_pAttenuation[1] = p_linear;
}

void GL2LightRenderer :: _applyQuadraticAtt( float p_quadratic)
{
	m_bChanged = m_pAttenuation[2] != p_quadratic;
	m_pAttenuation[2] = p_quadratic;
}

void GL2LightRenderer :: _applyExponent( float p_exponent)
{
	m_bChanged = m_pExponent != p_exponent;
	m_pExponent = p_exponent;
}

void GL2LightRenderer :: _applyCutOff( float p_cutOff)
{
	m_bChanged = m_pCutOff != p_cutOff;
	m_pCutOff = p_cutOff;
}
