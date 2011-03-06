#include "Gl3RenderSystem/PrecompiledHeader.h"

#include "Gl3RenderSystem/Gl3SubmeshRenderer.h"
#include "Gl3RenderSystem/Gl3Context.h"
#include "Gl3RenderSystem/Gl3RenderSystem.h"
#include "Gl3RenderSystem/Gl3Buffer.h"
#include "Gl3RenderSystem/Gl3ShaderProgram.h"

using namespace Castor3D;

Gl3SubmeshRenderer :: Gl3SubmeshRenderer( SceneManager * p_pSceneManager)
	:	SubmeshRenderer( p_pSceneManager)
	,	m_eDrawType( ePRIMITIVE_TYPE( -1))
	,	m_eNormalsMode( eNORMALS_MODE( -1))
{
	_createVbo();
}

Gl3SubmeshRenderer :: ~Gl3SubmeshRenderer()
{
}

void Gl3SubmeshRenderer :: Cleanup()
{
	m_vao.Cleanup();
	SubmeshRenderer::Cleanup();
	_createVbo();
}

void Gl3SubmeshRenderer :: Initialise()
{
	m_vertex->Initialise( eBufferStatic);

	for (int i = 0 ; i < eNbDrawTypes ; i++)
	{
		m_indices[i]->Initialise( eBufferStatic);
	}

	if (m_target->GetMaterial() == NULL)
	{
		m_target->SetMaterial( m_pSceneManager->GetMaterialManager()->GetDefaultMaterial());
	}
}
/*
VertexBufferPtr Gl3SubmeshRenderer :: CreateVertexBuffer()
{
	VertexInfosBufferPtr l_pReturn = Gl3RenderSystem::CreateBuffer<VertexInfosBuffer>();
	return l_pReturn;
}
*/
void Gl3SubmeshRenderer :: _drawBuffers( const Pass & p_pass)
{
	Gl3ShaderProgramPtr l_pProgram = p_pass.GetShader<Gl3ShaderProgram>();
	bool l_bUseShader = (l_pProgram != NULL) && glIsProgram( l_pProgram->GetProgramObject()) && l_pProgram->IsLinked();

	if (l_bUseShader)
	{
		Pipeline::ApplyMatrices();

		int l_iFragColourIndex = glGetFragDataLocation( l_pProgram->GetProgramObject(), "out_FragColor");

		GLenum l_eMode = GlEnum::Get( m_eLastDrawType);
/*
		_createVAOs( l_pProgram);

		if (m_vao.GetIndex() != GL_INVALID_INDEX)
		{
			m_vao.Activate();

			glDrawElements( l_eMode, m_indices->GetFilled(), GL_UNSIGNED_INT, 0);
			CheckGlError( CU_T( "Gl3SubmeshRenderer :: Draw - glDrawArrays"));

			m_vao.Deactivate();
		}
/**/
		static_pointer_cast<GlVboVertexBuffer>( m_vertex)->SetShaderProgram( l_pProgram);
		m_vertex->Activate();
		m_indices[m_eLastDrawType]->Activate();
		CheckGlError( glDrawElements( l_eMode, m_indices[m_eLastDrawType]->GetFilled(), GL_UNSIGNED_INT, 0), CU_T( "GlPassRenderer :: Draw - glDrawArrays"));
		m_indices[m_eLastDrawType]->Deactivate();
		m_vertex->Deactivate();
/**/
	}
}

void Gl3SubmeshRenderer :: _createVAOs( Gl3ShaderProgramPtr p_pProgram)
{
	if (m_eDrawType != m_eLastDrawType || m_eNormalsMode != m_eLastNormalsMode || m_vao.GetIndex() == GL_INVALID_INDEX)
	{
		m_eDrawType = m_eLastDrawType;
		m_eNormalsMode = m_eLastNormalsMode;

		static_pointer_cast<GlVboVertexBuffer>( m_vertex)->CleanupVbo();
		static_pointer_cast<GlVboIndexBuffer>( m_indices[m_eDrawType])->CleanupVbo();

		static_pointer_cast<GlVboVertexBuffer>( m_vertex)->SetShaderProgram( p_pProgram);
		m_indices[m_eDrawType]->Initialise( eBufferStatic);

		m_vao.Cleanup();
		m_vao.Initialise();
		m_vao.Activate();
		m_vertex->Activate();
		m_indices[m_eLastDrawType]->Activate();
		m_vao.Deactivate();
	}
}
