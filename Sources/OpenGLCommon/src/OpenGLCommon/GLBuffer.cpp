#include "PrecompiledHeader.h"

#include "OpenGLCommon/GLBuffer.h"

using namespace Castor3D;

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

void GLVBOVertexBuffer :: SetShaderProgram( ShaderProgram * p_pProgram)
{
	GLShaderProgram * l_pProgram = (GLShaderProgram *)p_pProgram;

	if (m_pProgram != l_pProgram)
	{
		CleanupAttribute();
		CleanupBufferObject();
		m_pProgram = l_pProgram;
		Initialise();
	}
}

void GLVBOVertexBuffer :: Initialise()
{
	if (m_pProgram == NULL || InitialiseAttribute())
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

		if (RenderSystem::UseShaders() && m_pProgram != NULL)
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
		if (RenderSystem::UseShaders() && m_pProgram != NULL)
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

void GLVBONormalsBuffer :: SetShaderProgram( ShaderProgram * p_pProgram)
{
	GLShaderProgram * l_pProgram = (GLShaderProgram *)p_pProgram;

	if (m_pProgram != l_pProgram)
	{
		CleanupAttribute();
		CleanupBufferObject();
		m_pProgram = l_pProgram;
		Initialise();
	}
}

void GLVBONormalsBuffer :: Initialise()
{
	if (m_pProgram == NULL || InitialiseAttribute())
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

		if (RenderSystem::UseShaders() && m_pProgram != NULL)
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
		if (RenderSystem::UseShaders() && m_pProgram != NULL)
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

void GLVertexInfosBufferObject :: SetShaderProgram( ShaderProgram * p_pProgram)
{
	GLShaderProgram * l_pProgram = (GLShaderProgram *)p_pProgram;

	if (m_pProgram != l_pProgram)
	{
		CleanupAttribute();
		CleanupBufferObject();
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
	if (m_pProgram == NULL || InitialiseAttribute())
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

		if (RenderSystem::UseShaders() && m_pProgram != NULL)
		{
			ActivateAttribute();
		}
		else
		{
			glInterleavedArrays( GL_T2F_N3F_V3F, 12 * sizeof( real), m_buffer);
			CheckGLError( CU_T( "GLVBONormalsBuffer :: Activate - glNormalPointer"));
		}
	}
}

void GLVertexInfosBufferObject :: Deactivate()
{
	if (m_bAssigned)
	{
		if (RenderSystem::UseShaders() && m_pProgram != NULL)
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
		m_texture.ActivateAttribute( false, 11 * sizeof( real), 0 * sizeof( real));
	}

	if (m_normal.GetAttribLocation() != GL_INVALID_INDEX)
	{
		m_normal.ActivateAttribute( true, 11 * sizeof( real), 2 * sizeof( real));
	}

	if (m_vertex.GetAttribLocation() != GL_INVALID_INDEX)
	{
		m_vertex.ActivateAttribute( false, 11 * sizeof( real), 5 * sizeof( real));
	}
	
	if (m_tangent.GetAttribLocation() != GL_INVALID_INDEX)
	{
		m_tangent.ActivateAttribute( true, 11 * sizeof( real), 8 * sizeof( real));
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