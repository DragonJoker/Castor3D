#include "GlRenderSystem/PrecompiledHeader.hpp"

#include "GlRenderSystem/GlMaterialRenderer.hpp"
#include "GlRenderSystem/GlSubmeshRenderer.hpp"
#include "GlRenderSystem/GlBuffer.hpp"
#include "GlRenderSystem/GlRenderSystem.hpp"
#include "GlRenderSystem/GlShaderProgram.hpp"
#include "GlRenderSystem/OpenGl.hpp"

using namespace GlRender;
using namespace Castor3D;
using namespace Castor;

GlPassRenderer :: GlPassRenderer( OpenGl & p_gl, GlRenderSystem * p_pRenderSystem )
	:	PassRenderer	( p_pRenderSystem	)
	,	m_gl			( p_gl				)
{
}

void GlPassRenderer :: Cleanup()
{
}

void GlPassRenderer :: Initialise()
{
	if( m_pRenderSystem->ForceShaders() || (m_pRenderSystem->UseShaders() && m_target && m_target->HasShader() && m_target->GetShader< GlShaderProgram >()->GetStatus() == ePROGRAM_STATUS_LINKED) )
	{
		m_pfnApply		= ApplyFunc( [&](){} );
		m_pfnApply2D	= ApplyFunc( [&](){} );
	}
	else
	{
		m_pfnApply		= ApplyFunc( [&]()
		{
			m_gl.Material(	eGL_DRAW_BUFFER_MODE_FRONT_AND_BACK, eGL_MATERIAL_COMPONENT_AMBIENT,	m_target->GetAmbient().const_ptr()	);
			m_gl.Material(	eGL_DRAW_BUFFER_MODE_FRONT_AND_BACK, eGL_MATERIAL_COMPONENT_DIFFUSE,	m_target->GetDiffuse().const_ptr()	);
			m_gl.Material(	eGL_DRAW_BUFFER_MODE_FRONT_AND_BACK, eGL_MATERIAL_COMPONENT_SPECULAR,	m_target->GetSpecular().const_ptr()	);
			m_gl.Material(	eGL_DRAW_BUFFER_MODE_FRONT_AND_BACK, eGL_MATERIAL_COMPONENT_EMISSION,	m_target->GetEmissive().const_ptr()	);
			m_gl.Material(	eGL_DRAW_BUFFER_MODE_FRONT_AND_BACK, eGL_MATERIAL_COMPONENT_SHININESS,	m_target->GetShininess()			);
		} );
		m_pfnApply2D	= ApplyFunc( [&]()
		{
			m_gl.Material(	eGL_DRAW_BUFFER_MODE_FRONT_AND_BACK, eGL_MATERIAL_COMPONENT_AMBIENT,	m_target->GetAmbient().const_ptr()	);
			m_gl.Material(	eGL_DRAW_BUFFER_MODE_FRONT_AND_BACK, eGL_MATERIAL_COMPONENT_DIFFUSE,	m_target->GetDiffuse().const_ptr()	);
			m_gl.Material(	eGL_DRAW_BUFFER_MODE_FRONT_AND_BACK, eGL_MATERIAL_COMPONENT_SPECULAR,	m_target->GetSpecular().const_ptr()	);
			m_gl.Material(	eGL_DRAW_BUFFER_MODE_FRONT_AND_BACK, eGL_MATERIAL_COMPONENT_EMISSION,	m_target->GetEmissive().const_ptr()	);
			m_gl.Material(	eGL_DRAW_BUFFER_MODE_FRONT_AND_BACK, eGL_MATERIAL_COMPONENT_SHININESS,	m_target->GetShininess()			);
		} );
	}

}

void GlPassRenderer :: Render()
{
	PassRenderer::Render();
	m_pfnApply();
}

void GlPassRenderer :: Render2D()
{
	PassRenderer::Render2D();
	m_pfnApply2D();
}

void GlPassRenderer :: EndRender()
{
}
