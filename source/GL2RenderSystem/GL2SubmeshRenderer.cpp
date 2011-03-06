#include "Gl2RenderSystem/PrecompiledHeader.h"

#include "Gl2RenderSystem/Gl2SubmeshRenderer.h"
#include "Gl2RenderSystem/Gl2RenderSystem.h"
#include "Gl2RenderSystem/Gl2Buffer.h"

using namespace Castor3D;

Gl2SubmeshRenderer :: Gl2SubmeshRenderer( SceneManager * p_pSceneManager)
	:	SubmeshRenderer( p_pSceneManager)
{
	_createVbo();
}

Gl2SubmeshRenderer :: ~Gl2SubmeshRenderer()
{
}

void Gl2SubmeshRenderer :: Cleanup()
{
	SubmeshRenderer::Cleanup();
	_createVbo();
}

void Gl2SubmeshRenderer :: Initialise()
{
	if (Gl2RenderSystem::GetSingletonPtr()->UseVertexBufferObjects())
	{
		m_vertex->Initialise( eBufferStatic);

		for (int i = 0 ; i < eNbDrawTypes ; i++)
		{
			m_indices[i]->Initialise( eBufferStatic);
		}
	}
}
/*
VertexBufferPtr Gl2SubmeshRenderer :: CreateVertexBuffer()
{
	VertexBufferPtr l_pReturn = BufferManager::CreateBuffer<real, GlVertexBuffer>();
	return l_pReturn;
}
*/
void Gl2SubmeshRenderer :: _drawBuffers( const Pass & p_pass)
{
	GlShaderProgramPtr l_pProgram = p_pass.GetShader<GlShaderProgram>();
	bool l_bUseShader = (l_pProgram != NULL) && l_pProgram->IsLinked();

	if (l_bUseShader)
	{
		static_pointer_cast<GlVboVertexBuffer>( m_vertex)->SetShaderProgram( l_pProgram);
		Pipeline::ApplyMatrices();
	}

	GLenum l_eMode = GlEnum::Get( m_eLastDrawType);

	m_vertex->Activate();
	m_indices[m_eLastDrawType]->Activate();
	CheckGlError( glDrawElements( l_eMode, m_indices[m_eLastDrawType]->GetFilled(), GL_UNSIGNED_INT, 0), CU_T( "GlPassRenderer :: Draw - glDrawArrays"));
	m_indices[m_eLastDrawType]->Deactivate();
	m_vertex->Deactivate();
}
