#include "GL2RenderSystem/PrecompiledHeader.h"

#include "GL2RenderSystem/GL2Buffer.h"
#include "GL2RenderSystem/GL2RenderSystem.h"

using namespace Castor3D;

//******************************************************************************

void GLVBIndicesBuffer :: Activate()
{
}

void GLVBIndicesBuffer :: Deactivate()
{
}

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

void GLVBOTextureBuffer :: Initialise( PassPtr p_pPass)
{
	if ( ! RenderSystem::UseShaders() || p_pPass == NULL || ! p_pPass->HasShader())
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
		unsigned int l_nbUnits = p_pPass->GetNbTexUnits();
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

void GLVBOTextureBuffer :: Activate( PassPtr p_pPass)
{
	unsigned int l_nbUnits = p_pPass->GetNbTexUnits();

	if (l_nbUnits > 0)
	{
		if ( ! RenderSystem::UseShaders() || p_pPass == NULL || ! p_pPass->HasShader())
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

void GLVBOTextureBuffer :: SetShaderProgram( ShaderProgramPtr p_pProgram)
{
	for (unsigned int i = 0 ; i < m_arrayBuffers.size() ; i++)
	{
		m_arrayBuffers[i]->SetShaderProgram( p_pProgram);
	}
}

//******************************************************************************