#include "PrecompiledHeader.h"

#include "GL4LightRenderer.h"
#include "GL3RenderSystem.h"
#include "GL4ShaderProgram.h"
#include "GL3Buffer.h"

using namespace Castor3D;

GL4LightRenderer :: GL4LightRenderer()
	:	GLLightRenderer(),
		m_bInitialised( false),
		m_bChanged( false),
		m_iGLPreviousIndex( GL_INVALID_INDEX),
		m_iGLIndex( GL_INVALID_INDEX)
{
	m_pfnEnableFunction			= PStateFunction(					& GL4LightRenderer::_enable);
	m_pfnDisableFunction		= PStateFunction(					& GL4LightRenderer::_disable);
	m_pfnAmbientFunction		= PColourFunction(					& GL4LightRenderer::_applyAmbient);
	m_pfnDiffuseFunction		= PColourFunction(					& GL4LightRenderer::_applyDiffuse);
	m_pfnSpecularFunction		= PColourFunction(					& GL4LightRenderer::_applySpecular);
	m_pfnOrientationFunction	= POrientationFunction(				& GL4LightRenderer::_applyOrientation);
	m_pfnPositionFunction		= PPositionFunction(				& GL4LightRenderer::_applyPosition);
	m_pfnConstantAttFunction	= PAttenuationComponentFunction(	& GL4LightRenderer::_applyConstantAtt);
	m_pfnLinearAttFunction		= PAttenuationComponentFunction(	& GL4LightRenderer::_applyLinearAtt);
	m_pfnQuadraticAttFunction	= PAttenuationComponentFunction(	& GL4LightRenderer::_applyQuadraticAtt);
	m_pfnExponentFunction		= PSpotComponentFunction(			& GL4LightRenderer::_applyExponent);
	m_pfnCutOffFunction			= PSpotComponentFunction(			& GL4LightRenderer::_applyCutOff);
}

void GL4LightRenderer :: Initialise()
{
	m_bChanged = true;
	m_iGLPreviousIndex = GL_INVALID_INDEX;
}

void GL4LightRenderer :: _enable()
{
	GL4ShaderProgram * l_pProgram = ((GL4ShaderProgram *)RenderSystem::GetSingletonPtr()->GetCurrentShaderProgram());

	if (l_pProgram != NULL)
	{
		m_iGLIndex = l_pProgram->AssignLight();
		GLUniformBufferObjectPtr l_pUniformBuffer = l_pProgram->GetUniformBuffer( "in_Lights");

		if (l_pUniformBuffer != m_pUniformBuffer)
		{
			m_pUniformBuffer = l_pUniformBuffer;
			m_pAmbients		= m_pUniformBuffer->GetVariable<GLUBOPoint4fVariable>(		"in_LightsAmbient");
			m_pDiffuses		= m_pUniformBuffer->GetVariable<GLUBOPoint4fVariable>(		"in_LightsDiffuse");
			m_pSpeculars		= m_pUniformBuffer->GetVariable<GLUBOPoint4fVariable>(		"in_LightsSpecular");
			m_pPositions		= m_pUniformBuffer->GetVariable<GLUBOPoint4fVariable>(		"in_LightsPosition");
			m_pAttenuations	= m_pUniformBuffer->GetVariable<GLUBOPoint3fVariable>(		"in_LightsAttenuation");
			m_pOrientations	= m_pUniformBuffer->GetVariable<GLUBOMatrix4x4fVariable>(	"in_LightsOrientation");
			m_pExponents		= m_pUniformBuffer->GetVariable<GLUBOFloatVariable>(		"in_LightsExponent");
			m_pCutOffs			= m_pUniformBuffer->GetVariable<GLUBOFloatVariable>(		"in_LightsCutOff");
		}

		if (m_iGLIndex != GL_INVALID_INDEX && ! m_pUniformBuffer.null() && m_pUniformBuffer->m_iUniformBlockIndex != GL_INVALID_INDEX && m_pUniformBuffer->m_uiIndex != GL_INVALID_INDEX && (m_bChanged || m_iGLIndex != m_iGLPreviousIndex))
		{
			if ( ! m_pAmbients.null())
			{
				m_pAmbients->SetValue(		m_varAmbient,		m_iGLIndex);
			}
			if ( ! m_pDiffuses.null())
			{
				m_pDiffuses->SetValue(		m_varDiffuse,		m_iGLIndex);
			}
			if ( ! m_pSpeculars.null())
			{
				m_pSpeculars->SetValue(	m_varSpecular,		m_iGLIndex);
			}
			if ( ! m_pPositions.null())
			{
				m_pPositions->SetValue(	m_varPosition,		m_iGLIndex);
			}
			if ( ! m_pAttenuations.null())
			{
				m_pAttenuations->SetValue(	m_varAttenuation,	m_iGLIndex);
			}
			if ( ! m_pOrientations.null())
			{
				m_pOrientations->SetValue(	m_varOrientation,	m_iGLIndex);
			}
			if ( ! m_pExponents.null())
			{
				m_pExponents->SetValue(	m_varExponent,		m_iGLIndex);
			}
			if ( ! m_pCutOffs.null())
			{
				m_pCutOffs->SetValue(		m_varCutOff,		m_iGLIndex);
			}
			SetGLPreviousIndex( m_iGLIndex);
			m_bChanged = false;
		}
	}
}

void GL4LightRenderer :: _disable()
{
	if (m_iGLIndex != GL_INVALID_INDEX)
	{
		((GL4ShaderProgram *)RenderSystem::GetSingletonPtr()->GetCurrentShaderProgram())->FreeLight( m_iGLIndex);
	}
}

void GL4LightRenderer :: _applyAmbient( const Colour & p_ambient)
{
	m_bChanged = m_varAmbient != p_ambient;
	m_varAmbient = p_ambient;
}

void GL4LightRenderer :: _applyDiffuse( const Colour & p_diffuse)
{
	m_bChanged = m_varDiffuse != p_diffuse;
	m_varDiffuse = p_diffuse;
}

void GL4LightRenderer :: _applySpecular( const Colour & p_specular)
{
	m_bChanged = m_varSpecular != p_specular;
	m_varSpecular = p_specular;
}

void GL4LightRenderer :: _applyExponent( float p_exponent)
{
	m_bChanged = m_varExponent != p_exponent;
	m_varExponent = p_exponent;
}

void GL4LightRenderer :: _applyCutOff( float p_cutOff)
{
	m_bChanged = m_varCutOff != p_cutOff;
	m_varCutOff = p_cutOff;
}

void GL4LightRenderer :: _applyPosition( const Point4f & p_position)
{
	m_bChanged = m_varPosition != p_position;
	m_varPosition = p_position;
}

void GL4LightRenderer :: _applyOrientation( const Matrix4x4r & p_orientation)
{
	m_bChanged = m_varOrientation != p_orientation;
	m_varOrientation = p_orientation;
}

void GL4LightRenderer :: _applyConstantAtt( float p_constant)
{
	m_bChanged = m_varAttenuation[0] != p_constant;
	m_varAttenuation[0] = p_constant;
}

void GL4LightRenderer :: _applyLinearAtt( float p_linear)
{
	m_bChanged = m_varAttenuation[1] != p_linear;
	m_varAttenuation[1] = p_linear;
}

void GL4LightRenderer :: _applyQuadraticAtt( float p_quadratic)
{
	m_bChanged = m_varAttenuation[2] != p_quadratic;
	m_varAttenuation[2] = p_quadratic;
}