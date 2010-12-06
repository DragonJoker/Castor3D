#include "PrecompiledHeader.h"

#include "GL3SubmeshRenderer.h"
#include "GL3Context.h"
#include "GL3RenderSystem.h"
#include "GL3Buffer.h"

using namespace Castor3D;

GL3SubmeshRenderer :: GL3SubmeshRenderer()
	:	m_uiVertextArrayObject	( 0),
		m_eDisplayMode			( eDRAW_TYPE( -1))
{
	_createVBOs();
}

GL3SubmeshRenderer :: ~GL3SubmeshRenderer()
{
}

void GL3SubmeshRenderer :: Cleanup()
{
	if (m_uiVertextArrayObject > 0)
	{
		glDeleteVertexArrays( 1, & m_uiVertextArrayObject);
	}

	BufferManager::GetSingleton().DeleteBuffer<real>( m_triangles);
	BufferManager::GetSingleton().DeleteBuffer<real>( m_lines);

	_createVBOs();
}

void GL3SubmeshRenderer :: Initialise()
{
	m_triangles->Initialise();
	m_lines->Initialise();

	if (m_target->GetMaterial().null())
	{
		m_target->SetMaterial( MaterialManager::GetSingleton().GetDefaultMaterial());
	}
}

void GL3SubmeshRenderer :: Draw( eDRAW_TYPE p_displayMode, PassPtr p_pPass)
{
	if (p_pPass->GetShader() != NULL && glIsProgram( static_pointer_cast<GLShaderProgram>( p_pPass->GetShader())->GetProgramObject()) && p_pPass->GetShader()->IsLinked())
	{
		_createVAOs( p_displayMode, p_pPass);
		size_t l_uiDataBuffer = glGetFragDataLocation( static_pointer_cast<GLShaderProgram>( p_pPass->GetShader())->GetProgramObject(), "out_FragColor");
		CheckGLError( CU_T( "GL3SubmeshRenderer :: Draw - glGetFragDataLocation"));
		Pipeline::ApplyMatrices();

		if (m_uiVertextArrayObject > 0)
		{
			glBindVertexArray( m_uiVertextArrayObject);
			CheckGLError( CU_T( "GL3SubmeshRenderer :: Draw - glBindVertexArray"));

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
				l_uiSize = m_triangles->GetFilled() / Vertex::Size;
				break;
			case DTLines:
				l_iDisplayMode = GL_LINES;
				l_uiSize = m_lines->GetFilled() / Vertex::Size;
				break;
			}

			glDrawArrays( l_iDisplayMode, 0, l_uiSize);
			CheckGLError( CU_T( "GL3SubmeshRenderer :: Draw - glDrawArrays"));

			glBindVertexArray( 0);
			CheckGLError( CU_T( "GL3SubmeshRenderer :: Draw - glBindVertexArray( 0)"));
		}
	}
}

void GL3SubmeshRenderer :: ShowVertex( eDRAW_TYPE p_displayMode)
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

void GL3SubmeshRenderer :: HideVertex()
{
	m_triangles->Deactivate();
	m_lines->Deactivate();
}

void GL3SubmeshRenderer :: _createVBOs()
{
	m_triangles =	BufferManager::CreateBuffer<real, GLVertexInfosBufferObject>();
	m_lines =		BufferManager::CreateBuffer<real, GLVertexInfosBufferObject>();
}

void GL3SubmeshRenderer :: _createVAOs( eDRAW_TYPE p_eDisplayMode, PassPtr p_pPass)
{
	if (m_eDisplayMode != p_eDisplayMode || m_uiVertextArrayObject <= 0)
	{
		GLShaderProgramPtr l_pProgram = Templates::static_pointer_cast<GLShaderProgram>( p_pPass->GetShader());

		static_pointer_cast<GLVertexInfosBufferObject>( m_triangles)->CleanupBufferObject();
		static_pointer_cast<GLVertexInfosBufferObject>( m_triangles)->CleanupAttribute();
		static_pointer_cast<GLVertexInfosBufferObject>( m_lines)->CleanupBufferObject();
		static_pointer_cast<GLVertexInfosBufferObject>( m_lines)->CleanupAttribute();

		if (m_uiVertextArrayObject > 0)
		{
			glDeleteVertexArrays( 1, & m_uiVertextArrayObject);
			CheckGLError( CU_T( "GL3SubmeshRenderer :: _createVAOs - glDeleteVertexArrays"));
		}

		glGenVertexArrays( 1, & m_uiVertextArrayObject);
		CheckGLError( CU_T( "GL3SubmeshRenderer :: _createVAOs - glGenVertexArrays"));

		glBindVertexArray( m_uiVertextArrayObject);
		CheckGLError( CU_T( "GL3SubmeshRenderer :: _createVAOs - glBindVertexArray"));

		static_pointer_cast<GLVertexInfosBufferObject>( m_triangles)->SetShaderProgram( p_pPass->GetShader().get());
		static_pointer_cast<GLVertexInfosBufferObject>( m_lines)->SetShaderProgram( p_pPass->GetShader().get());

		switch( p_eDisplayMode)
		{
		case DTTriangles:
		case DTTriangleStrip:
		case DTTriangleFan:
		case DTQuads:
		case DTQuadStrip:
			m_triangles->Activate();
			break;

		case DTLines:
			m_lines->Activate();
			break;

		default:
			break;
		}

		m_eDisplayMode = p_eDisplayMode;

		glBindVertexArray( 0);
		CheckGLError( CU_T( "GL3SubmeshRenderer :: _createVAOs - glBindVertexArray( 0)"));
	}
}