#include "PrecompiledHeader.h"

#include "GLMaterialRenderer.h"
#include "GLSubmeshRenderer.h"
#include "GLBuffer.h"
#include "Module_GL.h"
#include "GLRenderSystem.h"
#include "GLSLProgram.h"

using namespace Castor3D;

GLPassRenderer :: GLPassRenderer()
{
}

void GLPassRenderer :: Cleanup()
{
}

void GLPassRenderer :: Initialise()
{
}

void GLPassRenderer :: Apply( eDRAW_TYPE p_displayMode)
{
	m_varAmbient = m_target->GetAmbient();
	m_varDiffuse = m_target->GetDiffuse();
	m_varSpecular = m_target->GetSpecular();
	m_varEmissive = m_target->GetEmissive();
	m_varShininess = m_target->GetShininess();

	m_pUniformBuffer = GLShaderProgramPtr( m_target->GetShader())->GetUniformBuffer( "in_Material");//static_cast <GLRenderSystem *>( GLRenderSystem::GetSingletonPtr())->GetCurrentShaderProgram()->GetUniformBuffer( "in_Material");

	if ( ! m_pUniformBuffer.null() && m_pUniformBuffer->m_uiIndex != GL_INVALID_INDEX)
	{
		m_pUniformBuffer->SetVariableValue( "in_MatAmbient", m_varAmbient.const_ptr());
		m_pUniformBuffer->SetVariableValue( "in_MatDiffuse", m_varDiffuse.const_ptr());
		m_pUniformBuffer->SetVariableValue( "in_MatEmissive", m_varEmissive.const_ptr());
		m_pUniformBuffer->SetVariableValue( "in_MatSpecular", m_varSpecular.const_ptr());
		m_pUniformBuffer->SetVariableValue( "in_MatShininess", & m_varShininess);
	}
}

void GLPassRenderer :: Apply2D( eDRAW_TYPE p_displayMode)
{
}

void GLPassRenderer :: Remove()
{
}