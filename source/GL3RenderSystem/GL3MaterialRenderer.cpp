#include "Gl3RenderSystem/PrecompiledHeader.h"

#include "Gl3RenderSystem/Gl3MaterialRenderer.h"
#include "Gl3RenderSystem/Gl3SubmeshRenderer.h"
#include "Gl3RenderSystem/Gl3Buffer.h"
#include "Gl3RenderSystem/Gl3RenderSystem.h"
#include "Gl3RenderSystem/Gl3ShaderProgram.h"

using namespace Castor3D;

Gl3PassRenderer :: Gl3PassRenderer( SceneManager * p_pSceneManager)
	:	GlPassRenderer( p_pSceneManager)
{
	m_pfnApply = PApplyFunc( & Gl3PassRenderer::_apply);
	m_pfnApply2D = PVoidFunc( & Gl3PassRenderer::_apply2D);
	m_pfnRemove = PVoidFunc( & Gl3PassRenderer::_remove);
}

Gl3PassRenderer :: ~Gl3PassRenderer()
{
	Cleanup();
}

void Gl3PassRenderer :: Cleanup()
{
}

void Gl3PassRenderer :: Initialise()
{
	m_pfnApply = PApplyFunc( & Gl3PassRenderer::_apply);
	m_pfnApply2D = PVoidFunc( & Gl3PassRenderer::_apply2D);
	m_pfnRemove = PVoidFunc( & Gl3PassRenderer::_remove);
}

void Gl3PassRenderer :: _apply( ePRIMITIVE_TYPE p_displayMode)
{
	Gl3ShaderProgramPtr l_pProgram = m_target->GetShader<Gl3ShaderProgram>();

	if (l_pProgram != NULL)
	{
		l_pProgram->SetMaterialAmbient( m_target->GetAmbient());
		l_pProgram->SetMaterialDiffuse( m_target->GetDiffuse());
		l_pProgram->SetMaterialSpecular( m_target->GetSpecular());
		l_pProgram->SetMaterialEmissive( m_target->GetEmissive());
		l_pProgram->SetMaterialShininess( m_target->GetShininess());
	}

	if ( ! m_target->IsDoubleFace())
	{
		CheckGlError( glCullFace( GL_FRONT), CU_T( "GlPassRenderer :: Apply - glCullFace( GL_FRONT)"));
		CheckGlError( glEnable( GL_CULL_FACE), CU_T( "GlPassRenderer :: Apply - glEnable( GL_CULL_FACE)"));
	}
}

void Gl3PassRenderer :: _apply2D()
{
	Gl3ShaderProgramPtr l_pProgram = m_target->GetShader<Gl3ShaderProgram>();

	if (l_pProgram != NULL)
	{
		l_pProgram->SetMaterialAmbient( m_target->GetAmbient());
		l_pProgram->SetMaterialDiffuse( m_target->GetDiffuse());
		l_pProgram->SetMaterialSpecular( m_target->GetSpecular());
		l_pProgram->SetMaterialEmissive( m_target->GetEmissive());
		l_pProgram->SetMaterialShininess( m_target->GetShininess());
	}
}

void Gl3PassRenderer :: _remove()
{
	CheckGlError( glDisable( GL_CULL_FACE), CU_T( "Gl3PassRenderer :: Remove - glDisable( GL_CULL_FACE)"));
}
