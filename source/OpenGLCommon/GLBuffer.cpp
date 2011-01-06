#include "OpenGLCommon/PrecompiledHeader.h"

#include "OpenGLCommon/GLBuffer.h"

using namespace Castor3D;

//******************************************************************************

GLVBOIndicesBuffer :: GLVBOIndicesBuffer()
	:	GLVertexBufferObject<unsigned int>( IndicesBuffer::m_filled, IndicesBuffer::m_buffer)
{
	m_uiTarget = GL_ELEMENT_ARRAY_BUFFER;
}

GLVBOIndicesBuffer :: ~GLVBOIndicesBuffer()
{
	Cleanup();
}

void GLVBOIndicesBuffer :: Cleanup()
{
	Buffer::Cleanup();
	CleanupBufferObject();
}

void GLVBOIndicesBuffer :: CleanupBufferObject()
{
	if (m_bAssigned)
	{
		BufferManager::UnassignBuffer<unsigned int>( m_uiIndex, this);
	}

	GLVertexBufferObject::CleanupBufferObject();
	m_bAssigned = false;
}

void GLVBOIndicesBuffer :: Initialise()
{
	InitialiseBufferObject();
	m_bAssigned = (m_uiIndex != GL_INVALID_INDEX);

	if (m_bAssigned)
	{
		BufferManager::AssignBuffer<unsigned int>( m_uiIndex, this);
	}
}

void GLVBOIndicesBuffer :: Activate()
{
	if (m_bAssigned)
	{
		ActivateBufferObject();
	}
}

void GLVBOIndicesBuffer :: Deactivate()
{
	if (m_bAssigned)
	{
	}
}

//******************************************************************************

GLVBOVertexBuffer :: GLVBOVertexBuffer()
	:	GLVertexAttribs3r( "vertex"),
		GLVertexBufferObject<real>( VertexBuffer::m_filled, VertexBuffer::m_buffer)
{
}

GLVBOVertexBuffer :: ~GLVBOVertexBuffer()
{
	Cleanup();
}

void GLVBOVertexBuffer :: Cleanup()
{
	Buffer::Cleanup();
	CleanupBufferObject();
	CleanupAttribute();
}

void GLVBOVertexBuffer :: CleanupBufferObject()
{
	if (m_bAssigned)
	{
		BufferManager::UnassignBuffer<real>( m_uiIndex, this);
	}

	GLVertexBufferObject::CleanupBufferObject();
	m_bAssigned = false;
}

void GLVBOVertexBuffer :: SetShaderProgram( ShaderProgramPtr p_pProgram)
{
	GLShaderProgramPtr l_pProgram = static_pointer_cast<GLShaderProgram>( p_pProgram);

	if ((m_pProgram.expired() && ! l_pProgram == NULL) || (l_pProgram == NULL && ! m_pProgram.expired()) || m_pProgram.lock() != l_pProgram)
	{
		CleanupAttribute();
		CleanupBufferObject();
		m_pProgram = l_pProgram;
		Initialise();
	}
}

void GLVBOVertexBuffer :: Initialise()
{
	if (m_pProgram.expired() || InitialiseAttribute())
	{
		InitialiseBufferObject();
		m_bAssigned = (m_uiIndex != GL_INVALID_INDEX);

		if (m_bAssigned)
		{
			BufferManager::AssignBuffer<real>( m_uiIndex, this);
		}
	}
}

void GLVBOVertexBuffer :: Activate()
{
	if (m_bAssigned)
	{
		ActivateBufferObject();

		if (RenderSystem::UseShaders() && ! m_pProgram.expired())
		{
			ActivateAttribute();
		}
		else
		{
			glEnableClientState( GL_VERTEX_ARRAY);
			CheckGLError( CU_T( "GLVBOVertexBuffer :: Activate - glEnableClientState"));
			glVertexPointer( 3, GL_REAL, 0, NULL);
			CheckGLError( CU_T( "GLVBOVertexBuffer :: Activate - glVertexPointer"));
		}
	}
}

void GLVBOVertexBuffer :: Deactivate()
{
	if (m_bAssigned)
	{
		if (RenderSystem::UseShaders() && ! m_pProgram.expired())
		{
			DeactivateAttribute();
		}
		else
		{
			glDisableClientState( GL_VERTEX_ARRAY);
			CheckGLError( CU_T( "GLVBOVertexBuffer :: Deactivate - glDisableClientState"));
		}
	}
}

//******************************************************************************

GLVBONormalsBuffer :: GLVBONormalsBuffer()
	:	GLVertexAttribs3r( "normal"),
		GLVertexBufferObject<real>( NormalsBuffer::m_filled, NormalsBuffer::m_buffer)
{
}

GLVBONormalsBuffer :: ~GLVBONormalsBuffer()
{
	Cleanup();
}

void GLVBONormalsBuffer :: Cleanup()
{
	Buffer::Cleanup();
	CleanupBufferObject();
	CleanupAttribute();
}

void GLVBONormalsBuffer :: CleanupBufferObject()
{
	if (m_bAssigned)
	{
		BufferManager::UnassignBuffer<real>( m_uiIndex, this);
	}

	GLVertexBufferObject::CleanupBufferObject();
	m_bAssigned = false;
}

void GLVBONormalsBuffer :: SetShaderProgram( ShaderProgramPtr p_pProgram)
{
	GLShaderProgramPtr l_pProgram = static_pointer_cast<GLShaderProgram>( p_pProgram);

	if ((m_pProgram.expired() && ! l_pProgram == NULL) || (l_pProgram == NULL && ! m_pProgram.expired()) || m_pProgram.lock() != l_pProgram)
	{
		CleanupAttribute();
		CleanupBufferObject();
		m_pProgram = l_pProgram;
		Initialise();
	}
}

void GLVBONormalsBuffer :: Initialise()
{
	if (m_pProgram.expired() || InitialiseAttribute())
	{
		InitialiseBufferObject();
		m_bAssigned = (m_uiIndex != GL_INVALID_INDEX);

		if (m_bAssigned)
		{
			BufferManager::AssignBuffer<real>( m_uiIndex, this);
		}
	}
}

void GLVBONormalsBuffer :: Activate()
{
	if (m_bAssigned)
	{
		ActivateBufferObject();

		if (RenderSystem::UseShaders() && ! m_pProgram.expired())
		{
			ActivateAttribute();
		}
		else
		{
			CheckGLError( CU_T( "GLVBONormalsBuffer :: Activate - glBindBuffer"));
			glEnableClientState( GL_NORMAL_ARRAY);
			CheckGLError( CU_T( "GLVBONormalsBuffer :: Activate - glEnableClientState"));
			glNormalPointer( GL_REAL, 0, NULL);
			CheckGLError( CU_T( "GLVBONormalsBuffer :: Activate - glNormalPointer"));
		}
	}
}

void GLVBONormalsBuffer :: Deactivate()
{
	if (m_bAssigned)
	{
		if (RenderSystem::UseShaders() && ! m_pProgram.expired())
		{
			DeactivateAttribute();
		}
		else
		{
			glDisableClientState( GL_NORMAL_ARRAY);
			CheckGLError( CU_T( "GLVBONormalsBuffer :: Deactivate - glDisableClientState"));
		}
	}
}

//******************************************************************************

GLVertexInfosBufferObject :: GLVertexInfosBufferObject()
	:	GLVertexBufferObject<real>( VertexInfosBuffer::m_filled, VertexInfosBuffer::m_buffer),
		m_vertex( "vertex"),
		m_normal( "normal"),
		m_tangent( "tangent"),
		m_texture( "texture")
{
}

GLVertexInfosBufferObject :: ~GLVertexInfosBufferObject()
{
	Cleanup();
}

void GLVertexInfosBufferObject :: Cleanup()
{
	Buffer::Cleanup();
	CleanupAttribute();
	CleanupBufferObject();
}

void GLVertexInfosBufferObject :: CleanupBufferObject()
{
	if (m_bAssigned)
	{
		BufferManager::UnassignBuffer<real>( m_uiIndex, this);
	}

	GLVertexBufferObject::CleanupBufferObject();
	m_bAssigned = false;
}

void GLVertexInfosBufferObject :: SetShaderProgram( ShaderProgramPtr p_pProgram)
{
	GLShaderProgramPtr l_pProgram = static_pointer_cast<GLShaderProgram>( p_pProgram);

	if ((m_pProgram.expired() && ! l_pProgram == NULL) || (l_pProgram == NULL && ! m_pProgram.expired()) || m_pProgram.lock() != l_pProgram)
	{
		CleanupBufferObject();
		CleanupAttribute();
		m_vertex.SetShaderProgram( l_pProgram);
		m_normal.SetShaderProgram( l_pProgram);
		m_tangent.SetShaderProgram( l_pProgram);
		m_texture.SetShaderProgram( l_pProgram);
		m_pProgram = l_pProgram;
		Initialise();
	}
}

void GLVertexInfosBufferObject :: Initialise()
{
	if (m_pProgram.expired() || InitialiseAttribute())
	{
		InitialiseBufferObject();
		m_bAssigned = (m_uiIndex != GL_INVALID_INDEX);

		if (m_bAssigned)
		{
			BufferManager::AssignBuffer<real>( m_uiIndex, this);
		}
	}
}

void GLVertexInfosBufferObject :: Activate()
{
	if (m_bAssigned)
	{
		ActivateBufferObject();

		if (RenderSystem::UseShaders() && ! m_pProgram.expired())
		{
			ActivateAttribute();
		}
		else
		{
			glInterleavedArrays( GL_T2F_N3F_V3F, Vertex::Size * sizeof( real), 0);
			CheckGLError( CU_T( "GLVertexInfosBufferObject :: Activate - glNormalPointer"));
		}
	}
}

void GLVertexInfosBufferObject :: Deactivate()
{
	if (m_bAssigned)
	{
		if (RenderSystem::UseShaders() && ! m_pProgram.expired())
		{
			DeactivateAttribute();
		}
	}
}

void GLVertexInfosBufferObject :: CleanupAttribute()
{
	m_texture.CleanupAttribute();
	m_normal.CleanupAttribute();
	m_vertex.CleanupAttribute();
	m_tangent.CleanupAttribute();
}

bool GLVertexInfosBufferObject :: InitialiseAttribute()
{
	m_uiValid = 0;
	m_uiValid += (m_texture.InitialiseAttribute() ? 1 : 0);
	m_uiValid += (m_normal.InitialiseAttribute() ? 1 : 0);
	m_uiValid += (m_vertex.InitialiseAttribute() ? 1 : 0);
	m_uiValid += (m_tangent.InitialiseAttribute() ? 1 : 0);
	return m_uiValid > 0;
}

void GLVertexInfosBufferObject :: ActivateAttribute()
{
	if (m_texture.GetAttribLocation() != GL_INVALID_INDEX)
	{
		m_texture.ActivateAttribute( false, Vertex::Size * sizeof( real), 0 * sizeof( real));
	}

	if (m_normal.GetAttribLocation() != GL_INVALID_INDEX)
	{
		m_normal.ActivateAttribute( true, Vertex::Size * sizeof( real), 2 * sizeof( real));
	}

	if (m_vertex.GetAttribLocation() != GL_INVALID_INDEX)
	{
		m_vertex.ActivateAttribute( false, Vertex::Size * sizeof( real), 5 * sizeof( real));
	}
	
	if (m_tangent.GetAttribLocation() != GL_INVALID_INDEX)
	{
		m_tangent.ActivateAttribute( true, Vertex::Size * sizeof( real), 8 * sizeof( real));
	}
}

void GLVertexInfosBufferObject :: DeactivateAttribute()
{
	if (m_texture.GetAttribLocation() != GL_INVALID_INDEX)
	{
		m_texture.DeactivateAttribute();
	}

	if (m_normal.GetAttribLocation() != GL_INVALID_INDEX)
	{
		m_normal.DeactivateAttribute();
	}

	if (m_vertex.GetAttribLocation() != GL_INVALID_INDEX)
	{
		m_vertex.DeactivateAttribute();
	}

	if (m_tangent.GetAttribLocation() != GL_INVALID_INDEX)
	{
		m_tangent.DeactivateAttribute();
	}
}

//******************************************************************************