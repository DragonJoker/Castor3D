#include "PrecompiledHeader.h"

#include "GLSubmeshRenderer.h"
#include "GLContext.h"
#include "GLRenderSystem.h"
#include "GLBuffer.h"
#include "Module_GL.h"
#include "GLUniformVariable.h"
#include "GLPipeline.h"

using namespace Castor3D;

GLSubmeshRenderer :: GLSubmeshRenderer()
	:	m_uiVertextArrayObject	( 0),
		m_eDisplayMode			( eDRAW_TYPE( -1))
{
	_createVBOs();
}

GLSubmeshRenderer :: ~GLSubmeshRenderer()
{
}

void GLSubmeshRenderer :: Cleanup()
{
	if (m_uiVertextArrayObject > 0)
	{
		glDeleteVertexArrays( 1, & m_uiVertextArrayObject);
	}

	BufferManager::GetSingleton().DeleteBuffer<real>( m_triangles);
	BufferManager::GetSingleton().DeleteBuffer<real>( m_trianglesNormals);
	BufferManager::GetSingleton().DeleteBuffer<real>( m_trianglesTangents);
	BufferManager::GetSingleton().DeleteBuffer<real>( m_trianglesTexCoords);
	BufferManager::GetSingleton().DeleteBuffer<real>( m_lines);
	BufferManager::GetSingleton().DeleteBuffer<real>( m_linesNormals);
	BufferManager::GetSingleton().DeleteBuffer<real>( m_linesTexCoords);

	_createVBOs();
}

void GLSubmeshRenderer :: Initialise()
{
	m_triangles->Initialise();
	m_lines->Initialise();

	if ( ! m_target->GetMaterial().null())
	{
		GLVBOTextureBufferPtr( m_trianglesTexCoords)->Initialise( m_target->GetMaterial()->GetPass( 0));
		GLVBOTextureBufferPtr( m_linesTexCoords)->Initialise( m_target->GetMaterial()->GetPass( 0));
	}
	else
	{
		GLVBOTextureBufferPtr( m_trianglesTexCoords)->Initialise( PassPtr());
		GLVBOTextureBufferPtr( m_linesTexCoords)->Initialise( PassPtr());
	}
}

void GLSubmeshRenderer :: Draw( eDRAW_TYPE p_displayMode, PassPtr p_pPass)
{
	_createVAOs( p_displayMode, p_pPass);
//	size_t l_uiDataBuffer = glGetFragDataLocation( GLShaderProgramPtr( p_pPass->GetShader())->GetProgramObject(), "FragColor");

	if (m_uiVertextArrayObject > 0)
	{
		glBindVertexArray( m_uiVertextArrayObject);
		CheckGLError( "GLSubmeshRenderer :: Draw - glBindVertexArray");

		int l_iDisplayMode = 0;
		size_t l_uiSize = 0;

		switch( p_displayMode)
		{
		case DTTriangles:
		case DTTriangleStrip:
		case DTTriangleFan:
		case DTQuads:
		case DTQuadStrip:
			l_iDisplayMode = GL_TRIANGLES;
			l_uiSize = m_triangles->GetFilled() / 3;
			break;

		case DTLines:
			l_iDisplayMode = GL_LINES;
			l_uiSize = m_lines->GetFilled() / 3;
			break;
		}

		Pipeline::Apply( p_pPass->GetShader());

		glDrawArrays( l_iDisplayMode, 0, l_uiSize);
		CheckGLError( "GLSubmeshRenderer :: Draw - glDrawArrays");

		glBindVertexArray( 0);
		CheckGLError( "GLSubmeshRenderer :: Draw - glBindVertexArray( 0)");
	}
}

void GLSubmeshRenderer :: ShowVertex( eDRAW_TYPE p_displayMode)
{
	if (p_displayMode == DTTriangles)
	{
		m_triangles->Activate();
	}
	else
	{
		m_lines->Activate();
	}
}

void GLSubmeshRenderer :: HideVertex()
{
	m_triangles->Deactivate();
	m_lines->Deactivate();
}

void GLSubmeshRenderer :: _createVBOs()
{
	m_triangles.reset(			BufferManager::CreateBuffer<real, GLVBOVertexBuffer>());
	m_lines.reset(				BufferManager::CreateBuffer<real, GLVBOVertexBuffer>());
	m_trianglesNormals.reset(	BufferManager::CreateBuffer<real, GLVBONormalsBuffer>());
	m_linesNormals.reset(		BufferManager::CreateBuffer<real, GLVBONormalsBuffer>());
	m_trianglesTexCoords.reset(	BufferManager::CreateBuffer<real, GLVBOTextureBuffer>());
	m_linesTexCoords.reset(		BufferManager::CreateBuffer<real, GLVBOTextureBuffer>());
	m_trianglesTangents.reset(	BufferManager::CreateBuffer<real, GLVertexAttribsBuffer3r>( "tangent"));
}

void GLSubmeshRenderer :: _createVAOs( eDRAW_TYPE p_eDisplayMode, PassPtr p_pPass)
{
	if (m_eDisplayMode != p_eDisplayMode || m_uiVertextArrayObject <= 0)
	{
		GLShaderProgramPtr l_pProgram = p_pPass->GetShader();
		GLVBOVertexBufferPtr( m_triangles)->CleanupBufferObject();
		GLVBONormalsBufferPtr( m_trianglesNormals)->CleanupBufferObject();
		GLVertexAttribsBuffer3rPtr( m_trianglesTangents)->CleanupBufferObject();
		GLVBOTextureBufferPtr( m_trianglesTexCoords)->CleanupBufferObject();
		GLVBOVertexBufferPtr( m_triangles)->CleanupAttribute();
		GLVBONormalsBufferPtr( m_trianglesNormals)->CleanupAttribute();
		GLVertexAttribsBuffer3rPtr( m_trianglesTangents)->CleanupAttribute();
		GLVBOTextureBufferPtr( m_trianglesTexCoords)->CleanupAttribute();

		if (m_uiVertextArrayObject > 0)
		{
			glDeleteVertexArrays( 1, & m_uiVertextArrayObject);
			CheckGLError( "GLSubmeshRenderer :: _createVAOs - glDeleteVertexArrays");
		}

		glGenVertexArrays( 1, & m_uiVertextArrayObject);
		CheckGLError( "GLSubmeshRenderer :: _createVAOs - glGenVertexArrays");

		glBindVertexArray( m_uiVertextArrayObject);
		CheckGLError( "GLSubmeshRenderer :: _createVAOs - glBindVertexArray");

		GLVBOVertexBufferPtr( m_triangles)->SetShaderProgram( p_pPass->GetShader().get());
		GLVBOVertexBufferPtr( m_lines)->SetShaderProgram( p_pPass->GetShader().get());
		GLVBONormalsBufferPtr( m_trianglesNormals)->SetShaderProgram( p_pPass->GetShader().get());
		GLVBONormalsBufferPtr( m_linesNormals)->SetShaderProgram( p_pPass->GetShader().get());
		GLVBOTextureBufferPtr( m_trianglesTexCoords)->SetShaderProgram( p_pPass->GetShader().get());
		GLVBOTextureBufferPtr( m_linesTexCoords)->SetShaderProgram( p_pPass->GetShader().get());
		m_trianglesTangents->SetShaderProgram( p_pPass->GetShader().get());

		switch( p_eDisplayMode)
		{
		case DTTriangles:
		case DTTriangleStrip:
		case DTTriangleFan:
		case DTQuads:
		case DTQuadStrip:
			m_triangles->Activate();
			m_trianglesNormals->Activate();
			m_trianglesTangents->Activate();
			m_trianglesTexCoords->Activate( p_pPass);
			break;

		case DTLines:
			m_lines->Activate();
			m_linesNormals->Activate();
			m_linesTexCoords->Activate( p_pPass);
			break;

		default:
			break;
		}

		m_eDisplayMode = p_eDisplayMode;

		glBindVertexArray( 0);
		CheckGLError( "GLSubmeshRenderer :: _createVAOs - glBindVertexArray( 0)");
	}
}