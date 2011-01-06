#include "GL3RenderSystem/PrecompiledHeader.h"

#include "GL3RenderSystem/GL3MaterialRenderer.h"
#include "GL3RenderSystem/GL3SubmeshRenderer.h"
#include "GL3RenderSystem/GL3Buffer.h"
#include "GL3RenderSystem/GL3RenderSystem.h"
#include "GL3RenderSystem/GL3ShaderProgram.h"

using namespace Castor3D;

GL3PassRenderer :: GL3PassRenderer( SceneManager * p_pSceneManager)
	:	GLPassRenderer( p_pSceneManager)
{
	m_pfnApply = PApplyFunc( & GL3PassRenderer::_apply);
	m_pfnApply2D = PVoidFunc( & GL3PassRenderer::_apply2D);
	m_pfnRemove = PVoidFunc( & GL3PassRenderer::_remove);
}

GL3PassRenderer :: ~GL3PassRenderer()
{
	Cleanup();
}

void GL3PassRenderer :: Cleanup()
{
}

void GL3PassRenderer :: Initialise()
{
	m_pfnApply = PApplyFunc( & GL3PassRenderer::_apply);
	m_pfnApply2D = PVoidFunc( & GL3PassRenderer::_apply2D);
	m_pfnRemove = PVoidFunc( & GL3PassRenderer::_remove);
}

void GL3PassRenderer :: _apply( eDRAW_TYPE p_displayMode)
{
	GL3ShaderProgramPtr l_pProgram = m_target->GetShader<GL3ShaderProgram>();

	if ( ! l_pProgram == NULL)
	{
		l_pProgram->SetMaterialAmbient( m_target->GetAmbient());
		l_pProgram->SetMaterialDiffuse( m_target->GetDiffuse());
		l_pProgram->SetMaterialSpecular( m_target->GetSpecular());
		l_pProgram->SetMaterialEmissive( m_target->GetEmissive());
		l_pProgram->SetMaterialShininess( m_target->GetShininess());
	}

	if ( ! m_target->IsDoubleFace())
	{
		glCullFace( GL_FRONT);
		CheckGLError( CU_T( "GLPassRenderer :: Apply - glCullFace( GL_FRONT)"));
		glEnable( GL_CULL_FACE);
		CheckGLError( CU_T( "GLPassRenderer :: Apply - glEnable( GL_CULL_FACE)"));
	}
}

void GL3PassRenderer :: _apply2D()
{
	GL3ShaderProgramPtr l_pProgram = m_target->GetShader<GL3ShaderProgram>();

	if ( ! l_pProgram == NULL)
	{
		l_pProgram->SetMaterialAmbient( m_target->GetAmbient());
		l_pProgram->SetMaterialDiffuse( m_target->GetDiffuse());
		l_pProgram->SetMaterialSpecular( m_target->GetSpecular());
		l_pProgram->SetMaterialEmissive( m_target->GetEmissive());
		l_pProgram->SetMaterialShininess( m_target->GetShininess());
	}
}

void GL3PassRenderer :: _remove()
{
	glDisable( GL_CULL_FACE);
	CheckGLError( CU_T( "GL3PassRenderer :: Remove - glDisable( GL_CULL_FACE)"));
}