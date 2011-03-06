#include "Gl2RenderSystem/PrecompiledHeader.h"

#include "Gl2RenderSystem/Gl2MaterialRenderer.h"
#include "Gl2RenderSystem/Gl2SubmeshRenderer.h"
#include "Gl2RenderSystem/Gl2Buffer.h"
#include "Gl2RenderSystem/Gl2RenderSystem.h"
#include "Gl2RenderSystem/Gl2ShaderProgram.h"

using namespace Castor3D;

Gl2PassRenderer :: Gl2PassRenderer( SceneManager * p_pSceneManager)
	:	GlPassRenderer( p_pSceneManager)
{
	m_pfnApply = PApplyFunc( & Gl2PassRenderer::_apply);
	m_pfnApply2D = PVoidFunc( & Gl2PassRenderer::_apply2D);
	m_pfnRemove = PVoidFunc( & Gl2PassRenderer::_remove);
}

void Gl2PassRenderer :: Cleanup()
{
}

void Gl2PassRenderer :: Initialise()
{
	if (RenderSystem::UseShaders() && m_target != NULL && m_target->HasShader())
	{
		m_pfnApply = PApplyFunc( & Gl2PassRenderer::_apply);
		m_pfnApply2D = PVoidFunc( & Gl2PassRenderer::_apply2D);
		m_pfnRemove = PVoidFunc( & Gl2PassRenderer::_remove);
	}
	else
	{
		GlPassRenderer::Initialise();
	}
}

void Gl2PassRenderer :: _apply( ePRIMITIVE_TYPE p_displayMode)
{
	Gl2ShaderProgramPtr l_pProgram = m_target->GetShader<Gl2ShaderProgram>();
	l_pProgram->SetMaterialAmbient(		m_target->GetAmbient());
	l_pProgram->SetMaterialDiffuse(		m_target->GetDiffuse());
	l_pProgram->SetMaterialSpecular(	m_target->GetSpecular());
	l_pProgram->SetMaterialEmissive(	m_target->GetEmissive());
	l_pProgram->SetMaterialShininess(	m_target->GetShininess());

	if ( ! m_target->IsDoubleFace())
	{
		CheckGlError( glCullFace( GL_FRONT), CU_T( "GlPassRenderer :: Apply - glCullFace( GL_FRONT)"));
		CheckGlError( glEnable( GL_CULL_FACE), CU_T( "GlPassRenderer :: Apply - glEnable( GL_CULL_FACE)"));
	}

	if (m_target->GetSrcBlendFactor() != Pass::eSrcBlendNone && m_target->GetDstBlendFactor() != Pass::eDstBlendNone)
	{
		CheckGlError( glEnable( GL_BLEND), CU_T( "GlPassRenderer :: Apply - glEnable( GL_BLEND)"));
		CheckGlError( glBlendFunc( GlEnum::Get( m_target->GetSrcBlendFactor()), GlEnum::Get( m_target->GetDstBlendFactor())), CU_T( "GlPassRenderer :: Apply - glBlendFunc"));
	}
}

void Gl2PassRenderer :: _apply2D()
{
	Gl2ShaderProgramPtr l_pProgram = m_target->GetShader<Gl2ShaderProgram>();
	l_pProgram->SetMaterialAmbient(		m_target->GetAmbient());
	l_pProgram->SetMaterialDiffuse(		m_target->GetDiffuse());
	l_pProgram->SetMaterialSpecular(	m_target->GetSpecular());
	l_pProgram->SetMaterialEmissive(	m_target->GetEmissive());
	l_pProgram->SetMaterialShininess(	m_target->GetShininess());

	if (m_target->GetSrcBlendFactor() != Pass::eSrcBlendNone && m_target->GetDstBlendFactor() != Pass::eDstBlendNone)
	{
		CheckGlError( glEnable( GL_BLEND), CU_T( "GlPassRenderer :: Apply - glEnable( GL_BLEND)"));
		CheckGlError( glBlendFunc( GlEnum::Get( m_target->GetSrcBlendFactor()), GlEnum::Get( m_target->GetDstBlendFactor())), CU_T( "GlPassRenderer :: Apply - glBlendFunc"));
	}
}

void Gl2PassRenderer :: _remove()
{
	CheckGlError( glDisable( GL_CULL_FACE), CU_T( "GL3PassRenderer :: Remove - glDisable( GL_CULL_FACE)"));
}