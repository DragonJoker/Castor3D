#include "Gl3RenderSystem/PrecompiledHeader.h"

#include "Gl3RenderSystem/Gl3LightRenderer.h"
#include "Gl3RenderSystem/Gl3RenderSystem.h"
#include "Gl3RenderSystem/Gl3ShaderProgram.h"

using namespace Castor3D;

Gl3LightRenderer :: Gl3LightRenderer( SceneManager * p_pSceneManager)
	:	GlLightRenderer( p_pSceneManager),
		m_bChanged( true),
		m_iGLIndex( GL_INVALID_INDEX),
		m_iGLPreviousIndex( GL_INVALID_INDEX)
{
	m_pfnEnableFunction			= PStateFunction(					& Gl3LightRenderer::_enable);
	m_pfnDisableFunction		= PStateFunction(					& Gl3LightRenderer::_disable);
	m_pfnAmbientFunction		= PColourFunction(					& Gl3LightRenderer::_applyAmbient);
	m_pfnDiffuseFunction		= PColourFunction(					& Gl3LightRenderer::_applyDiffuse);
	m_pfnSpecularFunction		= PColourFunction(					& Gl3LightRenderer::_applySpecular);
	m_pfnOrientationFunction	= POrientationFunction(				& Gl3LightRenderer::_applyOrientation);
	m_pfnPositionFunction		= PPositionFunction(				& Gl3LightRenderer::_applyPosition);
	m_pfnConstantAttFunction	= PAttenuationComponentFunction(	& Gl3LightRenderer::_applyConstantAtt);
	m_pfnLinearAttFunction		= PAttenuationComponentFunction(	& Gl3LightRenderer::_applyLinearAtt);
	m_pfnQuadraticAttFunction	= PAttenuationComponentFunction(	& Gl3LightRenderer::_applyQuadraticAtt);
	m_pfnExponentFunction		= PSpotComponentFunction(			& Gl3LightRenderer::_applyExponent);
	m_pfnCutOffFunction			= PSpotComponentFunction(			& Gl3LightRenderer::_applyCutOff);
}

void Gl3LightRenderer :: Initialise()
{
	m_bChanged = true;
	m_iGLPreviousIndex = GL_INVALID_INDEX;
}

void Gl3LightRenderer :: _enable()
{
	Gl3ShaderProgram * l_pProgram = Gl3RenderSystem::GetSingletonPtr()->GetCurrentShaderProgram();

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

void Gl3LightRenderer :: _disable()
{
	if (m_iGLIndex != GL_INVALID_INDEX)
	{
		Gl3RenderSystem::GetSingletonPtr()->GetCurrentShaderProgram()->FreeLight( m_iGLIndex);
	}
}

void Gl3LightRenderer :: _applyAmbient( const Colour & p_ambient)
{
	m_bChanged = m_pAmbient != p_ambient;
	m_pAmbient = p_ambient;
}

void Gl3LightRenderer :: _applyDiffuse( const Colour & p_diffuse)
{
	m_bChanged = m_pDiffuse != p_diffuse;
	m_pDiffuse = p_diffuse;
}

void Gl3LightRenderer :: _applySpecular( const Colour & p_specular)
{
	m_bChanged = m_pSpecular != p_specular;
	m_pSpecular = p_specular;
}

void Gl3LightRenderer :: _applyPosition( const Point4f & p_position)
{
	m_bChanged = m_pPosition != p_position;
	m_pPosition = p_position;
}

void Gl3LightRenderer :: _applyOrientation( const Matrix4x4r & p_orientation)
{
	m_bChanged = m_pOrientation != p_orientation;
	m_pOrientation = p_orientation;
}

void Gl3LightRenderer :: _applyConstantAtt( float p_constant)
{
	m_bChanged = m_pAttenuation[0] != p_constant;
	m_pAttenuation[0] = p_constant;
}

void Gl3LightRenderer :: _applyLinearAtt( float p_linear)
{
	m_bChanged = m_pAttenuation[1] != p_linear;
	m_pAttenuation[1] = p_linear;
}

void Gl3LightRenderer :: _applyQuadraticAtt( float p_quadratic)
{
	m_bChanged = m_pAttenuation[2] != p_quadratic;
	m_pAttenuation[2] = p_quadratic;
}

void Gl3LightRenderer :: _applyExponent( float p_exponent)
{
	m_bChanged = m_pExponent != p_exponent;
	m_pExponent = p_exponent;
}

void Gl3LightRenderer :: _applyCutOff( float p_cutOff)
{
	m_bChanged = m_pCutOff != p_cutOff;
	m_pCutOff = p_cutOff;
}
