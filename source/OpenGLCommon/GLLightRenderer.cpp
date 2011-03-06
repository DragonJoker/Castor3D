#include "OpenGlCommon/PrecompiledHeader.h"

#include "OpenGlCommon/GlLightRenderer.h"

using namespace Castor3D;

GlLightRenderer :: GlLightRenderer( SceneManager * p_pSceneManager)
	:	LightRenderer( p_pSceneManager)
	,	m_iIndex( GL_INVALID_INDEX)
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

void GlLightRenderer :: Initialise()
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

void GlLightRenderer :: Enable()
{
	m_iIndex = RenderSystem::GetSingletonPtr()->LockLight();

	if (m_iIndex >= 0)
	{
		(this->*m_pfnEnableFunction)();
	}
}

void GlLightRenderer :: Disable()
{
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
	CheckGlError( glEnable( m_iIndex), CU_T( "GlLightRenderer :: Enable - glEnable"));
}

void GlLightRenderer :: _disable()
{
	CheckGlError( glDisable( m_iIndex), CU_T( "GlLightRenderer :: Disable - glDisable"));
	m_iIndex = -1;
}

void GlLightRenderer :: _applyAmbient( const Colour & p_ambient)
{
	CheckGlError( glLightfv( m_iIndex, GL_AMBIENT, p_ambient.const_ptr()), CU_T( "GlLightRenderer :: ApplyAmbient - glLightfv"));
}

void GlLightRenderer :: _applyDiffuse( const Colour & p_diffuse)
{
	CheckGlError( glLightfv( m_iIndex, GL_DIFFUSE, p_diffuse.const_ptr()), CU_T( "GlLightRenderer :: ApplyDiffuse - glLightfv"));
}

void GlLightRenderer :: _applySpecular( const Colour & p_specular)
{
	CheckGlError( glLightfv( m_iIndex, GL_SPECULAR, p_specular.const_ptr()), CU_T( "GlLightRenderer :: ApplySpecular - glLightfv"));
}

void GlLightRenderer :: _applyPosition( const Point4f & p_position)
{
	CheckGlError( glLightfv( m_iIndex, GL_POSITION, p_position.const_ptr()), CU_T( "GlLightRenderer :: ApplyPosition - glLightfv"));
}

void GlLightRenderer :: _applyOrientation( const Matrix4x4r & p_orientation)
{
	CheckGlError( glLightfv( m_iIndex, GL_SPOT_DIRECTION, Matrix4x4f( p_orientation).const_ptr()), CU_T( "GlLightRenderer :: ApplyPosition - glLightfv"));
}

void GlLightRenderer :: _applyConstantAtt( float p_constant)
{
	CheckGlError( glLightf( m_iIndex, GL_CONSTANT_ATTENUATION, p_constant), CU_T( "GlLightRenderer :: ApplyConstantAtt"));
}

void GlLightRenderer :: _applyLinearAtt( float p_linear)
{
	CheckGlError( glLightf( m_iIndex, GL_LINEAR_ATTENUATION, p_linear), CU_T( "GlLightRenderer :: ApplyLinearAtt"));
}

void GlLightRenderer :: _applyQuadraticAtt( float p_quadratic)
{
	CheckGlError( glLightf( m_iIndex, GL_QUADRATIC_ATTENUATION, p_quadratic), CU_T( "GlLightRenderer :: ApplyQuadraticAtt"));
}

void GlLightRenderer :: _applyExponent( float p_exponent)
{
	CheckGlError( glLightf( m_iIndex, GL_SPOT_EXPONENT, p_exponent), CU_T( "GlLightRenderer :: ApplyExponent - glLightfv"));
}

void GlLightRenderer :: _applyCutOff( float p_cutOff)
{
	CheckGlError( glLightf( m_iIndex, GL_SPOT_CUTOFF, p_cutOff), CU_T( "GlLightRenderer :: ApplyCutOff - glLightf"));
}