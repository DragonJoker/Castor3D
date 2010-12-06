#include "PrecompiledHeader.h"

#include "GL3MaterialRenderer.h"
#include "GL3SubmeshRenderer.h"
#include "GL3Buffer.h"
#include "GL3RenderSystem.h"
#include "GL3ShaderProgram.h"

using namespace Castor3D;

GL3PassRenderer :: GL3PassRenderer()
{
	m_pfnApply = PApplyFunc( & GL3PassRenderer::_apply);
	m_pfnApply2D = PApplyFunc( & GL3PassRenderer::_apply2D);
	m_pfnRemove = PRemoveFunc( & GL3PassRenderer::_remove);
}

void GL3PassRenderer :: Cleanup()
{
}

void GL3PassRenderer :: Initialise()
{
	m_pfnApply = PApplyFunc( & GL3PassRenderer::_apply);
	m_pfnApply2D = PApplyFunc( & GL3PassRenderer::_apply2D);
	m_pfnRemove = PRemoveFunc( & GL3PassRenderer::_remove);
}

void GL3PassRenderer :: _apply( eDRAW_TYPE p_displayMode)
{
	if ( ! m_target->GetShader().null())
	{
		GL3ShaderProgramPtr l_pProgram = static_pointer_cast<GL3ShaderProgram>( m_target->GetShader());
		l_pProgram->SetMaterialAmbient( m_target->GetAmbient());
		l_pProgram->SetMaterialDiffuse( m_target->GetDiffuse());
		l_pProgram->SetMaterialSpecular( m_target->GetSpecular());
		l_pProgram->SetMaterialEmissive( m_target->GetEmissive());
		l_pProgram->SetMaterialShininess( m_target->GetShininess());
	}
}

void GL3PassRenderer :: _apply2D( eDRAW_TYPE p_displayMode)
{
	if ( ! m_target->GetShader().null())
	{
		GL3ShaderProgramPtr l_pProgram = static_pointer_cast<GL3ShaderProgram>( m_target->GetShader());
		l_pProgram->SetMaterialAmbient( m_target->GetAmbient());
		l_pProgram->SetMaterialDiffuse( m_target->GetDiffuse());
		l_pProgram->SetMaterialSpecular( m_target->GetSpecular());
		l_pProgram->SetMaterialEmissive( m_target->GetEmissive());
		l_pProgram->SetMaterialShininess( m_target->GetShininess());
	}
}

void GL3PassRenderer :: _remove()
{
}