#include "PrecompiledHeader.h"

#include "GL2SubmeshRenderer.h"
#include "GL2RenderSystem.h"
#include "GL2Buffer.h"

using namespace Castor3D;

GL2SubmeshRenderer :: GL2SubmeshRenderer()
{
	_createBuffers();
}

GL2SubmeshRenderer :: ~GL2SubmeshRenderer()
{
}

void GL2SubmeshRenderer :: Cleanup()
{

	BufferManager::GetSingleton().DeleteBuffer<real>( m_triangles);
	BufferManager::GetSingleton().DeleteBuffer<real>( m_lines);
	_createBuffers();
}

void GL2SubmeshRenderer :: Initialise()
{
	if (GL2RenderSystem::GetSingletonPtr()->UseVBO())
	{
		m_triangles->Initialise();
		m_lines->Initialise();
	}
}

void GL2SubmeshRenderer :: Draw( eDRAW_TYPE p_displayMode, PassPtr p_pass)
{
	bool l_bUseShader = RenderSystem::UseShaders() && p_pass->HasShader();

	if (l_bUseShader)
	{
		Templates::static_pointer_cast<GLVertexInfosBufferObject>( m_triangles)->SetShaderProgram( p_pass->GetShader().get());
		Templates::static_pointer_cast<GLVertexInfosBufferObject>( m_lines)->SetShaderProgram( p_pass->GetShader().get());
		Pipeline::ApplyMatrices();
	}

	switch( p_displayMode)
	{
	case DTTriangles:
		m_triangles->Activate();
		glDrawArrays( GL_TRIANGLES, 0, m_triangles->GetFilled() / Vertex::Size);
		CheckGLError( CU_T( "GLPassRenderer :: Draw - glDrawArrays"));
		m_triangles->Deactivate();
		break;

	case DTTriangleStrip:
		m_triangles->Activate();
		glDrawArrays( GL_TRIANGLE_STRIP, 0, m_triangles->GetFilled() / Vertex::Size);
		CheckGLError( CU_T( "GLPassRenderer :: Draw - glDrawArrays"));
		m_triangles->Deactivate();
		break;

	case DTTriangleFan:
		m_triangles->Activate();
		glDrawArrays( GL_TRIANGLE_FAN, 0, m_triangles->GetFilled() / Vertex::Size);
		CheckGLError( CU_T( "GLPassRenderer :: Draw - glDrawArrays"));
		m_triangles->Deactivate();
		break;

	case DTLines:
		m_lines->Activate();
		glDrawArrays( GL_LINES, 0, m_lines->GetFilled() / Vertex::Size);
		CheckGLError( CU_T( "GLPassRenderer :: Draw - glDrawArrays"));
		m_lines->Deactivate();
		break;

	case DTQuads:
		m_triangles->Activate();
		glDrawArrays( GL_QUADS, 0, m_triangles->GetFilled() / Vertex::Size);
		CheckGLError( CU_T( "GLPassRenderer :: Draw - glDrawArrays"));
		m_triangles->Deactivate();
		break;

	case DTQuadStrip:
		m_triangles->Activate();
		glDrawArrays( GL_QUAD_STRIP, 0, m_triangles->GetFilled() / Vertex::Size);
		CheckGLError( CU_T( "GLPassRenderer :: Draw - glDrawArrays"));
		m_triangles->Deactivate();
		break;

	default:
		break;
	}
}

void GL2SubmeshRenderer :: ShowVertex( eDRAW_TYPE p_displayMode)
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

void GL2SubmeshRenderer :: HideVertex()
{
	m_triangles->Deactivate();
	m_lines->Deactivate();
}

void GL2SubmeshRenderer :: _createBuffers()
{
	m_triangles =				BufferManager::CreateBuffer<real, GLVertexInfosBufferObject>();
	m_lines =					BufferManager::CreateBuffer<real, GLVertexInfosBufferObject>();
}