#include "PrecompiledHeader.h"

#include "../../include/GL2RenderSystem/GLBuffer.h"
#include "Module_GL.h"
#include "GLRenderSystem.h"

using namespace Castor3D;

//******************************************************************************

void GLVBVertexBuffer :: Activate()
{
	glEnableClientState( GL_VERTEX_ARRAY);
	CheckGLError( "GLVBVertexBuffer :: Activate - glEnableClientState");
	glVertexPointer( 3, GL_REAL, 0, m_buffer);
	CheckGLError( "GLVBVertexBuffer :: Activate - glVertexPointer");
}

void GLVBVertexBuffer :: Deactivate()
{
	glDisableClientState( GL_VERTEX_ARRAY);
	CheckGLError( "GLVBVertexBuffer :: Deactivate - glDisableClientState");
}

//******************************************************************************

void GLVBNormalsBuffer :: Activate()
{
	glEnableClientState( GL_NORMAL_ARRAY);
	CheckGLError( "GLVBNormalsBuffer :: Activate - glEnableClientState");
	glNormalPointer( GL_REAL, 0, m_buffer);
	CheckGLError( "GLVBNormalsBuffer :: Activate - glNormalPointer");
}

void GLVBNormalsBuffer :: Deactivate()
{
	glDisableClientState( GL_NORMAL_ARRAY);
	CheckGLError( "GLVBNormalsBuffer :: Deactivate - glDisableClientState");
}

//******************************************************************************

void GLVBTextureBuffer :: Activate( PassPtr p_pass)
{
	unsigned int l_nbUnits = p_pass->GetNbTexUnits();

	if (l_nbUnits > 0)
	{
		glEnableClientState( GL_TEXTURE_COORD_ARRAY);
		CheckGLError( "GLVBTextureBuffer :: Activate - glEnableClientState");
		if (RenderSystem::UseMultiTexturing())
		{
			for (unsigned int j = 0 ; j < l_nbUnits ; j++)
			{
				glClientActiveTexture( GL_TEXTURE0 + j);
				CheckGLError( "GLVBTextureBuffer :: Activate - glClientActiveTexture");
				glTexCoordPointer( 2, GL_FLOAT, 0, m_buffer);
				CheckGLError( "GLVBTextureBuffer :: Activate - glTexCoordPointer");
			}
		}
		else
		{
			glTexCoordPointer( 2, GL_REAL, 0, m_buffer);
			CheckGLError( "GLVBTextureBuffer :: Activate - glTexCoordPointer");
		}
	}
}

void GLVBTextureBuffer :: Deactivate()
{
	glDisableClientState( GL_TEXTURE_COORD_ARRAY);
	CheckGLError( "GLVBTextureBuffer :: Deactivate - glDisableClientState");
}

//******************************************************************************

GLVBOVertexBuffer :: GLVBOVertexBuffer()
	:	GLVertexAttribs<real, 3>( "vertex"),
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
	if (GLRenderSystem::UseShaders())
	{
		CleanupAttribs();
	}
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
		if (GLRenderSystem::UseShaders())
		{
			CleanupAttribs();
		}

		CleanupBufferObject();
		m_pProgram = l_pProgram;
		Initialise();
	}
}

void GLVBOVertexBuffer :: Initialise()
{
	if ( ! GLRenderSystem::UseShaders() || m_pProgram == NULL || InitialiseAttribs())
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
		if (GLRenderSystem::UseShaders() && m_pProgram != NULL)
		{
			ActivateBufferObject();
			ActivateAttribs();
		}
		else
		{
			glBindBuffer( GL_ARRAY_BUFFER, m_uiIndex);
			CheckGLError( "GLVBOVertexBuffer :: Activate - glBindBuffer");
			glEnableClientState( GL_VERTEX_ARRAY);
			CheckGLError( "GLVBOVertexBuffer :: Activate - glEnableClientState");
			glVertexPointer( 3, GL_REAL, 0, NULL);
			CheckGLError( "GLVBOVertexBuffer :: Activate - glVertexPointer");
		}
	}
}

void GLVBOVertexBuffer :: Deactivate()
{
	if (m_bAssigned)
	{
		if (GLRenderSystem::UseShaders() && m_pProgram != NULL)
		{
			DeactivateAttribs();
		}
		else
		{
			glDisableClientState( GL_VERTEX_ARRAY);
			CheckGLError( "GLVBOVertexBuffer :: Deactivate - glDisableClientState");
		}
	}
}

//******************************************************************************

GLVBONormalsBuffer :: GLVBONormalsBuffer()
	:	GLVertexAttribs<real, 3>( "normal"),
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
	if (GLRenderSystem::UseShaders())
	{
		CleanupAttribs();
	}
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
		if (GLRenderSystem::UseShaders())
		{
			CleanupAttribs();
		}

		CleanupBufferObject();
		m_pProgram = l_pProgram;
		Initialise();
	}
}

void GLVBONormalsBuffer :: Initialise()
{
	if ( ! GLRenderSystem::UseShaders() || m_pProgram == NULL || InitialiseAttribs())
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
		if (GLRenderSystem::UseShaders() && m_pProgram != NULL)
		{
			ActivateBufferObject();
			ActivateAttribs();
		}
		else
		{
			glBindBuffer( GL_ARRAY_BUFFER, m_uiIndex);
			CheckGLError( "GLVBONormalsBuffer :: Activate - glBindBuffer");
			glEnableClientState( GL_NORMAL_ARRAY);
			CheckGLError( "GLVBONormalsBuffer :: Activate - glEnableClientState");
			glNormalPointer( GL_REAL, 0, NULL);
			CheckGLError( "GLVBONormalsBuffer :: Activate - glNormalPointer");
		}
	}
}

void GLVBONormalsBuffer :: Deactivate()
{
	if (m_bAssigned)
	{
		if (GLRenderSystem::UseShaders() && m_pProgram != NULL)
		{
			DeactivateAttribs();
		}
		else
		{
			glDisableClientState( GL_NORMAL_ARRAY);
			CheckGLError( "GLVBONormalsBuffer :: Deactivate - glDisableClientState");
		}
	}
}

//******************************************************************************

GLVBOTextureBuffer :: GLVBOTextureBuffer()
	:	TextureBuffer(),
		m_index( GL_INVALID_INDEX)
{
}

GLVBOTextureBuffer :: ~GLVBOTextureBuffer()
{
	Cleanup();
}

void GLVBOTextureBuffer :: Cleanup()
{
	for (unsigned int i = 0 ; i < m_arrayBuffers.size() ; i++)
	{
		m_arrayBuffers[i]->Cleanup();
	}

	if (m_index != GL_INVALID_INDEX && glIsBuffer( m_index))
	{
		glDeleteBuffers( 1, & m_index);
	}

	TextureBuffer::Cleanup();
	m_index = GL_INVALID_INDEX;

	for (unsigned int i = 0 ; i < m_arrayBuffers.size() ; i++)
	{
		m_arrayBuffers[i]->Cleanup();
	}
}

void GLVBOTextureBuffer :: Initialise( PassPtr p_pass)
{
	if ( ! GLRenderSystem::UseShaders() || p_pass.null() || p_pass->GetShader().null())
	{
		glGenBuffers( 1, & m_index);
		CheckGLError( "GLVBOTextureBuffer :: Initialise - glGenBuffers");
		glBindBuffer( GL_ARRAY_BUFFER, m_index);
		CheckGLError( "GLVBOTextureBuffer :: Initialise - glBindBuffer");
		glBufferData( GL_ARRAY_BUFFER, m_filled * sizeof( real), m_buffer, GL_STATIC_DRAW);
		CheckGLError( "GLVBOTextureBuffer :: Initialise - glBufferData");
		m_bAssigned = (m_index != GL_INVALID_INDEX);
		BufferManager::AssignBuffer<real>( m_index, this);
	}
	else
	{
		unsigned int l_nbUnits = p_pass->GetNbTexUnits();
		String l_strBase = "MultiTexCoord";
		GLVertexAttribsBuffer2rPtr l_pBuffer;

		for (unsigned int i = 0 ; i < l_nbUnits ; i++)
		{
			String l_strName = l_strBase;
			l_strName << i;
			l_pBuffer = BufferManager::CreateBuffer<real, GLVertexAttribsBuffer2r>( l_strName);
			m_arrayBuffers.push_back( l_pBuffer);
			l_pBuffer->InitialiseBuffer( * this);
			l_pBuffer->Initialise();
		}
	}
}

void GLVBOTextureBuffer :: Activate( PassPtr p_pass)
{
	unsigned int l_nbUnits = p_pass->GetNbTexUnits();

	if (l_nbUnits > 0)
	{
		if ( ! GLRenderSystem::UseShaders() || p_pass.null() || p_pass->GetShader().null())
		{
			glBindBuffer( GL_ARRAY_BUFFER, m_index);
			CheckGLError( "GLVBOTextureBuffer :: Activate - glBindBuffer");
			glEnableClientState( GL_TEXTURE_COORD_ARRAY);
			CheckGLError( "GLVBOTextureBuffer :: Activate - glEnableClientState");

			if (RenderSystem::UseMultiTexturing())
			{
				for (unsigned int j = 0 ; j < l_nbUnits ; j++)
				{
					glClientActiveTexture( GL_TEXTURE0 + j);
					CheckGLError( "GLVBOTextureBuffer :: Activate - glClientActiveTexture");
					glTexCoordPointer( 2, GL_REAL, 0, NULL);
					CheckGLError( "GLVBOTextureBuffer :: Activate - glTexCoordPointer");
				}
			}
			else
			{
				glTexCoordPointer( 2, GL_REAL, 0, NULL);
				CheckGLError( "GLGeometryRenderer :: Activate - glTexCoordPointer");
			}
		}
		else
		{
			for (unsigned int i = 0 ; i < m_arrayBuffers.size() ; i++)
			{
				m_arrayBuffers[i]->Activate();
			}
		}
	}
}

void GLVBOTextureBuffer :: Deactivate()
{
	glDisableClientState( GL_TEXTURE_COORD_ARRAY);
	CheckGLError( "GLVBOTextureBuffer :: Deactivate - glDisableClientState");
}

void GLVBOTextureBuffer :: SetShaderProgram( ShaderProgram * p_pProgram)
{
	for (unsigned int i = 0 ; i < m_arrayBuffers.size() ; i++)
	{
		m_arrayBuffers[i]->SetShaderProgram( p_pProgram);
	}
}

//******************************************************************************