#include "GL3RenderSystem/PrecompiledHeader.h"

#include "GL3RenderSystem/GL3SubmeshRenderer.h"
#include "GL3RenderSystem/GL3Context.h"
#include "GL3RenderSystem/GL3RenderSystem.h"
#include "GL3RenderSystem/GL3Buffer.h"
#include "GL3RenderSystem/GL3ShaderProgram.h"

using namespace Castor3D;

GL3SubmeshRenderer :: GL3SubmeshRenderer( SceneManager * p_pSceneManager)
	:	SubmeshRenderer( p_pSceneManager)
	,	m_eDrawType( eDRAW_TYPE( -1))
	,	m_eNormalsMode( eNORMALS_MODE( -1))
{
	_createVBOs();
}

GL3SubmeshRenderer :: ~GL3SubmeshRenderer()
{
}

void GL3SubmeshRenderer :: Cleanup()
{
	m_vao.Cleanup();
	SubmeshRenderer::Cleanup();
	_createVBOs();
}

void GL3SubmeshRenderer :: Initialise()
{
	m_vertex->Initialise();

	for (int i = 0 ; i < eNbDrawType ; i++)
	{
		m_indices[i]->Initialise();
	}

	if (m_target->GetMaterial() == NULL)
	{
		m_target->SetMaterial( m_pSceneManager->GetMaterialManager()->GetDefaultMaterial());
	}
}

VertexInfosBufferPtr GL3SubmeshRenderer :: CreateVertexBuffer()
{
	VertexInfosBufferPtr l_pReturn = GL3RenderSystem::CreateBuffer<VertexInfosBuffer>();
	return l_pReturn;
}

void GL3SubmeshRenderer :: _drawBuffers( const Pass & p_pass)
{
	GL3ShaderProgramPtr l_pProgram = p_pass.GetShader<GL3ShaderProgram>();
	bool l_bUseShader = ( ! l_pProgram == NULL) && glIsProgram( l_pProgram->GetProgramObject()) && l_pProgram->IsLinked();

	if (l_bUseShader)
	{
		Pipeline::ApplyMatrices();

		int l_iFragColourIndex = glGetFragDataLocation( l_pProgram->GetProgramObject(), "out_FragColor");

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
/*
		_createVAOs( l_pProgram);

		if (m_vao.GetIndex() != GL_INVALID_INDEX)
		{
			m_vao.Activate();

			glDrawElements( l_eMode, m_indices->GetFilled(), GL_UNSIGNED_INT, 0);
			CheckGLError( CU_T( "GL3SubmeshRenderer :: Draw - glDrawArrays"));

			m_vao.Deactivate();
		}
/**/
		static_pointer_cast<GLVertexInfosBufferObject>( m_vertex)->SetShaderProgram( l_pProgram);
		m_vertex->Activate();
		m_indices[m_eLastDrawType]->Activate();
		glDrawElements( l_eMode, m_indices[m_eLastDrawType]->GetFilled(), GL_UNSIGNED_INT, 0);
		CheckGLError( CU_T( "GLPassRenderer :: Draw - glDrawArrays"));
		m_indices[m_eLastDrawType]->Deactivate();
		m_vertex->Deactivate();
/**/
	}
}

void GL3SubmeshRenderer :: _createVAOs( GL3ShaderProgramPtr p_pProgram)
{
	if (m_eDrawType != m_eLastDrawType || m_eNormalsMode != m_eLastNormalsMode || m_vao.GetIndex() == GL_INVALID_INDEX)
	{
		m_eDrawType = m_eLastDrawType;
		m_eNormalsMode = m_eLastNormalsMode;

		static_pointer_cast<GLVertexInfosBufferObject>( m_vertex)->CleanupBufferObject();
		static_pointer_cast<GLVertexInfosBufferObject>( m_vertex)->CleanupAttribute();
		static_pointer_cast<GLVBOIndicesBuffer>( m_indices[m_eDrawType])->CleanupBufferObject();

		static_pointer_cast<GLVertexInfosBufferObject>( m_vertex)->SetShaderProgram( p_pProgram);
		m_indices[m_eDrawType]->Initialise();

		m_vao.Cleanup();
		m_vao.Initialise();
		m_vao.Activate();
		m_vertex->Activate();
		m_indices[m_eLastDrawType]->Activate();
		m_vao.Deactivate();
	}
}