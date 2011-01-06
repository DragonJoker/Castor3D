#include "GL2RenderSystem/PrecompiledHeader.h"

#include "GL2RenderSystem/GL2MaterialRenderer.h"
#include "GL2RenderSystem/GL2SubmeshRenderer.h"
#include "GL2RenderSystem/GL2Buffer.h"
#include "GL2RenderSystem/GL2RenderSystem.h"
#include "GL2RenderSystem/GL2ShaderProgram.h"

using namespace Castor3D;

GL2PassRenderer :: GL2PassRenderer( SceneManager * p_pSceneManager)
	:	GLPassRenderer( p_pSceneManager)
{
	m_pfnApply = PApplyFunc( & GL2PassRenderer::_apply);
	m_pfnApply2D = PVoidFunc( & GL2PassRenderer::_apply2D);
	m_pfnRemove = PVoidFunc( & GL2PassRenderer::_remove);
}

void GL2PassRenderer :: Cleanup()
{
}

void GL2PassRenderer :: Initialise()
{
	if (RenderSystem::UseShaders() && m_target != NULL && m_target->HasShader())
	{
		m_pfnApply = PApplyFunc( & GL2PassRenderer::_apply);
		m_pfnApply2D = PVoidFunc( & GL2PassRenderer::_apply2D);
		m_pfnRemove = PVoidFunc( & GL2PassRenderer::_remove);
	}
	else
	{
		GLPassRenderer::Initialise();
	}
}

void GL2PassRenderer :: _apply( eDRAW_TYPE p_displayMode)
{
	GL2ShaderProgramPtr l_pProgram = m_target->GetShader<GL2ShaderProgram>();
	l_pProgram->SetMaterialAmbient(		m_target->GetAmbient());
	l_pProgram->SetMaterialDiffuse(		m_target->GetDiffuse());
	l_pProgram->SetMaterialSpecular(	m_target->GetSpecular());
	l_pProgram->SetMaterialEmissive(	m_target->GetEmissive());
	l_pProgram->SetMaterialShininess(	m_target->GetShininess());

	if ( ! m_target->IsDoubleFace())
	{
		glCullFace( GL_FRONT);
		CheckGLError( CU_T( "GLPassRenderer :: Apply - glCullFace( GL_FRONT)"));
		glEnable( GL_CULL_FACE);
		CheckGLError( CU_T( "GLPassRenderer :: Apply - glEnable( GL_CULL_FACE)"));
	}
}

void GL2PassRenderer :: _apply2D()
{
	GL2ShaderProgramPtr l_pProgram = m_target->GetShader<GL2ShaderProgram>();
	l_pProgram->SetMaterialAmbient(		m_target->GetAmbient());
	l_pProgram->SetMaterialDiffuse(		m_target->GetDiffuse());
	l_pProgram->SetMaterialSpecular(	m_target->GetSpecular());
	l_pProgram->SetMaterialEmissive(	m_target->GetEmissive());
	l_pProgram->SetMaterialShininess(	m_target->GetShininess());
}

void GL2PassRenderer :: _remove()
{
	glDisable( GL_CULL_FACE);
	CheckGLError( CU_T( "GL3PassRenderer :: Remove - glDisable( GL_CULL_FACE)"));
}