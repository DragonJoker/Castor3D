#include "GL2RenderSystem/PrecompiledHeader.h"

#include "GL2RenderSystem/GL2SubmeshRenderer.h"
#include "GL2RenderSystem/GL2RenderSystem.h"
#include "GL2RenderSystem/GL2Buffer.h"

using namespace Castor3D;

GL2SubmeshRenderer :: GL2SubmeshRenderer( SceneManager * p_pSceneManager)
	:	SubmeshRenderer( p_pSceneManager)
{
	_createVBOs();
}

GL2SubmeshRenderer :: ~GL2SubmeshRenderer()
{
}

void GL2SubmeshRenderer :: Cleanup()
{
	SubmeshRenderer::Cleanup();
	_createVBOs();
}

void GL2SubmeshRenderer :: Initialise()
{
	if (GL2RenderSystem::GetSingletonPtr()->UseVBO())
	{
		m_vertex->Initialise();

		for (int i = 0 ; i < eNbDrawType ; i++)
		{
			m_indices[i]->Initialise();
		}
	}
}

VertexInfosBufferPtr GL2SubmeshRenderer :: CreateVertexBuffer()
{
	VertexInfosBufferPtr l_pReturn = BufferManager::CreateBuffer<real, GLVertexInfosBufferObject>();
	return l_pReturn;
}

void GL2SubmeshRenderer :: _drawBuffers( const Pass & p_pass)
{
	GLShaderProgramPtr l_pProgram = p_pass.GetShader<GLShaderProgram>();
	bool l_bUseShader = ( ! l_pProgram == NULL) && l_pProgram->IsLinked();

	if (l_bUseShader)
	{
		static_pointer_cast<GLVertexInfosBufferObject>( m_vertex)->SetShaderProgram( l_pProgram);
		Pipeline::ApplyMatrices();
	}

	GLenum l_eMode = 0;

	switch (m_eLastDrawType)
	{
	case eTriangles:
		l_eMode = GL_TRIANGLES;
		break;

	case eLines:
		l_eMode = GL_LINES;
		break;

	case ePoints:
		l_eMode = GL_POINTS;
		break;

	default:
		break;
	}

	m_vertex->Activate();
	m_indices[m_eLastDrawType]->Activate();
	glDrawElements( l_eMode, m_indices[m_eLastDrawType]->GetFilled(), GL_UNSIGNED_INT, 0);
	CheckGLError( CU_T( "GLPassRenderer :: Draw - glDrawArrays"));
	m_indices[m_eLastDrawType]->Deactivate();
	m_vertex->Deactivate();
}