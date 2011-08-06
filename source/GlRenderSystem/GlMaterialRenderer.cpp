#include "GlRenderSystem/PrecompiledHeader.hpp"

#include "GlRenderSystem/GlMaterialRenderer.hpp"
#include "GlRenderSystem/GlSubmeshRenderer.hpp"
#include "GlRenderSystem/GlBuffer.hpp"
#include "GlRenderSystem/GlRenderSystem.hpp"
#include "GlRenderSystem/GlShaderProgram.hpp"
#include "GlRenderSystem/CgGlShaderProgram.hpp"
#include "GlRenderSystem/OpenGl.hpp"

using namespace Castor3D;

GlPassRenderer :: GlPassRenderer()
	:	PassRenderer()
{
	m_pfnApply = & GlPassRenderer::_applyGlsl;
	m_pfnApply2D = & GlPassRenderer::_apply2DGlsl;
	m_pfnRemove = & GlPassRenderer::_removeGlsl;
}

void GlPassRenderer :: Cleanup()
{
	CASTOR_TRACK;
}

void GlPassRenderer :: Initialise()
{
	CASTOR_TRACK;
	if (GlRenderSystem::GetOpenGlMajor() < 3)
	{
		if (RenderSystem::UseShaders() && m_target && m_target->HasShader())
		{
			m_pfnApply = PApplyFunc( & GlPassRenderer::_applyGlsl);
			m_pfnApply2D = PVoidFunc( & GlPassRenderer::_apply2DGlsl);
			m_pfnRemove = PVoidFunc( & GlPassRenderer::_removeGlsl);
		}
		else
		{
			m_pfnApply = & GlPassRenderer::_apply;
			m_pfnApply2D = & GlPassRenderer::_apply2D;
			m_pfnRemove = & GlPassRenderer::_remove;
		}
	}
#if CASTOR_USE_OPENGL4
	else if (GlRenderSystem::GetOpenGlMajor() < 4)
#endif
	else
	{
		m_pfnApply = PApplyFunc( & GlPassRenderer::_applyGlsl);
		m_pfnApply2D = PVoidFunc( & GlPassRenderer::_apply2DGlsl);
		m_pfnRemove = PVoidFunc( & GlPassRenderer::_removeGlsl);
	}
#if CASTOR_USE_OPENGL4
	else
	{
	}
#endif
}

void GlPassRenderer :: Render( ePRIMITIVE_TYPE p_displayMode)
{
	CASTOR_TRACK;
	(this->*m_pfnApply)( p_displayMode);
}

void GlPassRenderer :: Render2D()
{
	CASTOR_TRACK;
	(this->*m_pfnApply2D)();
}

void GlPassRenderer :: EndRender()
{
	CASTOR_TRACK;
	(this->*m_pfnRemove)();
}

void GlPassRenderer :: _apply( ePRIMITIVE_TYPE p_displayMode)
{
	CASTOR_TRACK;
	bool l_bReturn = true;
	l_bReturn &= OpenGl::Materialfv( GL_FRONT_AND_BACK, GL_AMBIENT, m_target->GetAmbient().const_ptr());
	l_bReturn &= OpenGl::Materialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, m_target->GetDiffuse().const_ptr());
	l_bReturn &= OpenGl::Materialfv( GL_FRONT_AND_BACK, GL_SPECULAR, m_target->GetSpecular().const_ptr());
	l_bReturn &= OpenGl::Materialfv( GL_FRONT_AND_BACK, GL_EMISSION, m_target->GetEmissive().const_ptr());
	l_bReturn &= OpenGl::Materialf( GL_FRONT_AND_BACK, GL_SHININESS, m_target->GetShininess());

	if ( ! m_target->IsDoubleFace())
	{
		l_bReturn &= OpenGl::CullFace( GL_FRONT);
		l_bReturn &= OpenGl::FrontFace( GL_CCW);
		l_bReturn &= OpenGl::Enable( GL_CULL_FACE);
	}

	if (m_target->GetSrcBlendFactor() != Pass::eSRC_BLEND_NONE && m_target->GetDstBlendFactor() != Pass::eDST_BLEND_NONE)
	{
		l_bReturn &= OpenGl::Enable( GL_BLEND);
 		l_bReturn &= OpenGl::Disable( GL_DEPTH_TEST);
		l_bReturn &= OpenGl::BlendFunc( GlEnum::Get( m_target->GetSrcBlendFactor()), GlEnum::Get( m_target->GetDstBlendFactor()));
	}
}

void GlPassRenderer :: _apply2D()
{
	CASTOR_TRACK;
	bool l_bReturn = true;
	if (m_target->GetSrcBlendFactor() != Pass::eSRC_BLEND_NONE && m_target->GetDstBlendFactor() != Pass::eDST_BLEND_NONE)
	{
		l_bReturn &= OpenGl::Enable( GL_BLEND);
 		l_bReturn &= OpenGl::Disable( GL_DEPTH_TEST);
		l_bReturn &= OpenGl::BlendFunc( GlEnum::Get( m_target->GetSrcBlendFactor()), GlEnum::Get( m_target->GetDstBlendFactor()));
	}

	l_bReturn &= OpenGl::Color4fv( m_target->GetAmbient().const_ptr());
}

void GlPassRenderer :: _remove()
{
	CASTOR_TRACK;
	if ( ! m_target->IsDoubleFace())
	{
		OpenGl::Disable( GL_CULL_FACE);
	}

	if (m_target->GetSrcBlendFactor() != Pass::eSRC_BLEND_NONE && m_target->GetDstBlendFactor() != Pass::eDST_BLEND_NONE)
	{
		OpenGl::Disable( GL_BLEND);
 		OpenGl::Enable( GL_DEPTH_TEST);
	}
}

void GlPassRenderer :: _applyGlsl( ePRIMITIVE_TYPE p_displayMode)
{
	CASTOR_TRACK;
	ShaderProgramBase * l_pProgram = m_target->GetShader<ShaderProgramBase>().get();
	bool l_bReturn = true;

	if (l_pProgram)
	{
		l_pProgram->SetMaterialAmbient(		m_target->GetAmbient());
		l_pProgram->SetMaterialDiffuse(		m_target->GetDiffuse());
		l_pProgram->SetMaterialSpecular(	m_target->GetSpecular());
		l_pProgram->SetMaterialEmissive(	m_target->GetEmissive());
		l_pProgram->SetMaterialShininess(	m_target->GetShininess());
	}

	if ( ! m_target->IsDoubleFace())
	{
		l_bReturn &= OpenGl::CullFace( GL_FRONT);
		l_bReturn &= OpenGl::Enable( GL_CULL_FACE);
	}

	if (m_target->GetSrcBlendFactor() != Pass::eSRC_BLEND_NONE && m_target->GetDstBlendFactor() != Pass::eDST_BLEND_NONE)
	{
		l_bReturn &= OpenGl::Enable( GL_BLEND);
		l_bReturn &= OpenGl::BlendFunc( GlEnum::Get( m_target->GetSrcBlendFactor()), GlEnum::Get( m_target->GetDstBlendFactor()));
	}
}

void GlPassRenderer :: _apply2DGlsl()
{
	CASTOR_TRACK;
	ShaderProgramBase * l_pProgram = m_target->GetShader<ShaderProgramBase>().get();
	bool l_bReturn = true;

	if (l_pProgram)
	{
		l_pProgram->SetMaterialAmbient(		m_target->GetAmbient());
		l_pProgram->SetMaterialDiffuse(		m_target->GetDiffuse());
		l_pProgram->SetMaterialSpecular(	m_target->GetSpecular());
		l_pProgram->SetMaterialEmissive(	m_target->GetEmissive());
		l_pProgram->SetMaterialShininess(	m_target->GetShininess());

		if (m_target->GetSrcBlendFactor() != Pass::eSRC_BLEND_NONE && m_target->GetDstBlendFactor() != Pass::eDST_BLEND_NONE)
		{
			l_bReturn &= OpenGl::Enable( GL_BLEND);
			l_bReturn &= OpenGl::BlendFunc( GlEnum::Get( m_target->GetSrcBlendFactor()), GlEnum::Get( m_target->GetDstBlendFactor()));
		}
	}
}

void GlPassRenderer :: _removeGlsl()
{
	CASTOR_TRACK;
	if ( ! m_target->IsDoubleFace())
	{
		OpenGl::Disable( GL_CULL_FACE);
	}

	if (m_target->GetSrcBlendFactor() != Pass::eSRC_BLEND_NONE && m_target->GetDstBlendFactor() != Pass::eDST_BLEND_NONE)
	{
		OpenGl::Disable( GL_BLEND);
		OpenGl::Enable( GL_DEPTH_TEST);
	}
}
#if CASTOR_USE_OPENGL4
void GlPassRenderer :: _applyGl4( ePRIMITIVE_TYPE p_displayMode)
{
	CASTOR_TRACK;
	m_bChanged =	m_ptAmbient != m_target->GetAmbient()
				||	m_ptDiffuse != m_target->GetDiffuse()
				||	m_ptSpecular != m_target->GetSpecular()
				||	m_ptEmissive != m_target->GetEmissive()
				||	m_fShininess != m_target->GetShininess();

	m_ptAmbient		= m_target->GetAmbient();
	m_ptDiffuse		= m_target->GetDiffuse();
	m_ptSpecular	= m_target->GetSpecular();
	m_ptEmissive	= m_target->GetEmissive();
	m_fShininess	= m_target->GetShininess();

	GlUniformBufferObjectPtr l_pUniformBuffer = m_target->GetShader<GlShaderProgram>()->GetUniformBuffer( "in_Material");

	if (l_pUniformBuffer != m_pUniformBuffer)
	{
		m_pUniformBuffer = l_pUniformBuffer;
		m_pAmbient		= m_pUniformBuffer->GetVariable<GlUboPoint4fVariable>(	"in_MatAmbient");
		m_pDiffuse		= m_pUniformBuffer->GetVariable<GlUboPoint4fVariable>(	"in_MatDiffuse");
		m_pSpecular		= m_pUniformBuffer->GetVariable<GlUboPoint4fVariable>(	"in_MatEmissive");
		m_pEmissive		= m_pUniformBuffer->GetVariable<GlUboPoint4fVariable>(	"in_MatSpecular");
		m_pShininess	= m_pUniformBuffer->GetVariable<GlUboFloatVariable>(	"in_MatShininess");
	}

	if (m_pUniformBuffer && m_pUniformBuffer->m_iUniformBlockIndex != GL_INVALID_INDEX && m_pUniformBuffer->m_uiIndex != GL_INVALID_INDEX && m_bChanged)
	{
		if (m_pAmbient)
		{
			m_pAmbient->SetValue(	m_ptAmbient,	0);
		}
		if (m_pDiffuse)
		{
			m_pDiffuse->SetValue(	m_ptDiffuse,	0);
		}
		if (m_pSpecular)
		{
			m_pSpecular->SetValue(	m_ptEmissive,	0);
		}
		if (m_pEmissive)
		{
			m_pEmissive->SetValue(	m_ptSpecular,	0);
		}
		if (m_pShininess)
		{
			m_pShininess->SetValue(	m_fShininess,	0);
		}
	}
}

void GlPassRenderer :: _apply2DGl4()
{
	CASTOR_TRACK;
}

void GlPassRenderer :: _removeGl4()
{
	CASTOR_TRACK;
}
#endif
