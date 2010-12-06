#include "PrecompiledHeader.h"

#include "GL2Buffer.h"
#include "GL2RenderSystem.h"

using namespace Castor3D;

//******************************************************************************

void GLVBVertexBuffer :: Activate()
{
	glEnableClientState( GL_VERTEX_ARRAY);
	CheckGLError( CU_T( "GLVBVertexBuffer :: Activate - glEnableClientState"));
	glVertexPointer( 3, GL_REAL, 0, m_buffer);
	CheckGLError( CU_T( "GLVBVertexBuffer :: Activate - glVertexPointer"));
}

void GLVBVertexBuffer :: Deactivate()
{
	glDisableClientState( GL_VERTEX_ARRAY);
	CheckGLError( CU_T( "GLVBVertexBuffer :: Deactivate - glDisableClientState"));
}

//******************************************************************************

void GLVBNormalsBuffer :: Activate()
{
	glEnableClientState( GL_NORMAL_ARRAY);
	CheckGLError( CU_T( "GLVBNormalsBuffer :: Activate - glEnableClientState"));
	glNormalPointer( GL_REAL, 0, m_buffer);
	CheckGLError( CU_T( "GLVBNormalsBuffer :: Activate - glNormalPointer"));
}

void GLVBNormalsBuffer :: Deactivate()
{
	glDisableClientState( GL_NORMAL_ARRAY);
	CheckGLError( CU_T( "GLVBNormalsBuffer :: Deactivate - glDisableClientState"));
}

//******************************************************************************

void GLVBTextureBuffer :: Activate( PassPtr p_pass)
{
	unsigned int l_nbUnits = p_pass->GetNbTexUnits();

	if (l_nbUnits > 0)
	{
		glEnableClientState( GL_TEXTURE_COORD_ARRAY);
		CheckGLError( CU_T( "GLVBTextureBuffer :: Activate - glEnableClientState"));
		if (RenderSystem::UseMultiTexturing())
		{
			for (unsigned int j = 0 ; j < l_nbUnits ; j++)
			{
				glClientActiveTexture( GL_TEXTURE0 + j);
				CheckGLError( CU_T( "GLVBTextureBuffer :: Activate - glClientActiveTexture"));
				glTexCoordPointer( 2, GL_REAL, 0, m_buffer);
				CheckGLError( CU_T( "GLVBTextureBuffer :: Activate - glTexCoordPointer"));
			}
		}
		else
		{
			glTexCoordPointer( 2, GL_REAL, 0, m_buffer);
			CheckGLError( CU_T( "GLVBTextureBuffer :: Activate - glTexCoordPointer"));
		}
	}
}

void GLVBTextureBuffer :: Deactivate()
{
	glDisableClientState( GL_TEXTURE_COORD_ARRAY);
	CheckGLError( CU_T( "GLVBTextureBuffer :: Deactivate - glDisableClientState"));
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
	if ( ! RenderSystem::UseShaders() || p_pass.null() || p_pass->GetShader().null())
	{
		glGenBuffers( 1, & m_index);
		CheckGLError( CU_T( "GLVBOTextureBuffer :: Initialise - glGenBuffers"));
		glBindBuffer( GL_ARRAY_BUFFER, m_index);
		CheckGLError( CU_T( "GLVBOTextureBuffer :: Initialise - glBindBuffer"));
		glBufferData( GL_ARRAY_BUFFER, m_filled * sizeof( real), m_buffer, GL_STREAM_DRAW);
		CheckGLError( CU_T( "GLVBOTextureBuffer :: Initialise - glBufferData"));
		m_bAssigned = (m_index != GL_INVALID_INDEX);
		BufferManager::AssignBuffer<real>( m_index, this);
	}
	else
	{
		unsigned int l_nbUnits = p_pass->GetNbTexUnits();
		String l_strBase = "texture";
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
		if ( ! RenderSystem::UseShaders() || p_pass.null() || p_pass->GetShader().null())
		{
			glBindBuffer( GL_ARRAY_BUFFER, m_index);
			CheckGLError( CU_T( "GLVBOTextureBuffer :: Activate - glBindBuffer"));
			glEnableClientState( GL_TEXTURE_COORD_ARRAY);
			CheckGLError( CU_T( "GLVBOTextureBuffer :: Activate - glEnableClientState"));

			if (RenderSystem::UseMultiTexturing())
			{
				for (unsigned int j = 0 ; j < l_nbUnits ; j++)
				{
					glClientActiveTexture( GL_TEXTURE0 + j);
					CheckGLError( CU_T( "GLVBOTextureBuffer :: Activate - glClientActiveTexture"));
					glTexCoordPointer( 2, GL_REAL, 0, NULL);
					CheckGLError( CU_T( "GLVBOTextureBuffer :: Activate - glTexCoordPointer"));
				}
			}
			else
			{
				glTexCoordPointer( 2, GL_REAL, 0, NULL);
				CheckGLError( CU_T( "GLGeometryRenderer :: Activate - glTexCoordPointer"));
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
	CheckGLError( CU_T( "GLVBOTextureBuffer :: Deactivate - glDisableClientState"));
}

void GLVBOTextureBuffer :: SetShaderProgram( ShaderProgram * p_pProgram)
{
	for (unsigned int i = 0 ; i < m_arrayBuffers.size() ; i++)
	{
		m_arrayBuffers[i]->SetShaderProgram( p_pProgram);
	}
}

//******************************************************************************

GLTextureBufferObject :: GLTextureBufferObject()
	:	TextureBufferObject(),
		m_uiIndex( GL_INVALID_INDEX)
{
}

GLTextureBufferObject :: ~GLTextureBufferObject()
{
	Cleanup();
}

void GLTextureBufferObject :: Cleanup()
{
	if (m_uiIndex != GL_INVALID_INDEX && glIsBuffer( m_uiIndex))
	{
		BufferManager::UnassignBuffer<unsigned char>( m_uiIndex, this);
		glDeleteBuffers( 1, & m_uiIndex);
		CheckGLError( CU_T( "GLTextureBufferObject :: Cleanup - glDeleteBuffers"));
	}

	m_uiIndex = GL_INVALID_INDEX;
}

void GLTextureBufferObject :: Initialise( const Resource::PixelFormat & p_format, size_t p_uiSize, const unsigned char * p_pBytes)
{
	TextureBufferObject::Initialise( p_format, p_uiSize, p_pBytes);

	glGenBuffers( 1, & m_uiIndex);
	CheckGLError( CU_T( "GLTextureBufferObject :: Initialise - glGenBuffers"));

	if (m_uiIndex != GL_INVALID_INDEX && glIsBuffer( m_uiIndex))
	{
		glBindBuffer( GL_TEXTURE_BUFFER, m_uiIndex);
		CheckGLError( CU_T( "GLTextureBufferObject :: Initialise - glBindBuffer"));
		glBufferData( GL_TEXTURE_BUFFER, m_uiSize * GetPixelFormatBits( m_pixelFormat) / 8, m_pBytes, GL_STREAM_DRAW);
		CheckGLError( CU_T( "GLTextureBufferObject :: Initialise - glBufferData"));
		glBindBuffer( GL_TEXTURE_BUFFER, 0);
		CheckGLError( CU_T( "GLTextureBufferObject :: Initialise - glBindBuffer"));
		BufferManager::AssignBuffer<unsigned char>( m_uiIndex, this);
	}
	else
	{
		m_uiIndex = GL_INVALID_INDEX;
	}
}

//******************************************************************************