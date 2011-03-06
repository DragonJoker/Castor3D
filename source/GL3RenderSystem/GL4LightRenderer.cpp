#include "Gl3RenderSystem/PrecompiledHeader.h"

#include "Gl3RenderSystem/Gl4LightRenderer.h"
#include "Gl3RenderSystem/Gl3RenderSystem.h"
#include "Gl3RenderSystem/Gl4ShaderProgram.h"
#include "Gl3RenderSystem/Gl3Buffer.h"

using namespace Castor3D;

Gl4LightRenderer :: Gl4LightRenderer( SceneManager * p_pSceneManager)
	:	GlLightRenderer( p_pSceneManager)
	,	m_bInitialised( false)
	,	m_bChanged( false)
	,	m_iGLPreviousIndex( GL_INVALID_INDEX)
	,	m_iGLIndex( GL_INVALID_INDEX)
{
	m_pfnEnableFunction			= PStateFunction(					& Gl4LightRenderer::_enable);
	m_pfnDisableFunction		= PStateFunction(					& Gl4LightRenderer::_disable);
	m_pfnAmbientFunction		= PColourFunction(					& Gl4LightRenderer::_applyAmbient);
	m_pfnDiffuseFunction		= PColourFunction(					& Gl4LightRenderer::_applyDiffuse);
	m_pfnSpecularFunction		= PColourFunction(					& Gl4LightRenderer::_applySpecular);
	m_pfnOrientationFunction	= POrientationFunction(				& Gl4LightRenderer::_applyOrientation);
	m_pfnPositionFunction		= PPositionFunction(				& Gl4LightRenderer::_applyPosition);
	m_pfnConstantAttFunction	= PAttenuationComponentFunction(	& Gl4LightRenderer::_applyConstantAtt);
	m_pfnLinearAttFunction		= PAttenuationComponentFunction(	& Gl4LightRenderer::_applyLinearAtt);
	m_pfnQuadraticAttFunction	= PAttenuationComponentFunction(	& Gl4LightRenderer::_applyQuadraticAtt);
	m_pfnExponentFunction		= PSpotComponentFunction(			& Gl4LightRenderer::_applyExponent);
	m_pfnCutOffFunction			= PSpotComponentFunction(			& Gl4LightRenderer::_applyCutOff);
}

Gl4LightRenderer :: ~Gl4LightRenderer()
{
}

void Gl4LightRenderer :: Initialise()
{
	m_bChanged = true;
	m_iGLPreviousIndex = GL_INVALID_INDEX;
}

void Gl4LightRenderer :: _enable()
{
	Gl4ShaderProgram * l_pProgram = ((Gl4ShaderProgram *)Gl3RenderSystem::GetSingletonPtr()->GetCurrentShaderProgram());

	if (l_pProgram != NULL)
	{
		m_iGLIndex = l_pProgram->AssignLight();
		GlUniformBufferObjectPtr l_pUniformBuffer = l_pProgram->GetUniformBuffer( "in_Lights");

		if (l_pUniformBuffer != m_pUniformBuffer)
		{
			m_pUniformBuffer = l_pUniformBuffer;
			m_pAmbients			= m_pUniformBuffer->GetVariable<GlUboPoint4fVariable>(		"in_LightsAmbient");
			m_pDiffuses			= m_pUniformBuffer->GetVariable<GlUboPoint4fVariable>(		"in_LightsDiffuse");
			m_pSpeculars		= m_pUniformBuffer->GetVariable<GlUboPoint4fVariable>(		"in_LightsSpecular");
			m_pPositions		= m_pUniformBuffer->GetVariable<GlUboPoint4fVariable>(		"in_LightsPosition");
			m_pAttenuations		= m_pUniformBuffer->GetVariable<GlUboPoint3fVariable>(		"in_LightsAttenuation");
			m_pOrientations		= m_pUniformBuffer->GetVariable<GlUboMatrix4x4fVariable>(	"in_LightsOrientation");
			m_pExponents		= m_pUniformBuffer->GetVariable<GlUboFloatVariable>(		"in_LightsExponent");
			m_pCutOffs			= m_pUniformBuffer->GetVariable<GlUboFloatVariable>(		"in_LightsCutOff");
		}

		if (m_iGLIndex != GL_INVALID_INDEX && m_pUniformBuffer != NULL && m_pUniformBuffer->m_iUniformBlockIndex != GL_INVALID_INDEX && m_pUniformBuffer->m_uiIndex != GL_INVALID_INDEX && (m_bChanged || m_iGLIndex != m_iGLPreviousIndex))
		{
			if (m_pAmbients != NULL)
			{
				m_pAmbients->SetValue(		m_varAmbient,		m_iGLIndex);
			}
			if (m_pDiffuses != NULL)
			{
				m_pDiffuses->SetValue(		m_varDiffuse,		m_iGLIndex);
			}
			if (m_pSpeculars != NULL)
			{
				m_pSpeculars->SetValue(	m_varSpecular,		m_iGLIndex);
			}
			if (m_pPositions != NULL)
			{
				m_pPositions->SetValue(	m_varPosition,		m_iGLIndex);
			}
			if (m_pAttenuations != NULL)
			{
				m_pAttenuations->SetValue(	m_varAttenuation,	m_iGLIndex);
			}
			if (m_pOrientations != NULL)
			{
				m_pOrientations->SetValue(	m_varOrientation,	m_iGLIndex);
			}
			if (m_pExponents != NULL)
			{
				m_pExponents->SetValue(	m_varExponent,		m_iGLIndex);
			}
			if (m_pCutOffs != NULL)
			{
				m_pCutOffs->SetValue(		m_varCutOff,		m_iGLIndex);
			}
			SetGLPreviousIndex( m_iGLIndex);
			m_bChanged = false;
		}
	}
}

void Gl4LightRenderer :: _disable()
{
	if (m_iGLIndex != GL_INVALID_INDEX)
	{
		((Gl4ShaderProgram *)Gl3RenderSystem::GetSingletonPtr()->GetCurrentShaderProgram())->FreeLight( m_iGLIndex);
	}
}

void Gl4LightRenderer :: _applyAmbient( const Colour & p_ambient)
{
	m_bChanged = m_varAmbient != p_ambient;
	m_varAmbient = p_ambient;
}

void Gl4LightRenderer :: _applyDiffuse( const Colour & p_diffuse)
{
	m_bChanged = m_varDiffuse != p_diffuse;
	m_varDiffuse = p_diffuse;
}

void Gl4LightRenderer :: _applySpecular( const Colour & p_specular)
{
	m_bChanged = m_varSpecular != p_specular;
	m_varSpecular = p_specular;
}

void Gl4LightRenderer :: _applyExponent( float p_exponent)
{
	m_bChanged = m_varExponent != p_exponent;
	m_varExponent = p_exponent;
}

void Gl4LightRenderer :: _applyCutOff( float p_cutOff)
{
	m_bChanged = m_varCutOff != p_cutOff;
	m_varCutOff = p_cutOff;
}

void Gl4LightRenderer :: _applyPosition( const Point4f & p_position)
{
	m_bChanged = m_varPosition != p_position;
	m_varPosition = p_position;
}

void Gl4LightRenderer :: _applyOrientation( const Matrix4x4r & p_orientation)
{
	m_bChanged = m_varOrientation != p_orientation;
	m_varOrientation = p_orientation;
}

void Gl4LightRenderer :: _applyConstantAtt( float p_constant)
{
	m_bChanged = m_varAttenuation[0] != p_constant;
	m_varAttenuation[0] = p_constant;
}

void Gl4LightRenderer :: _applyLinearAtt( float p_linear)
{
	m_bChanged = m_varAttenuation[1] != p_linear;
	m_varAttenuation[1] = p_linear;
}

void Gl4LightRenderer :: _applyQuadraticAtt( float p_quadratic)
{
	m_bChanged = m_varAttenuation[2] != p_quadratic;
	m_varAttenuation[2] = p_quadratic;
}
