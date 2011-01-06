#include "GL3RenderSystem/PrecompiledHeader.h"

#include "GL3RenderSystem/GL3LightRenderer.h"
#include "GL3RenderSystem/GL3RenderSystem.h"
#include "GL3RenderSystem/GL3ShaderProgram.h"

using namespace Castor3D;

GL3LightRenderer :: GL3LightRenderer( SceneManager * p_pSceneManager)
	:	GLLightRenderer( p_pSceneManager),
		m_bChanged( true),
		m_iGLIndex( GL_INVALID_INDEX),
		m_iGLPreviousIndex( GL_INVALID_INDEX)
{
	m_pfnEnableFunction			= PStateFunction(					& GL3LightRenderer::_enable);
	m_pfnDisableFunction		= PStateFunction(					& GL3LightRenderer::_disable);
	m_pfnAmbientFunction		= PColourFunction(					& GL3LightRenderer::_applyAmbient);
	m_pfnDiffuseFunction		= PColourFunction(					& GL3LightRenderer::_applyDiffuse);
	m_pfnSpecularFunction		= PColourFunction(					& GL3LightRenderer::_applySpecular);
	m_pfnOrientationFunction	= POrientationFunction(				& GL3LightRenderer::_applyOrientation);
	m_pfnPositionFunction		= PPositionFunction(				& GL3LightRenderer::_applyPosition);
	m_pfnConstantAttFunction	= PAttenuationComponentFunction(	& GL3LightRenderer::_applyConstantAtt);
	m_pfnLinearAttFunction		= PAttenuationComponentFunction(	& GL3LightRenderer::_applyLinearAtt);
	m_pfnQuadraticAttFunction	= PAttenuationComponentFunction(	& GL3LightRenderer::_applyQuadraticAtt);
	m_pfnExponentFunction		= PSpotComponentFunction(			& GL3LightRenderer::_applyExponent);
	m_pfnCutOffFunction			= PSpotComponentFunction(			& GL3LightRenderer::_applyCutOff);
}

void GL3LightRenderer :: Initialise()
{
	m_bChanged = true;
	m_iGLPreviousIndex = GL_INVALID_INDEX;
}

void GL3LightRenderer :: _enable()
{
	GL3ShaderProgram * l_pProgram = ((GL3ShaderProgram *)RenderSystem::GetSingletonPtr()->GetCurrentShaderProgram());

	if (l_pProgram != NULL)
	{
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
}

void GL3LightRenderer :: _disable()
{
	if (m_iGLIndex != GL_INVALID_INDEX)
	{
		((GL3ShaderProgram *)RenderSystem::GetSingletonPtr()->GetCurrentShaderProgram())->FreeLight( m_iGLIndex);
	}
}

void GL3LightRenderer :: _applyAmbient( const Colour & p_ambient)
{
	m_bChanged = m_pAmbient != p_ambient;
	m_pAmbient = p_ambient;
}

void GL3LightRenderer :: _applyDiffuse( const Colour & p_diffuse)
{
	m_bChanged = m_pDiffuse != p_diffuse;
	m_pDiffuse = p_diffuse;
}

void GL3LightRenderer :: _applySpecular( const Colour & p_specular)
{
	m_bChanged = m_pSpecular != p_specular;
	m_pSpecular = p_specular;
}

void GL3LightRenderer :: _applyPosition( const Point4f & p_position)
{
	m_bChanged = m_pPosition != p_position;
	m_pPosition = p_position;
}

void GL3LightRenderer :: _applyOrientation( const Matrix4x4r & p_orientation)
{
	m_bChanged = m_pOrientation != p_orientation;
	m_pOrientation = p_orientation;
}

void GL3LightRenderer :: _applyConstantAtt( float p_constant)
{
	m_bChanged = m_pAttenuation[0] != p_constant;
	m_pAttenuation[0] = p_constant;
}

void GL3LightRenderer :: _applyLinearAtt( float p_linear)
{
	m_bChanged = m_pAttenuation[1] != p_linear;
	m_pAttenuation[1] = p_linear;
}

void GL3LightRenderer :: _applyQuadraticAtt( float p_quadratic)
{
	m_bChanged = m_pAttenuation[2] != p_quadratic;
	m_pAttenuation[2] = p_quadratic;
}

void GL3LightRenderer :: _applyExponent( float p_exponent)
{
	m_bChanged = m_pExponent != p_exponent;
	m_pExponent = p_exponent;
}

void GL3LightRenderer :: _applyCutOff( float p_cutOff)
{
	m_bChanged = m_pCutOff != p_cutOff;
	m_pCutOff = p_cutOff;
}
