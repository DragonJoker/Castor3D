#include "PrecompiledHeader.h"

#include "GL2MaterialRenderer.h"
#include "GL2SubmeshRenderer.h"
#include "GL2Buffer.h"
#include "GL2RenderSystem.h"
#include "GL2ShaderProgram.h"

using namespace Castor3D;

GL2PassRenderer :: GL2PassRenderer()
{
	m_pfnApply = PApplyFunc( & GL2PassRenderer::_apply);
	m_pfnApply2D = PApplyFunc( & GL2PassRenderer::_apply2D);
	m_pfnRemove = PRemoveFunc( & GL2PassRenderer::_remove);
}

void GL2PassRenderer :: Cleanup()
{
}

void GL2PassRenderer :: Initialise()
{
	if (RenderSystem::UseShaders() && m_target != NULL && ! m_target->GetShader().null())
	{
		m_pfnApply = PApplyFunc( & GL2PassRenderer::_apply);
		m_pfnApply2D = PApplyFunc( & GL2PassRenderer::_apply2D);
		m_pfnRemove = PRemoveFunc( & GL2PassRenderer::_remove);
	}
	else
	{
		GLPassRenderer::Initialise();
	}
}

void GL2PassRenderer :: _apply( eDRAW_TYPE p_displayMode)
{
	GL2ShaderProgramPtr l_pProgram = static_pointer_cast<GL2ShaderProgram>( m_target->GetShader());
	l_pProgram->SetMaterialAmbient(		m_target->GetAmbient());
	l_pProgram->SetMaterialDiffuse(		m_target->GetDiffuse());
	l_pProgram->SetMaterialSpecular(	m_target->GetSpecular());
	l_pProgram->SetMaterialEmissive(	m_target->GetEmissive());
	l_pProgram->SetMaterialShininess(	m_target->GetShininess());
}

void GL2PassRenderer :: _apply2D( eDRAW_TYPE p_displayMode)
{
	GL2ShaderProgramPtr l_pProgram = static_pointer_cast<GL2ShaderProgram>( m_target->GetShader());
	l_pProgram->SetMaterialAmbient(		m_target->GetAmbient());
	l_pProgram->SetMaterialDiffuse(		m_target->GetDiffuse());
	l_pProgram->SetMaterialSpecular(	m_target->GetSpecular());
	l_pProgram->SetMaterialEmissive(	m_target->GetEmissive());
	l_pProgram->SetMaterialShininess(	m_target->GetShininess());
}

void GL2PassRenderer :: _remove()
{
}