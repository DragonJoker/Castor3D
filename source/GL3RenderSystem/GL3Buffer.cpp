#include "GL3RenderSystem/PrecompiledHeader.h"

#include "GL3RenderSystem/GL3Buffer.h"
#include "GL3RenderSystem/GL3RenderSystem.h"
#include "GL3RenderSystem/GL3ShaderProgram.h"

using namespace Castor3D;
using namespace Castor::Resources;

//******************************************************************************

GLVBOTextureBuffer :: GLVBOTextureBuffer()
{
}

GLVBOTextureBuffer :: ~GLVBOTextureBuffer()
{
	Cleanup();
}

void GLVBOTextureBuffer :: CleanupBufferObject()
{
	for (unsigned int i = 0 ; i < m_arrayBuffers.size() ; i++)
	{
		m_arrayBuffers[i]->CleanupBufferObject();
	}
}

void GLVBOTextureBuffer :: CleanupAttribute()
{
	for (unsigned int i = 0 ; i < m_arrayBuffers.size() ; i++)
	{
		m_arrayBuffers[i]->CleanupAttribute();
	}
}

void GLVBOTextureBuffer :: Cleanup()
{
	TextureBuffer::Cleanup();

	for (unsigned int i = 0 ; i < m_arrayBuffers.size() ; i++)
	{
		m_arrayBuffers[i]->Cleanup();
	}
}

void GLVBOTextureBuffer :: Initialise( PassPtr p_pass)
{
	for (unsigned int i = 0 ; i < m_arrayBuffers.size() ; i++)
	{
		m_arrayBuffers[i]->Cleanup();
	}

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

void GLVBOTextureBuffer :: Activate( PassPtr p_pass)
{
	unsigned int l_nbUnits = p_pass->GetNbTexUnits();

	if (l_nbUnits > 0)
	{
		for (unsigned int i = 0 ; i < m_arrayBuffers.size() ; i++)
		{
			m_arrayBuffers[i]->Activate();
		}
	}
}

void GLVBOTextureBuffer :: Deactivate()
{
	for (unsigned int i = 0 ; i < m_arrayBuffers.size() ; i++)
	{
		m_arrayBuffers[i]->Deactivate();
	}
}

void GLVBOTextureBuffer :: SetShaderProgram( ShaderProgramPtr p_pProgram)
{
	for (unsigned int i = 0 ; i < m_arrayBuffers.size() ; i++)
	{
		m_arrayBuffers[i]->SetShaderProgram( p_pProgram);
	}
}

//******************************************************************************

GLUBOVariableBase :: GLUBOVariableBase( GLUniformBufferObject * p_pParent, const String & p_strName, size_t p_uiCount)
	:	m_uiIndex( GL_INVALID_INDEX),
		m_pBuffer( NULL),
		m_strName( p_strName),
		m_uiCount( p_uiCount),
		m_pParent( p_pParent)
{
}

GLUBOVariableBase :: ~GLUBOVariableBase()
{
}

void GLUBOVariableBase :: Activate()
{
	glBufferSubData( GL_UNIFORM_BUFFER_EXT, m_iOffset, m_iSize, m_pBuffer);
	CheckGLError( CU_T( "UBOVariable :: Activate - glBufferSubData - ") + m_strName);
}

void GLUBOVariableBase :: Deactivate()
{
}

//******************************************************************************

GLUniformBufferObject :: GLUniformBufferObject( const String & p_strUniformBlockName)
	:	Buffer3D(),
		m_uiIndex( GL_INVALID_INDEX),
		m_pProgram( NULL),
		m_strUniformBlockName( p_strUniformBlockName),
		m_iUniformBlockIndex( GL_INVALID_INDEX),
		m_bChanged( true)
{
}

GLUniformBufferObject :: ~GLUniformBufferObject()
{
	Cleanup();
}

void GLUniformBufferObject :: Cleanup()
{
	if (m_uiIndex != GL_INVALID_INDEX)
	{
		BufferManager::UnassignBuffer<unsigned char>( m_uiIndex, this);

		if (glIsBuffer( m_uiIndex))
		{
			glDeleteBuffers( 1, & m_uiIndex);
			m_uiIndex = GL_INVALID_INDEX;
		}
	}

	Buffer3D::Cleanup();
}

void GLUniformBufferObject :: Initialise( GLShaderProgram * p_pProgram)
{
	m_pProgram = p_pProgram;

	if (m_pProgram != NULL && m_pProgram->GetProgramObject() > 0)
	{
		if (m_uiIndex == GL_INVALID_INDEX)
		{
			glGenBuffers( 1, & m_uiIndex);
			CheckGLError( CU_T( "GLUniformBufferObject :: Initialise - ") + m_strUniformBlockName + CU_T( " - glGenBuffers"));
		}

		if (m_iUniformBlockIndex == GL_INVALID_INDEX && m_uiIndex != GL_INVALID_INDEX)
		{
			BufferManager::AssignBuffer<unsigned char>( m_uiIndex, this);

			m_iUniformBlockIndex = glGetUniformBlockIndex( m_pProgram->GetProgramObject(), m_strUniformBlockName.char_str());
			CheckGLError( CU_T( "GLUniformBufferObject :: Initialise - ") + m_strUniformBlockName + CU_T( " - glGetUniformBlockIndex"));

			if (m_iUniformBlockIndex != GL_INVALID_INDEX)
			{
				glGetActiveUniformBlockiv( m_pProgram->GetProgramObject(), m_iUniformBlockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, & m_iUniformBlockSize);
				CheckGLError( CU_T( "GLUniformBufferObject :: Initialise - ") + m_strUniformBlockName + CU_T( " - glGetActiveUniformBlockiv"));
				InitialiseBuffer( m_iUniformBlockSize, 0);
				glBindBuffer( GL_UNIFORM_BUFFER, m_uiIndex);
				CheckGLError( CU_T( "GLUniformBufferObject :: Initialise - ") + m_strUniformBlockName + CU_T( " - glBindBuffer"));
				glBufferData( GL_UNIFORM_BUFFER, m_iUniformBlockSize, NULL, GL_STREAM_DRAW);
				CheckGLError( CU_T( "GLUniformBufferObject :: Initialise - ") + m_strUniformBlockName + CU_T( " - glBufferData"));
				glBindBufferBase (GL_UNIFORM_BUFFER, 0, m_uiIndex);
				CheckGLError( CU_T( "GLUniformBufferObject :: Initialise - ") + m_strUniformBlockName + CU_T( " - glBindBufferBase"));
				glUniformBlockBinding( m_pProgram->GetProgramObject(), m_iUniformBlockIndex, 0);
				CheckGLError( CU_T( "GLUniformBufferObject :: Initialise - ") + m_strUniformBlockName + CU_T( " - glUniformBlockBinding"));
			}
		}
	}
}

void GLUniformBufferObject :: Activate()
{
	if (m_iUniformBlockIndex != GL_INVALID_INDEX && m_bChanged)
	{
		glBindBuffer( GL_UNIFORM_BUFFER, m_uiIndex);
		CheckGLError( CU_T( "GLUniformBufferObject :: Activate - ") + m_strUniformBlockName + CU_T( " - glBindBuffer"));

		for (GLUBOVariablePtrStrMap::iterator l_it = m_mapVariables.begin() ; l_it != m_mapVariables.end() ; ++l_it)
		{
			l_it->second->Activate();
		}
	}

	m_bChanged = false;
}

void GLUniformBufferObject :: Deactivate()
{
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	CheckGLError( CU_T( "GLUniformBufferObject :: Deactivate - ") + m_strUniformBlockName + CU_T( " - glBindBuffer"));
}

bool GLUniformBufferObject :: AddVariable( GLUBOVariablePtr p_pVariable)
{
	bool l_bReturn = false;
	const char * l_pChar = p_pVariable->m_strName.char_str();
	glGetUniformIndices( m_pProgram->GetProgramObject(), 1, & l_pChar, & (p_pVariable->m_uiIndex));
	CheckGLError( CU_T( "GLUniformBufferObject :: GetVariable - ") + m_strUniformBlockName + CU_T( " - glGetUniformIndices - " + p_pVariable->m_strName));

	if (p_pVariable->m_uiIndex != GL_INVALID_INDEX)
	{
		glGetActiveUniformsiv( m_pProgram->GetProgramObject(), 1, & p_pVariable->m_uiIndex, GL_UNIFORM_OFFSET, & (p_pVariable->m_iOffset));
		CheckGLError( CU_T( "GLUniformBufferObject :: GetVariable - ") + m_strUniformBlockName + CU_T( " - glGetActiveUniformsiv GL_UNIFORM_OFFSET - " + p_pVariable->m_strName));
		glGetActiveUniformsiv( m_pProgram->GetProgramObject(), 1, & p_pVariable->m_uiIndex, GL_UNIFORM_SIZE, & (p_pVariable->m_iSize));
		CheckGLError( CU_T( "GLUniformBufferObject :: GetVariable - ") + m_strUniformBlockName + CU_T( " - glGetActiveUniformsiv GL_UNIFORM_SIZE - " + p_pVariable->m_strName));

		if (p_pVariable->m_iSize == 1)
		{
			p_pVariable->m_iSize = p_pVariable->GetSize();
		}

		if (p_pVariable->GetSize() + p_pVariable->m_iOffset <= m_totalSize)
		{
			m_mapVariables.insert( GLUBOVariablePtrStrMap::value_type( p_pVariable->m_strName, p_pVariable));
			l_bReturn = true;
			m_bChanged = true;
		}
	}

	return l_bReturn;
}

//******************************************************************************

GLVertexArrayObjects :: GLVertexArrayObjects()
	:	m_uiIndex( GL_INVALID_INDEX)
{
}

GLVertexArrayObjects :: ~GLVertexArrayObjects()
{
	Cleanup();
}

void GLVertexArrayObjects :: Cleanup()
{
	if (m_uiIndex != GL_INVALID_INDEX)
	{
		glDeleteVertexArrays( 1, & m_uiIndex);
		CheckGLError( CU_T( "GLVertexArrayObjects :: Cleanup - glDeleteVertexArrays"));
		m_uiIndex = GL_INVALID_INDEX;
	}
}

void GLVertexArrayObjects :: Initialise()
{
	if (m_uiIndex == GL_INVALID_INDEX)
	{
		glGenVertexArrays( 1, & m_uiIndex);
		CheckGLError( CU_T( "GLVertexArrayObjects :: Initialise - glGenVertexArrays"));
	}
}

void GLVertexArrayObjects :: Activate()
{
	if (m_uiIndex != GL_INVALID_INDEX)
	{
		glBindVertexArray( m_uiIndex);
		CheckGLError( CU_T( "GLVertexArrayObjects :: Activate - glBindVertexArray"));
	}
}

void GLVertexArrayObjects :: Deactivate()
{
	glBindVertexArray( 0);
	CheckGLError( CU_T( "GLVertexArrayObjects :: Deactivate - glBindVertexArray( 0)"));
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

void GLTextureBufferObject :: Initialise( const PixelFormat & p_format, size_t p_uiSize, const unsigned char * p_pBytes)
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