#include "GlLightRenderer.hpp"
#include "GlRenderSystem.hpp"
#include "GlShaderProgram.hpp"
#include "OpenGl.hpp"

#include <SceneNode.hpp>

using namespace GlRender;
using namespace Castor3D;
using namespace Castor;

GlLightRenderer::GlLightRenderer( OpenGl & p_gl, GlRenderSystem * p_pRenderSystem )
	:	LightRenderer( p_pRenderSystem	)
	,	m_eIndex( eGL_LIGHT_INDEX_INVALID	)
	,	m_bChanged( true	)
	,	m_iGlIndex( eGL_INVALID_INDEX	)
	,	m_iGlPreviousIndex( eGL_INVALID_INDEX	)
	,	m_gl( p_gl	)
{
}

void GlLightRenderer::Initialise()
{
	m_bChanged = true;
	m_mapChangedByProgram.clear();
	m_iGlPreviousIndex = eGL_INVALID_INDEX;
}

void GlLightRenderer::Enable()
{
	GlRenderSystem * l_pRenderSystem = static_cast< GlRenderSystem * >( m_pRenderSystem );
	m_eIndex = eGL_LIGHT_INDEX( l_pRenderSystem->LockLight() );

	if ( m_eIndex != eGL_LIGHT_INDEX_INVALID )
	{
		m_gl.Enable( m_eIndex );
	}
}

void GlLightRenderer::Disable()
{
	m_pRenderSystem->UnlockLight( m_eIndex );

	if ( m_eIndex != eGL_LIGHT_INDEX_INVALID )
	{
		m_gl.Disable( m_eIndex );
		m_eIndex = eGL_LIGHT_INDEX_INVALID;
	}
}

void GlLightRenderer::ApplyAmbient()
{
	if ( m_eIndex != eGL_LIGHT_INDEX_INVALID )
	{
		m_gl.Light( m_eIndex, eGL_LIGHT_COMPONENT_AMBIENT, m_target->GetAmbient().const_ptr() );
	}
}

void GlLightRenderer::ApplyDiffuse()
{
	if ( m_eIndex != eGL_LIGHT_INDEX_INVALID )
	{
		m_gl.Light( m_eIndex, eGL_LIGHT_COMPONENT_DIFFUSE, m_target->GetDiffuse().const_ptr() );
	}
}

void GlLightRenderer::ApplySpecular()
{
	if ( m_eIndex != eGL_LIGHT_INDEX_INVALID )
	{
		m_gl.Light( m_eIndex, eGL_LIGHT_COMPONENT_SPECULAR, m_target->GetSpecular().const_ptr() );
	}
}

void GlLightRenderer::ApplyPosition()
{
	if ( m_eIndex != eGL_LIGHT_INDEX_INVALID )
	{
		m_gl.Light( m_eIndex, eGL_LIGHT_COMPONENT_POSITION, m_target->GetPositionType().const_ptr() );
	}
}

void GlLightRenderer::ApplyOrientation()
{
	if ( m_eIndex != eGL_LIGHT_INDEX_INVALID )
	{
#if CASTOR_USE_DOUBLE
		m_gl.Light( m_eIndex, eGL_LIGHT_COMPONENT_SPOT_DIRECTION, Matrix4x4f( m_target->GetParent()->GetTransformationMatrix().const_ptr() ).const_ptr() );
#else
		m_gl.Light( m_eIndex, eGL_LIGHT_COMPONENT_SPOT_DIRECTION, m_target->GetParent()->GetTransformationMatrix().const_ptr() );
#endif
	}
}

void GlLightRenderer::ApplyConstantAtt( float p_constant )
{
	if ( m_eIndex != eGL_LIGHT_INDEX_INVALID )
	{
		m_gl.Light( m_eIndex, eGL_LIGHT_COMPONENT_CONSTANT_ATTENUATION, p_constant );
	}
}

void GlLightRenderer::ApplyLinearAtt( float p_linear )
{
	if ( m_eIndex != eGL_LIGHT_INDEX_INVALID )
	{
		m_gl.Light( m_eIndex, eGL_LIGHT_COMPONENT_LINEAR_ATTENUATION, p_linear );
	}
}

void GlLightRenderer::ApplyQuadraticAtt( float p_quadratic )
{
	if ( m_eIndex != eGL_LIGHT_INDEX_INVALID )
	{
		m_gl.Light( m_eIndex, eGL_LIGHT_COMPONENT_QUADRATIC_ATTENUATION, p_quadratic );
	}
}

void GlLightRenderer::ApplyExponent( float p_exponent )
{
	if ( m_eIndex != eGL_LIGHT_INDEX_INVALID )
	{
		m_gl.Light( m_eIndex, eGL_LIGHT_COMPONENT_SPOT_EXPONENT, p_exponent );
	}
}

void GlLightRenderer::ApplyCutOff( float p_cutOff )
{
	if ( m_eIndex != eGL_LIGHT_INDEX_INVALID )
	{
		m_gl.Light( m_eIndex, eGL_LIGHT_COMPONENT_SPOT_CUTOFF, p_cutOff );
	}
}
