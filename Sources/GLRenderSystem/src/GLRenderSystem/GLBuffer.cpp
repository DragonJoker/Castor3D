//******************************************************************************
#include "PrecompiledHeader.h"

#include "../../include/GLRenderSystem/GLBuffer.h"
#include "../../include/GLRenderSystem/Module_GL.h"
#include "../../include/GLRenderSystem/GLRenderSystem.h"
//******************************************************************************
using namespace Castor3D;
//******************************************************************************

void GLVBVertexBuffer :: Activate()
{
	glEnableClientState( GL_VERTEX_ARRAY);
	CheckGLError( "GLVBVertexBuffer :: Activate - glEnableClientState");
	glVertexPointer( 3, GL_FLOAT, 0, m_buffer);
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
	glNormalPointer( GL_FLOAT, 0, m_buffer);
	CheckGLError( "GLVBNormalsBuffer :: Activate - glNormalPointer");
}

void GLVBNormalsBuffer :: Deactivate()
{
	glDisableClientState( GL_NORMAL_ARRAY);
	CheckGLError( "GLVBNormalsBuffer :: Deactivate - glDisableClientState");
}

//******************************************************************************

void GLVBVertexAttribsBuffer :: Activate()
{
	glEnableVertexAttribArray( m_iAttribLocation);
	CheckGLError( "GLVBVertexAttribsBuffer :: Activate - glEnableVertexAttribArray");
	glVertexAttribPointer( m_iAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, m_buffer);
	CheckGLError( "GLVBVertexAttribsBuffer :: Activate - glVertexAttribPointer");
}

void GLVBVertexAttribsBuffer :: Deactivate()
{
	glDisableVertexAttribArray( m_iAttribLocation);
	CheckGLError( "GLVBVertexAttribsBuffer :: Deactivate - glDisableVertexAttribArray");
}

//******************************************************************************

void GLVBTextureBuffer :: Activate( Pass * p_pass)
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
			glTexCoordPointer( 2, GL_FLOAT, 0, m_buffer);
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

void GLVBOVertexBuffer :: Initialise()
{
	glGenBuffers( 1, & m_index);
	CheckGLError( "GLVBOBuffer :: Initialise - glGenBuffers");
	glBindBuffer( GL_ARRAY_BUFFER, m_index);
	CheckGLError( "GLVBOBuffer :: Initialise - glBindBuffer");
	glBufferData( GL_ARRAY_BUFFER, m_filled * sizeof( float), m_buffer, GL_STREAM_DRAW);
	CheckGLError( "GLVBOBuffer :: Initialise - glBufferData");
	m_bAssigned = (m_index > 0);
	BufferManager::GetSingleton().AssignBuffer( m_index, this);
}

void GLVBOVertexBuffer :: Cleanup()
{
	if (m_index > 0)
	{
		glDeleteBuffers( 1, & m_index);
	}

	Buffer::Cleanup();
	m_index = 0;
}

void GLVBOVertexBuffer :: Activate()
{
	glBindBuffer( GL_ARRAY_BUFFER, m_index);
	CheckGLError( "GLVBOVertexBuffer :: Activate - glBindBuffer");
	glEnableClientState( GL_VERTEX_ARRAY);
	CheckGLError( "GLVBOVertexBuffer :: Activate - glEnableClientState");
	glVertexPointer( 3, GL_FLOAT, 0, 0);
	CheckGLError( "GLVBOVertexBuffer :: Activate - glVertexPointer");
}

void GLVBOVertexBuffer :: Deactivate()
{
	glDisableClientState( GL_VERTEX_ARRAY);
	CheckGLError( "GLVBOVertexBuffer :: Deactivate - glDisableClientState");
}

//******************************************************************************

void GLVBONormalsBuffer :: Initialise()
{
	glGenBuffers( 1, & m_index);
	CheckGLError( "GLVBOBuffer :: Initialise - glGenBuffers");
	glBindBuffer( GL_ARRAY_BUFFER, m_index);
	CheckGLError( "GLVBOBuffer :: Initialise - glBindBuffer");
	glBufferData( GL_ARRAY_BUFFER, m_filled * sizeof( float), m_buffer, GL_STREAM_DRAW);
	CheckGLError( "GLVBOBuffer :: Initialise - glBufferData");
	m_bAssigned = (m_index > 0);
	BufferManager::GetSingleton().AssignBuffer( m_index, this);
}

void GLVBONormalsBuffer :: Cleanup()
{
	if (m_index > 0)
	{
		glDeleteBuffers( 1, & m_index);
	}

	Buffer::Cleanup();
	m_index = 0;
}

void GLVBONormalsBuffer :: Activate()
{
	glBindBuffer( GL_ARRAY_BUFFER, m_index);
	CheckGLError( "GLVBONormalsBuffer :: Activate - glBindBuffer");
	glEnableClientState( GL_NORMAL_ARRAY);
	CheckGLError( "GLVBONormalsBuffer :: Activate - glEnableClientState");
	glNormalPointer( GL_FLOAT, 0, 0);
	CheckGLError( "GLVBONormalsBuffer :: Activate - glNormalPointer");
}

void GLVBONormalsBuffer :: Deactivate()
{
	glDisableClientState( GL_NORMAL_ARRAY);
	CheckGLError( "GLVBONormalsBuffer :: Deactivate - glDisableClientState");
}

//******************************************************************************

void GLVBOVertexAttribsBuffer :: Activate()
{
	glBindBuffer( GL_ARRAY_BUFFER, m_index);
	CheckGLError( "GLVBOVertexAttribsBuffer :: Activate - glBindBuffer");
	glEnableVertexAttribArray( m_iAttribLocation);
	CheckGLError( "GLVBOVertexAttribsBuffer :: Activate - glEnableVertexAttribArray");
	glVertexAttribPointer( m_iAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, m_buffer);
	CheckGLError( "GLVBOVertexAttribsBuffer :: Activate - glVertexAttribPointer");
}

void GLVBOVertexAttribsBuffer :: Deactivate()
{
	glDisableVertexAttribArray( m_iAttribLocation);
	CheckGLError( "GLVBOVertexAttribsBuffer :: Deactivate - glDisableVertexAttribArray");
}

//******************************************************************************

void GLVBOTextureBuffer :: Initialise()
{
	glGenBuffers( 1, & m_index);
	CheckGLError( "GLVBOTextureBuffer :: Initialise - glGenBuffers");
	glBindBuffer( GL_ARRAY_BUFFER, m_index);
	CheckGLError( "GLVBOTextureBuffer :: Initialise - glBindBuffer");
	glBufferData( GL_ARRAY_BUFFER, m_filled * sizeof( float), m_buffer, GL_STREAM_DRAW);
	CheckGLError( "GLVBOTextureBuffer :: Initialise - glBufferData");
	m_bAssigned = (m_index > 0);
	BufferManager::GetSingleton().AssignBuffer( m_index, this);
}

void GLVBOTextureBuffer :: Cleanup()
{
	if (m_index > 0)
	{
		glDeleteBuffers( 1, & m_index);
	}

	TextureBuffer::Cleanup();
	m_index = 0;
}

void GLVBOTextureBuffer :: Activate( Pass * p_pass)
{
	unsigned int l_nbUnits = p_pass->GetNbTexUnits();

	if (l_nbUnits > 0)
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
				glTexCoordPointer( 2, GL_FLOAT, 0, 0);
				CheckGLError( "GLVBOTextureBuffer :: Activate - glTexCoordPointer");
			}
		}
		else
		{
			glTexCoordPointer( 2, GL_FLOAT, 0, 0);
			CheckGLError( "GLGeometryRenderer :: Activate - glTexCoordPointer");
		}
	}
}

void GLVBOTextureBuffer :: Deactivate()
{
	glDisableClientState( GL_TEXTURE_COORD_ARRAY);
	CheckGLError( "GLVBOTextureBuffer :: Deactivate - glDisableClientState");
}

//******************************************************************************