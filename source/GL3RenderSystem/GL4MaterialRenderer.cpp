#include "Gl3RenderSystem/PrecompiledHeader.h"

#include "Gl3RenderSystem/Gl4MaterialRenderer.h"
#include "Gl3RenderSystem/Gl3SubmeshRenderer.h"
#include "Gl3RenderSystem/Gl3Buffer.h"
#include "Gl3RenderSystem/Gl3RenderSystem.h"
#include "Gl3RenderSystem/Gl4ShaderProgram.h"

using namespace Castor3D;

Gl4PassRenderer :: Gl4PassRenderer( SceneManager * p_pSceneManager)
	:	PassRenderer( p_pSceneManager)
{
}

Gl4PassRenderer :: ~Gl4PassRenderer()
{
	Cleanup();
}

void Gl4PassRenderer :: Cleanup()
{
}

void Gl4PassRenderer :: Initialise()
{
}

void Gl4PassRenderer :: Render( ePRIMITIVE_TYPE p_displayMode)
{
	m_bChanged = m_varAmbient != m_target->GetAmbient()
					|| m_varDiffuse != m_target->GetDiffuse()
					|| m_varSpecular != m_target->GetSpecular()
					|| m_varEmissive != m_target->GetEmissive()
					|| m_varShininess != m_target->GetShininess();

	m_varAmbient	= m_target->GetAmbient();
	m_varDiffuse	= m_target->GetDiffuse();
	m_varSpecular	= m_target->GetSpecular();
	m_varEmissive	= m_target->GetEmissive();
	m_varShininess	= m_target->GetShininess();

	GlUniformBufferObjectPtr l_pUniformBuffer = m_target->GetShader<Gl4ShaderProgram>()->GetUniformBuffer( "in_Material");

	if (l_pUniformBuffer != m_pUniformBuffer)
	{
		m_pUniformBuffer = l_pUniformBuffer;
		m_pAmbient		= m_pUniformBuffer->GetVariable<GlUboPoint4fVariable>(	"in_MatAmbient");
		m_pDiffuse		= m_pUniformBuffer->GetVariable<GlUboPoint4fVariable>(	"in_MatDiffuse");
		m_pSpecular		= m_pUniformBuffer->GetVariable<GlUboPoint4fVariable>(	"in_MatEmissive");
		m_pEmissive		= m_pUniformBuffer->GetVariable<GlUboPoint4fVariable>(	"in_MatSpecular");
		m_pShininess	= m_pUniformBuffer->GetVariable<GlUboFloatVariable>(	"in_MatShininess");
	}

	if (m_pUniformBuffer != NULL && m_pUniformBuffer->m_iUniformBlockIndex != GL_INVALID_INDEX && m_pUniformBuffer->m_uiIndex != GL_INVALID_INDEX && m_bChanged)
	{
		if (m_pAmbient != NULL)
		{
			m_pAmbient->SetValue(	m_varAmbient,	0);
		}
		if (m_pDiffuse != NULL)
		{
			m_pDiffuse->SetValue(	m_varDiffuse,	0);
		}
		if (m_pSpecular != NULL)
		{
			m_pSpecular->SetValue(	m_varEmissive,	0);
		}
		if (m_pEmissive != NULL)
		{
			m_pEmissive->SetValue(	m_varSpecular,	0);
		}
		if (m_pShininess != NULL)
		{
			m_pShininess->SetValue(	m_varShininess,	0);
		}
	}
}

void Gl4PassRenderer :: Render2D()
{
}

void Gl4PassRenderer :: EndRender()
{
}
