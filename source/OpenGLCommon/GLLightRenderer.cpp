#include "OpenGLCommon/PrecompiledHeader.h"

#include "OpenGLCommon/GLLightRenderer.h"

using namespace Castor3D;

GLLightRenderer :: GLLightRenderer( SceneManager * p_pSceneManager)
	:	LightRenderer( p_pSceneManager)
	,	m_iIndex( GL_INVALID_INDEX)
{
	m_pfnEnableFunction			= & GLLightRenderer::_enable;
	m_pfnDisableFunction		= & GLLightRenderer::_disable;
	m_pfnAmbientFunction		= & GLLightRenderer::_applyAmbient;
	m_pfnDiffuseFunction		= & GLLightRenderer::_applyDiffuse;
	m_pfnSpecularFunction		= & GLLightRenderer::_applySpecular;
	m_pfnOrientationFunction	= & GLLightRenderer::_applyOrientation;
	m_pfnPositionFunction		= & GLLightRenderer::_applyPosition;
	m_pfnConstantAttFunction	= & GLLightRenderer::_applyConstantAtt;
	m_pfnLinearAttFunction		= & GLLightRenderer::_applyLinearAtt;
	m_pfnQuadraticAttFunction	= & GLLightRenderer::_applyQuadraticAtt;
	m_pfnExponentFunction		= & GLLightRenderer::_applyExponent;
	m_pfnCutOffFunction			= & GLLightRenderer::_applyCutOff;
}

void GLLightRenderer :: Initialise()
{
	m_pfnEnableFunction			= & GLLightRenderer::_enable;
	m_pfnDisableFunction		= & GLLightRenderer::_disable;
	m_pfnAmbientFunction		= & GLLightRenderer::_applyAmbient;
	m_pfnDiffuseFunction		= & GLLightRenderer::_applyDiffuse;
	m_pfnSpecularFunction		= & GLLightRenderer::_applySpecular;
	m_pfnOrientationFunction	= & GLLightRenderer::_applyOrientation;
	m_pfnPositionFunction		= & GLLightRenderer::_applyPosition;
	m_pfnConstantAttFunction	= & GLLightRenderer::_applyConstantAtt;
	m_pfnLinearAttFunction		= & GLLightRenderer::_applyLinearAtt;
	m_pfnQuadraticAttFunction	= & GLLightRenderer::_applyQuadraticAtt;
	m_pfnExponentFunction		= & GLLightRenderer::_applyExponent;
	m_pfnCutOffFunction			= & GLLightRenderer::_applyCutOff;
}

void GLLightRenderer :: Enable()
{
	m_iIndex = RenderSystem::GetSingletonPtr()->LockLight();

	if (m_iIndex >= 0)
	{
		(this->*m_pfnEnableFunction)();
	}
}

void GLLightRenderer :: Disable()
{
	RenderSystem::GetSingletonPtr()->UnlockLight( m_iIndex);

	if (m_iIndex >= 0)
	{
		(this->*m_pfnDisableFunction)();
	}
}

void GLLightRenderer :: ApplyAmbient()
{
	if (m_iIndex >= 0)
	{
		(this->*m_pfnAmbientFunction)( m_target->GetAmbient());
	}
}

void GLLightRenderer :: ApplyDiffuse()
{
	if (m_iIndex >= 0)
	{
		(this->*m_pfnDiffuseFunction)( m_target->GetDiffuse());
	}
}

void GLLightRenderer :: ApplySpecular()
{
	if (m_iIndex >= 0)
	{
		(this->*m_pfnSpecularFunction)( m_target->GetSpecular());
	}
}

void GLLightRenderer :: ApplyPosition()
{
	if (m_iIndex >= 0)
	{
		(this->*m_pfnPositionFunction)( Point4f( float( m_target->GetPositionType()[0]), float( m_target->GetPositionType()[1]), float( m_target->GetPositionType()[2]), float( m_target->GetPositionType()[3])));
	}
}

void GLLightRenderer :: ApplyOrientation()
{
	if (m_iIndex >= 0)
	{
		(this->*m_pfnOrientationFunction)( m_target->GetParent()->GetRotationMatrix());
	}
}

void GLLightRenderer :: ApplyConstantAtt( float p_constant)
{
	if (m_iIndex >= 0)
	{
		(this->*m_pfnConstantAttFunction)( p_constant);
	}
}

void GLLightRenderer :: ApplyLinearAtt( float p_linear)
{
	if (m_iIndex >= 0)
	{
		(this->*m_pfnLinearAttFunction)( p_linear);
	}
}

void GLLightRenderer :: ApplyQuadraticAtt( float p_quadratic)
{
	if (m_iIndex >= 0)
	{
		(this->*m_pfnQuadraticAttFunction)( p_quadratic);
	}
}

void GLLightRenderer :: ApplyExponent( float p_fExponent)
{
	if (m_iIndex >= 0)
	{
		(this->*m_pfnExponentFunction)( p_fExponent);
	}
}

void GLLightRenderer :: ApplyCutOff( float p_fCutOff)
{
	if (m_iIndex >= 0)
	{
		(this->*m_pfnCutOffFunction)( p_fCutOff);
	}
}

void GLLightRenderer :: _enable()
{
	glEnable( m_iIndex);
	CheckGLError( CU_T( "GLLightRenderer :: Enable - glEnable"));
}

void GLLightRenderer :: _disable()
{
	glDisable( m_iIndex);
	m_iIndex = -1;
	CheckGLError( CU_T( "GLLightRenderer :: Disable - glDisable"));
}

void GLLightRenderer :: _applyAmbient( const Colour & p_ambient)
{
	glLightfv( m_iIndex, GL_AMBIENT, p_ambient.const_ptr());
	CheckGLError( CU_T( "GLLightRenderer :: ApplyAmbient - glLightfv"));
}

void GLLightRenderer :: _applyDiffuse( const Colour & p_diffuse)
{
	glLightfv( m_iIndex, GL_DIFFUSE, p_diffuse.const_ptr());
	CheckGLError( CU_T( "GLLightRenderer :: ApplyDiffuse - glLightfv"));
}

void GLLightRenderer :: _applySpecular( const Colour & p_specular)
{
	glLightfv( m_iIndex, GL_SPECULAR, p_specular.const_ptr());
	CheckGLError( CU_T( "GLLightRenderer :: ApplySpecular - glLightfv"));
}

void GLLightRenderer :: _applyPosition( const Point4f & p_position)
{
	glLightfv( m_iIndex, GL_POSITION, p_position.const_ptr());
	CheckGLError( CU_T( "GLLightRenderer :: ApplyPosition - glLightfv"));
}

void GLLightRenderer :: _applyOrientation( const Matrix4x4r & p_orientation)
{
	glLightfv( m_iIndex, GL_SPOT_DIRECTION, Matrix4x4f( p_orientation).const_ptr());
	CheckGLError( CU_T( "GLLightRenderer :: ApplyPosition - glLightfv"));
}

void GLLightRenderer :: _applyConstantAtt( float p_constant)
{
	glLightf( m_iIndex, GL_CONSTANT_ATTENUATION, p_constant);
	CheckGLError( CU_T( "GLLightRenderer :: ApplyConstantAtt"));
}

void GLLightRenderer :: _applyLinearAtt( float p_linear)
{
	glLightf( m_iIndex, GL_LINEAR_ATTENUATION, p_linear);
	CheckGLError( CU_T( "GLLightRenderer :: ApplyLinearAtt"));
}

void GLLightRenderer :: _applyQuadraticAtt( float p_quadratic)
{
	glLightf( m_iIndex, GL_QUADRATIC_ATTENUATION, p_quadratic);
	CheckGLError( CU_T( "GLLightRenderer :: ApplyQuadraticAtt"));
}

void GLLightRenderer :: _applyExponent( float p_exponent)
{
	glLightf( m_iIndex, GL_SPOT_EXPONENT, p_exponent);
	CheckGLError( CU_T( "GLLightRenderer :: ApplyExponent - glLightfv"));
}

void GLLightRenderer :: _applyCutOff( float p_cutOff)
{
	glLightf( m_iIndex, GL_SPOT_CUTOFF, p_cutOff);
	CheckGLError( CU_T( "GLLightRenderer :: ApplyCutOff - glLightf"));
}