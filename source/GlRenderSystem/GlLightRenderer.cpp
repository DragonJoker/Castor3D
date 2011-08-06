#include "GlRenderSystem/PrecompiledHeader.hpp"

#include "GlRenderSystem/GlLightRenderer.hpp"
#include "GlRenderSystem/GlRenderSystem.hpp"
#include "GlRenderSystem/GlShaderProgram.hpp"
#include "GlRenderSystem/CgGlShaderProgram.hpp"
#include "GlRenderSystem/OpenGl.hpp"

using namespace Castor3D;

GlLightRenderer :: GlLightRenderer()
	:	LightRenderer		()
	,	m_iIndex			( GL_INVALID_INDEX)
	,	m_bChanged			( true)
	,	m_iGlIndex			( GL_INVALID_INDEX)
	,	m_iGlPreviousIndex	( GL_INVALID_INDEX)
{
	m_pfnEnableFunction			= & GlLightRenderer::_enableGlsl;
	m_pfnDisableFunction		= & GlLightRenderer::_disableGlsl;
	m_pfnAmbientFunction		= & GlLightRenderer::_applyAmbientGlsl;
	m_pfnDiffuseFunction		= & GlLightRenderer::_applyDiffuseGlsl;
	m_pfnSpecularFunction		= & GlLightRenderer::_applySpecularGlsl;
	m_pfnOrientationFunction	= & GlLightRenderer::_applyOrientationGlsl;
	m_pfnPositionFunction		= & GlLightRenderer::_applyPositionGlsl;
	m_pfnConstantAttFunction	= & GlLightRenderer::_applyConstantAttGlsl;
	m_pfnLinearAttFunction		= & GlLightRenderer::_applyLinearAttGlsl;
	m_pfnQuadraticAttFunction	= & GlLightRenderer::_applyQuadraticAttGlsl;
	m_pfnExponentFunction		= & GlLightRenderer::_applyExponentGlsl;
	m_pfnCutOffFunction			= & GlLightRenderer::_applyCutOffGlsl;
}

void GlLightRenderer :: Initialise()
{
	CASTOR_TRACK;
	m_bChanged = true;
	m_iGlPreviousIndex = GL_INVALID_INDEX;

	if (GlRenderSystem::GetOpenGlMajor() < 3)
	{
		if (RenderSystem::UseShaders() && GlRenderSystem::GetSingletonPtr()->GetCurrentShaderProgram())
		{
			m_pfnEnableFunction			= & GlLightRenderer::_enableGlsl;
			m_pfnDisableFunction		= & GlLightRenderer::_disableGlsl;
			m_pfnAmbientFunction		= & GlLightRenderer::_applyAmbientGlsl;
			m_pfnDiffuseFunction		= & GlLightRenderer::_applyDiffuseGlsl;
			m_pfnSpecularFunction		= & GlLightRenderer::_applySpecularGlsl;
			m_pfnOrientationFunction	= & GlLightRenderer::_applyOrientationGlsl;
			m_pfnPositionFunction		= & GlLightRenderer::_applyPositionGlsl;
			m_pfnConstantAttFunction	= & GlLightRenderer::_applyConstantAttGlsl;
			m_pfnLinearAttFunction		= & GlLightRenderer::_applyLinearAttGlsl;
			m_pfnQuadraticAttFunction	= & GlLightRenderer::_applyQuadraticAttGlsl;
			m_pfnExponentFunction		= & GlLightRenderer::_applyExponentGlsl;
			m_pfnCutOffFunction			= & GlLightRenderer::_applyCutOffGlsl;
		}
		else
		{
			m_pfnEnableFunction			= & GlLightRenderer::_enable;
			m_pfnDisableFunction		= & GlLightRenderer::_disable;
			m_pfnAmbientFunction		= & GlLightRenderer::_applyAmbient;
			m_pfnDiffuseFunction		= & GlLightRenderer::_applyDiffuse;
			m_pfnSpecularFunction		= & GlLightRenderer::_applySpecular;
			m_pfnOrientationFunction	= & GlLightRenderer::_applyOrientation;
			m_pfnPositionFunction		= & GlLightRenderer::_applyPosition;
			m_pfnConstantAttFunction	= & GlLightRenderer::_applyConstantAtt;
			m_pfnLinearAttFunction		= & GlLightRenderer::_applyLinearAtt;
			m_pfnQuadraticAttFunction	= & GlLightRenderer::_applyQuadraticAtt;
			m_pfnExponentFunction		= & GlLightRenderer::_applyExponent;
			m_pfnCutOffFunction			= & GlLightRenderer::_applyCutOff;
		}
	}
	else
#if CASTOR_USE_OPENGL4
	if (GlRenderSystem::GetOpenGlMajor() < 4)
#endif
	{
		m_pfnEnableFunction			= & GlLightRenderer::_enableGlsl;
		m_pfnDisableFunction		= & GlLightRenderer::_disableGlsl;
		m_pfnAmbientFunction		= & GlLightRenderer::_applyAmbientGlsl;
		m_pfnDiffuseFunction		= & GlLightRenderer::_applyDiffuseGlsl;
		m_pfnSpecularFunction		= & GlLightRenderer::_applySpecularGlsl;
		m_pfnOrientationFunction	= & GlLightRenderer::_applyOrientationGlsl;
		m_pfnPositionFunction		= & GlLightRenderer::_applyPositionGlsl;
		m_pfnConstantAttFunction	= & GlLightRenderer::_applyConstantAttGlsl;
		m_pfnLinearAttFunction		= & GlLightRenderer::_applyLinearAttGlsl;
		m_pfnQuadraticAttFunction	= & GlLightRenderer::_applyQuadraticAttGlsl;
		m_pfnExponentFunction		= & GlLightRenderer::_applyExponentGlsl;
		m_pfnCutOffFunction			= & GlLightRenderer::_applyCutOffGlsl;
	}
#if CASTOR_USE_OPENGL4
	else
	{
		m_pfnEnableFunction			= & GlLightRenderer::_enableGl4;
		m_pfnDisableFunction		= & GlLightRenderer::_disableGlsl;
		m_pfnAmbientFunction		= & GlLightRenderer::_applyAmbientGlsl;
		m_pfnDiffuseFunction		= & GlLightRenderer::_applyDiffuseGlsl;
		m_pfnSpecularFunction		= & GlLightRenderer::_applySpecularGlsl;
		m_pfnOrientationFunction	= & GlLightRenderer::_applyOrientationGlsl;
		m_pfnPositionFunction		= & GlLightRenderer::_applyPositionGlsl;
		m_pfnConstantAttFunction	= & GlLightRenderer::_applyConstantAttGlsl;
		m_pfnLinearAttFunction		= & GlLightRenderer::_applyLinearAttGlsl;
		m_pfnQuadraticAttFunction	= & GlLightRenderer::_applyQuadraticAttGlsl;
		m_pfnExponentFunction		= & GlLightRenderer::_applyExponentGlsl;
		m_pfnCutOffFunction			= & GlLightRenderer::_applyCutOffGlsl;
	}
#endif
}

void GlLightRenderer :: Enable()
{
	CASTOR_TRACK;
	m_iIndex = RenderSystem::GetSingletonPtr()->LockLight();

	if (m_iIndex >= 0)
	{
		(this->*m_pfnEnableFunction)();
	}
}

void GlLightRenderer :: Disable()
{
	CASTOR_TRACK;
	RenderSystem::GetSingletonPtr()->UnlockLight( m_iIndex);

	if (m_iIndex >= 0)
	{
		(this->*m_pfnDisableFunction)();
	}
}

void GlLightRenderer :: ApplyAmbient()
{
	if (m_iIndex >= 0)
	{
		(this->*m_pfnAmbientFunction)( m_target->GetAmbient());
	}
}

void GlLightRenderer :: ApplyDiffuse()
{
	if (m_iIndex >= 0)
	{
		(this->*m_pfnDiffuseFunction)( m_target->GetDiffuse());
	}
}

void GlLightRenderer :: ApplySpecular()
{
	if (m_iIndex >= 0)
	{
		(this->*m_pfnSpecularFunction)( m_target->GetSpecular());
	}
}

void GlLightRenderer :: ApplyPosition()
{
	if (m_iIndex >= 0)
	{
		(this->*m_pfnPositionFunction)( Point4f( float( m_target->GetPositionType()[0]), float( m_target->GetPositionType()[1]), float( m_target->GetPositionType()[2]), float( m_target->GetPositionType()[3])));
	}
}

void GlLightRenderer :: ApplyOrientation()
{
	if (m_iIndex >= 0)
	{
		(this->*m_pfnOrientationFunction)( m_target->GetParent()->GetRotationMatrix());
	}
}

void GlLightRenderer :: ApplyConstantAtt( float p_constant)
{
	if (m_iIndex >= 0)
	{
		(this->*m_pfnConstantAttFunction)( p_constant);
	}
}

void GlLightRenderer :: ApplyLinearAtt( float p_linear)
{
	if (m_iIndex >= 0)
	{
		(this->*m_pfnLinearAttFunction)( p_linear);
	}
}

void GlLightRenderer :: ApplyQuadraticAtt( float p_quadratic)
{
	if (m_iIndex >= 0)
	{
		(this->*m_pfnQuadraticAttFunction)( p_quadratic);
	}
}

void GlLightRenderer :: ApplyExponent( float p_fExponent)
{
	if (m_iIndex >= 0)
	{
		(this->*m_pfnExponentFunction)( p_fExponent);
	}
}

void GlLightRenderer :: ApplyCutOff( float p_fCutOff)
{
	if (m_iIndex >= 0)
	{
		(this->*m_pfnCutOffFunction)( p_fCutOff);
	}
}

void GlLightRenderer :: _enable()
{
	CASTOR_TRACK;
	OpenGl::Enable( m_iIndex);
}

void GlLightRenderer :: _disable()
{
	CASTOR_TRACK;
	OpenGl::Disable( m_iIndex);
	m_iIndex = -1;
}

void GlLightRenderer :: _applyAmbient( Colour const & p_ambient)
{
	OpenGl::Lightfv( m_iIndex, GL_AMBIENT, p_ambient.const_ptr());
}

void GlLightRenderer :: _applyDiffuse( Colour const & p_diffuse)
{
	OpenGl::Lightfv( m_iIndex, GL_DIFFUSE, p_diffuse.const_ptr());
}

void GlLightRenderer :: _applySpecular( Colour const & p_specular)
{
	OpenGl::Lightfv( m_iIndex, GL_SPECULAR, p_specular.const_ptr());
}

void GlLightRenderer :: _applyPosition( const Point4f & p_position)
{
	OpenGl::Lightfv( m_iIndex, GL_POSITION, p_position.const_ptr());
}

void GlLightRenderer :: _applyOrientation( Matrix4x4r const & p_orientation)
{
	OpenGl::Lightfv( m_iIndex, GL_SPOT_DIRECTION, Matrix4x4f( p_orientation).const_ptr());
}

void GlLightRenderer :: _applyConstantAtt( float p_constant)
{
	OpenGl::Lightf( m_iIndex, GL_CONSTANT_ATTENUATION, p_constant);
}

void GlLightRenderer :: _applyLinearAtt( float p_linear)
{
	OpenGl::Lightf( m_iIndex, GL_LINEAR_ATTENUATION, p_linear);
}

void GlLightRenderer :: _applyQuadraticAtt( float p_quadratic)
{
	OpenGl::Lightf( m_iIndex, GL_QUADRATIC_ATTENUATION, p_quadratic);
}

void GlLightRenderer :: _applyExponent( float p_exponent)
{
	OpenGl::Lightf( m_iIndex, GL_SPOT_EXPONENT, p_exponent);
}

void GlLightRenderer :: _applyCutOff( float p_cutOff)
{
	OpenGl::Lightf( m_iIndex, GL_SPOT_CUTOFF, p_cutOff);
}

void GlLightRenderer :: _enableGlsl()
{
	CASTOR_TRACK;
	ShaderProgramBase * l_pProgram = GlRenderSystem::GetSingletonPtr()->GetCurrentShaderProgram();

	if (l_pProgram)
	{
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
}

void GlLightRenderer :: _disableGlsl()
{
	CASTOR_TRACK;
	if (m_iGlIndex != GL_INVALID_INDEX)
	{
		GlRenderSystem::GetSingletonPtr()->GetCurrentShaderProgram()->FreeLight( m_iGlIndex);
	}
}

void GlLightRenderer :: _applyAmbientGlsl( Colour const & p_ambient)
{
	m_bChanged = m_pAmbient != p_ambient;
	m_pAmbient = p_ambient;
}

void GlLightRenderer :: _applyDiffuseGlsl( Colour const & p_diffuse)
{
	m_bChanged = m_pDiffuse != p_diffuse;
	m_pDiffuse = p_diffuse;
}

void GlLightRenderer :: _applySpecularGlsl( Colour const & p_specular)
{
	m_bChanged = m_pSpecular != p_specular;
	m_pSpecular = p_specular;
}

void GlLightRenderer :: _applyPositionGlsl( const Point4f & p_position)
{
	m_bChanged = m_pPosition != p_position;
	m_pPosition = p_position;
}

void GlLightRenderer :: _applyOrientationGlsl( Matrix4x4r const & p_orientation)
{
	m_bChanged = m_pOrientation != p_orientation;
	m_pOrientation = p_orientation;
}

void GlLightRenderer :: _applyConstantAttGlsl( float p_constant)
{
	m_bChanged = m_pAttenuation[0] != p_constant;
	m_pAttenuation[0] = p_constant;
}

void GlLightRenderer :: _applyLinearAttGlsl( float p_linear)
{
	m_bChanged = m_pAttenuation[1] != p_linear;
	m_pAttenuation[1] = p_linear;
}

void GlLightRenderer :: _applyQuadraticAttGlsl( float p_quadratic)
{
	m_bChanged = m_pAttenuation[2] != p_quadratic;
	m_pAttenuation[2] = p_quadratic;
}

void GlLightRenderer :: _applyExponentGlsl( float p_exponent)
{
	m_bChanged = m_pExponent != p_exponent;
	m_pExponent = p_exponent;
}

void GlLightRenderer :: _applyCutOffGlsl( float p_cutOff)
{
	m_bChanged = m_pCutOff != p_cutOff;
	m_pCutOff = p_cutOff;
}
#if CASTOR_USE_OPENGL4
void GlLightRenderer :: _enableGl4()
{
	GlShaderProgram * l_pProgram = GlRenderSystem::GetSingletonPtr()->GetCurrentShaderProgram();

	if (l_pProgram)
	{
		m_iGlIndex = l_pProgram->AssignLight();
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

		if (m_iGlIndex != GL_INVALID_INDEX && m_pUniformBuffer && m_pUniformBuffer->m_iUniformBlockIndex != GL_INVALID_INDEX && m_pUniformBuffer->m_uiIndex != GL_INVALID_INDEX && (m_bChanged || m_iGlIndex != m_iGlPreviousIndex))
		{
			if (m_pAmbients)
			{
				m_pAmbients->SetValue(		m_pAmbient,		m_iGlIndex);
			}
			if (m_pDiffuses)
			{
				m_pDiffuses->SetValue(		m_pDiffuse,		m_iGlIndex);
			}
			if (m_pSpeculars)
			{
				m_pSpeculars->SetValue(		m_pSpecular,	m_iGlIndex);
			}
			if (m_pPositions)
			{
				m_pPositions->SetValue(		m_pPosition,	m_iGlIndex);
			}
			if (m_pAttenuations)
			{
				m_pAttenuations->SetValue(	m_pAttenuation,	m_iGlIndex);
			}
			if (m_pOrientations)
			{
				m_pOrientations->SetValue(	m_pOrientation,	m_iGlIndex);
			}
			if (m_pExponents)
			{
				m_pExponents->SetValue(		m_pExponent,	m_iGlIndex);
			}
			if (m_pCutOffs)
			{
				m_pCutOffs->SetValue(		m_pCutOff,		m_iGlIndex);
			}

			SetGlPreviousIndex( m_iGlIndex);
			m_bChanged = false;
		}
	}
}
#endif
