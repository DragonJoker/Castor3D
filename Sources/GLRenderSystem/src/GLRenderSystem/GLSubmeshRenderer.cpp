#include "PrecompiledHeader.h"

#include "GLSubmeshRenderer.h"
#include "GLContext.h"
#include "GLRenderSystem.h"
#include "GLBuffer.h"
#include "Module_GL.h"

using namespace Castor3D;

GLSubmeshRenderer :: GLSubmeshRenderer( GLRenderSystem * p_rs)
	:	SubmeshRenderer( p_rs),
		m_glRenderSystem	( p_rs)
{
	_createBuffers();
}

GLSubmeshRenderer :: ~GLSubmeshRenderer()
{
}

void GLSubmeshRenderer :: Cleanup()
{
	BufferManager::GetSingleton().DeleteBuffer( m_triangles);
	BufferManager::GetSingleton().DeleteBuffer( m_trianglesNormals);
	BufferManager::GetSingleton().DeleteBuffer( m_trianglesTangents);
	BufferManager::GetSingleton().DeleteBuffer( m_trianglesTexCoords);
	BufferManager::GetSingleton().DeleteBuffer( m_lines);
	BufferManager::GetSingleton().DeleteBuffer( m_linesNormals);
	BufferManager::GetSingleton().DeleteBuffer( m_linesTexCoords);

	_createBuffers();
}

void GLSubmeshRenderer :: Initialise()
{
	if (m_glRenderSystem->UseVBO())
	{
		m_triangles->Initialise();
		m_trianglesTexCoords->Initialise();
		
		m_lines->Initialise();
		m_linesTexCoords->Initialise();
	}
}

void GLSubmeshRenderer :: Draw( DrawType p_displayMode, Pass * p_pass)
{
	bool l_bUseShader = p_pass->GetShader() != NULL;

	if (l_bUseShader)
	{
		m_trianglesTangents->SetAttribLocation( p_pass->GetShader()->GetTangentAttribLocation());
	}

	switch( p_displayMode)
	{
	case DTTriangles:
		m_triangles->Activate();
		m_trianglesNormals->Activate();
		m_trianglesTexCoords->Activate( p_pass);

		if (l_bUseShader)
		{
			m_trianglesTangents->Activate();
			glDrawArrays( GL_TRIANGLES, 0, m_triangles->GetFilled() / 3);
			CheckGLError( "GLPassRenderer :: Draw - glDrawArrays");
			m_trianglesTangents->Deactivate();
		}
		else
		{
			glDrawArrays( GL_TRIANGLES, 0, m_triangles->GetFilled() / 3);
			CheckGLError( "GLPassRenderer :: Draw - glDrawArrays");
		}

		m_trianglesTexCoords->Deactivate();
		m_trianglesNormals->Deactivate();
		m_triangles->Deactivate();
	break;

	case DTTriangleStrip:
//		m_triangles->Activate();
		m_trianglesNormals->Activate();
		m_trianglesTexCoords->Activate( p_pass);

		if (l_bUseShader)
		{
			m_trianglesTangents->Activate();
			glDrawArrays( GL_TRIANGLE_STRIP, 0, m_triangles->GetFilled() / 3);
			CheckGLError( "GLPassRenderer :: Draw - glDrawArrays");
			m_trianglesTangents->Deactivate();
		}
		else
		{
			glDrawArrays( GL_TRIANGLE_STRIP, 0, m_triangles->GetFilled() / 3);
			CheckGLError( "GLPassRenderer :: Draw - glDrawArrays");
		}

		m_trianglesTexCoords->Deactivate();
		m_trianglesNormals->Deactivate();
//		m_triangles->Deactivate();
	break;

	case DTTriangleFan:
//		m_triangles->Activate();
		m_trianglesNormals->Activate();
		m_trianglesTexCoords->Activate( p_pass);

		if (l_bUseShader)
		{
			m_trianglesTangents->Activate();
			glDrawArrays( GL_TRIANGLE_FAN, 0, m_triangles->GetFilled() / 3);
			CheckGLError( "GLPassRenderer :: Draw - glDrawArrays");
			m_trianglesTangents->Deactivate();
		}
		else
		{
			glDrawArrays( GL_TRIANGLE_FAN, 0, m_triangles->GetFilled() / 3);
			CheckGLError( "GLPassRenderer :: Draw - glDrawArrays");
		}

		m_trianglesTexCoords->Deactivate();
		m_trianglesNormals->Deactivate();
//		m_triangles->Deactivate();
		break;

	case DTLines:
		m_lines->Activate();
		m_linesTexCoords->Activate( p_pass);

		glDrawArrays( GL_LINES, 0, m_lines->GetFilled() / 3);
		CheckGLError( "GLPassRenderer :: Draw - glDrawArrays");

		m_linesTexCoords->Deactivate();
		m_lines->Deactivate();
	break;

	case DTQuads:
//		m_triangles->Activate();
		m_trianglesNormals->Activate();
		m_trianglesTexCoords->Activate( p_pass);

		if (l_bUseShader)
		{
			m_trianglesTangents->Activate();
			glDrawArrays( GL_QUADS, 0, m_triangles->GetFilled() / 3);
			CheckGLError( "GLPassRenderer :: Draw - glDrawArrays");
			m_trianglesTangents->Deactivate();
		}
		else
		{
			glDrawArrays( GL_QUADS, 0, m_triangles->GetFilled() / 3);
			CheckGLError( "GLPassRenderer :: Draw - glDrawArrays");
		}

		m_trianglesTexCoords->Deactivate();
		m_trianglesNormals->Deactivate();
//		m_triangles->Deactivate();
	break;

	case DTQuadStrip:
//		m_triangles->Activate();
		m_trianglesNormals->Activate();
		m_trianglesTexCoords->Activate( p_pass);

		if (l_bUseShader)
		{
			m_trianglesTangents->Activate();
			glDrawArrays( GL_QUAD_STRIP, 0, m_triangles->GetFilled() / 3);
			CheckGLError( "GLPassRenderer :: Draw - glDrawArrays");
			m_trianglesTangents->Deactivate();
		}
		else
		{
			glDrawArrays( GL_QUAD_STRIP, 0, m_triangles->GetFilled() / 3);
			CheckGLError( "GLPassRenderer :: Draw - glDrawArrays");
		}

		m_trianglesTexCoords->Deactivate();
		m_trianglesNormals->Deactivate();
//		m_triangles->Deactivate();
	break;

	default:
	break;
	}
}

void GLSubmeshRenderer :: ShowVertex( DrawType p_displayMode)
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

void GLSubmeshRenderer :: _createBuffers()
{
	if (GLRenderSystem::UseVBO())
	{
		m_triangles = new GLVBOVertexBuffer;
		m_trianglesNormals = new GLVBONormalsBuffer;
		m_trianglesTangents = new GLVBOVertexAttribsBuffer;
		m_trianglesTexCoords = new GLVBOTextureBuffer;
		m_lines = new GLVBOVertexBuffer;
		m_linesNormals = new GLVBONormalsBuffer;
		m_linesTexCoords = new GLVBOTextureBuffer;
	}
	else
	{
		m_triangles = new GLVBVertexBuffer;
		m_trianglesNormals = new GLVBNormalsBuffer;
		m_trianglesTangents = new GLVBVertexAttribsBuffer;
		m_trianglesTexCoords = new GLVBTextureBuffer;
		m_lines = new GLVBVertexBuffer;
		m_linesNormals = new GLVBNormalsBuffer;
		m_linesTexCoords = new GLVBTextureBuffer;
	}
}