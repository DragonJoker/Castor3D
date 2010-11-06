#include "PrecompiledHeader.h"

#include "GLSubmeshRenderer.h"
#include "GLContext.h"
#include "GLRenderSystem.h"
#include "GLBuffer.h"
#include "Module_GL.h"
#include "GLPipeline.h"

using namespace Castor3D;

GLSubmeshRenderer :: GLSubmeshRenderer()
{
	_createBuffers();
}

GLSubmeshRenderer :: ~GLSubmeshRenderer()
{
}

void GLSubmeshRenderer :: Cleanup()
{
	BufferManager::GetSingleton().DeleteBuffer<real>( m_triangles);
	BufferManager::GetSingleton().DeleteBuffer<real>( m_trianglesNormals);
	BufferManager::GetSingleton().DeleteBuffer<real>( m_trianglesTangents);
	BufferManager::GetSingleton().DeleteBuffer<real>( m_trianglesTexCoords);
	BufferManager::GetSingleton().DeleteBuffer<real>( m_lines);
	BufferManager::GetSingleton().DeleteBuffer<real>( m_linesNormals);
	BufferManager::GetSingleton().DeleteBuffer<real>( m_linesTexCoords);

	_createBuffers();
}

void GLSubmeshRenderer :: Initialise()
{
	if (GLRenderSystem::GetSingletonPtr()->UseVBO())
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
}

void GLSubmeshRenderer :: Draw( eDRAW_TYPE p_displayMode, PassPtr p_pass)
{
	bool l_bUseShader = GLRenderSystem::GetSingletonPtr()->UseShaders() && p_pass->HasShader();

	if (l_bUseShader)
	{
		GLVBOVertexBufferPtr( m_triangles)->SetShaderProgram( p_pass->GetShader().get());
		GLVBOVertexBufferPtr( m_lines)->SetShaderProgram( p_pass->GetShader().get());
		GLVBONormalsBufferPtr( m_trianglesNormals)->SetShaderProgram( p_pass->GetShader().get());
		GLVBONormalsBufferPtr( m_linesNormals)->SetShaderProgram( p_pass->GetShader().get());
		GLVBOTextureBufferPtr( m_trianglesTexCoords)->SetShaderProgram( p_pass->GetShader().get());
		GLVBOTextureBufferPtr( m_linesTexCoords)->SetShaderProgram( p_pass->GetShader().get());
		m_trianglesTangents->SetShaderProgram( p_pass->GetShader().get());
		Pipeline::Apply( p_pass->GetShader());
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
		m_triangles->Activate();
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
		m_triangles->Deactivate();
	break;

	case DTTriangleFan:
		m_triangles->Activate();
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
		m_triangles->Deactivate();
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
		m_triangles->Activate();
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
		m_triangles->Deactivate();
	break;

	case DTQuadStrip:
		m_triangles->Activate();
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
		m_triangles->Deactivate();
	break;

	default:
	break;
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

void GLSubmeshRenderer :: _createBuffers()
{
	m_triangles.reset(				BufferManager::CreateBuffer<real, GLVBOVertexBuffer>());
	m_lines.reset(					BufferManager::CreateBuffer<real, GLVBOVertexBuffer>());
	m_trianglesNormals.reset(		BufferManager::CreateBuffer<real, GLVBONormalsBuffer>());
	m_linesNormals.reset(			BufferManager::CreateBuffer<real, GLVBONormalsBuffer>());
	m_trianglesTexCoords.reset(		BufferManager::CreateBuffer<real, GLVBOTextureBuffer>());
	m_linesTexCoords.reset(			BufferManager::CreateBuffer<real, GLVBOTextureBuffer>());

	if (GLRenderSystem::GetSingletonPtr()->UseShaders())
	{
		m_trianglesTangents.reset(	BufferManager::CreateBuffer<real, GLVertexAttribsBuffer3r>( "tangent"));
	}
}