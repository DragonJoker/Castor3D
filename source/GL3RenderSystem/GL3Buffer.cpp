#include "Gl3RenderSystem/PrecompiledHeader.h"

#include "Gl3RenderSystem/Gl3Buffer.h"
#include "Gl3RenderSystem/Gl3RenderSystem.h"
#include "Gl3RenderSystem/Gl3ShaderProgram.h"

using namespace Castor3D;
using namespace Castor::Resources;

//******************************************************************************

Gl3VboVertexBuffer :: Gl3VboVertexBuffer( const BufferElementDeclaration * p_pElements, size_t p_uiNbElements)
	:	GlVboVertexBuffer( p_pElements, p_uiNbElements)
{
}

Gl3VboVertexBuffer :: ~Gl3VboVertexBuffer()
{
	Cleanup();
}

void Gl3VboVertexBuffer :: Activate()
{
	if (m_bAssigned)
	{
		_activateBufferObject();
		_activateAttribute();
	}
}

void Gl3VboVertexBuffer :: Deactivate()
{
	if (m_bAssigned)
	{
		_deactivateAttribute();
	}
}

void Gl3VboVertexBuffer :: _activateBufferObject()
{
	CASTOR_ASSERT( ! m_pProgram.expired());
	CheckGlError( glBindBuffer( GL_ARRAY_BUFFER, m_uiIndex), CU_T( "GlVboVertexBuffer :: Activate - glBindBuffer"));
}

//******************************************************************************

GlUboVariableBase :: GlUboVariableBase( GlUniformBufferObject * p_pParent, const String & p_strName, size_t p_uiCount)
	:	m_uiIndex( GL_INVALID_INDEX),
		m_pBuffer( NULL),
		m_strName( p_strName),
		m_uiCount( p_uiCount),
		m_pParent( p_pParent)
{
}

GlUboVariableBase :: ~GlUboVariableBase()
{
}

void GlUboVariableBase :: Activate()
{
	CheckGlError( glBufferSubData( GL_UNIFORM_BUFFER_EXT, m_iOffset, m_iSize, m_pBuffer), CU_T( "UBOVariable :: Activate - glBufferSubData - ") + m_strName);
}

void GlUboVariableBase :: Deactivate()
{
}

//******************************************************************************

GlUniformBufferObject :: GlUniformBufferObject( const String & p_strUniformBlockName)
	:	Buffer3D<unsigned char>(),
		m_uiIndex( GL_INVALID_INDEX),
		m_pProgram( NULL),
		m_strUniformBlockName( p_strUniformBlockName),
		m_iUniformBlockIndex( GL_INVALID_INDEX),
		m_bChanged( true)
{
}

GlUniformBufferObject :: ~GlUniformBufferObject()
{
	Cleanup();
}

void GlUniformBufferObject :: Cleanup()
{
	if (m_uiIndex != GL_INVALID_INDEX)
	{
		BufferManager::UnassignBuffer<unsigned char>( m_uiIndex, this);

//		if (glIsBuffer( m_uiIndex))
		{
			glDeleteBuffers( 1, & m_uiIndex);
			m_uiIndex = GL_INVALID_INDEX;
		}
	}

	Buffer3D<unsigned char>::Cleanup();
}

void GlUniformBufferObject :: Initialise( GlShaderProgram * p_pProgram)
{
	m_pProgram = p_pProgram;

	if (m_pProgram != NULL && m_pProgram->GetProgramObject() > 0)
	{
		if (m_uiIndex == GL_INVALID_INDEX)
		{
			CheckGlError( glGenBuffers( 1, & m_uiIndex), CU_T( "GlUniformBufferObject :: Initialise - ") + m_strUniformBlockName + CU_T( " - glGenBuffers"));
		}

		if (m_iUniformBlockIndex == GL_INVALID_INDEX && m_uiIndex != GL_INVALID_INDEX)
		{
			BufferManager::AssignBuffer<unsigned char>( m_uiIndex, this);

			CheckGlError( m_iUniformBlockIndex = glGetUniformBlockIndex( m_pProgram->GetProgramObject(), m_strUniformBlockName.char_str()), CU_T( "GlUniformBufferObject :: Initialise - ") + m_strUniformBlockName + CU_T( " - glGetUniformBlockIndex"));

			if (m_iUniformBlockIndex != GL_INVALID_INDEX)
			{
				CheckGlError( glGetActiveUniformBlockiv( m_pProgram->GetProgramObject(), m_iUniformBlockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, & m_iUniformBlockSize), CU_T( "GlUniformBufferObject :: Initialise - ") + m_strUniformBlockName + CU_T( " - glGetActiveUniformBlockiv"));
				InitialiseBuffer( m_iUniformBlockSize, 0);
				CheckGlError( glBindBuffer( GL_UNIFORM_BUFFER, m_uiIndex), CU_T( "GlUniformBufferObject :: Initialise - ") + m_strUniformBlockName + CU_T( " - glBindBuffer"));
				CheckGlError( glBufferData( GL_UNIFORM_BUFFER, m_iUniformBlockSize, NULL, GL_STREAM_DRAW), CU_T( "GlUniformBufferObject :: Initialise - ") + m_strUniformBlockName + CU_T( " - glBufferData"));
				CheckGlError( glBindBufferBase (GL_UNIFORM_BUFFER, 0, m_uiIndex), CU_T( "GlUniformBufferObject :: Initialise - ") + m_strUniformBlockName + CU_T( " - glBindBufferBase"));
				CheckGlError( glUniformBlockBinding( m_pProgram->GetProgramObject(), m_iUniformBlockIndex, 0), CU_T( "GlUniformBufferObject :: Initialise - ") + m_strUniformBlockName + CU_T( " - glUniformBlockBinding"));
			}
		}
	}
}

void GlUniformBufferObject :: Activate()
{
	if (m_iUniformBlockIndex != GL_INVALID_INDEX && m_bChanged)
	{
		CheckGlError( glBindBuffer( GL_UNIFORM_BUFFER, m_uiIndex), CU_T( "GlUniformBufferObject :: Activate - ") + m_strUniformBlockName + CU_T( " - glBindBuffer"));

		for (GlUboVariablePtrStrMap::iterator l_it = m_mapVariables.begin() ; l_it != m_mapVariables.end() ; ++l_it)
		{
			l_it->second->Activate();
		}
	}

	m_bChanged = false;
}

void GlUniformBufferObject :: Deactivate()
{
	CheckGlError( glBindBuffer( GL_UNIFORM_BUFFER, 0), CU_T( "GlUniformBufferObject :: Deactivate - ") + m_strUniformBlockName + CU_T( " - glBindBuffer"));
}

void * GlUniformBufferObject :: Lock( size_t p_uiOffset, size_t p_uiSize, size_t p_uiFlags)
{
	CheckGlError( glBindBuffer( GL_UNIFORM_BUFFER, m_uiIndex), CU_T( "GlUniformBufferObject :: Lock - glBindBuffer"));
	unsigned char * l_pBuffer;
	CheckGlError( l_pBuffer = reinterpret_cast<unsigned char*>( glMapBuffer( GL_UNIFORM_BUFFER, GlEnum::GetLockFlags( p_uiFlags))), CU_T( "GlUniformBufferObject :: Lock - glMapBuffer"));
	return l_pBuffer + p_uiOffset;
}

void GlUniformBufferObject :: Unlock()
{
	CheckGlError( glUnmapBuffer( GL_UNIFORM_BUFFER), CU_T( "GlUniformBufferObject :: Unlock - glUnmapBuffer"));
}

bool GlUniformBufferObject :: AddVariable( GlUboVariablePtr p_pVariable)
{
	bool l_bReturn = false;
	const char * l_pChar = p_pVariable->m_strName.char_str();
	CheckGlError( glGetUniformIndices( m_pProgram->GetProgramObject(), 1, & l_pChar, & (p_pVariable->m_uiIndex)), CU_T( "GlUniformBufferObject :: GetVariable - ") + m_strUniformBlockName + CU_T( " - glGetUniformIndices - " + p_pVariable->m_strName));

	if (p_pVariable->m_uiIndex != GL_INVALID_INDEX)
	{
		CheckGlError( glGetActiveUniformsiv( m_pProgram->GetProgramObject(), 1, & p_pVariable->m_uiIndex, GL_UNIFORM_OFFSET, & (p_pVariable->m_iOffset)), CU_T( "GlUniformBufferObject :: GetVariable - ") + m_strUniformBlockName + CU_T( " - glGetActiveUniformsiv GL_UNIFORM_OFFSET - " + p_pVariable->m_strName));
		CheckGlError( glGetActiveUniformsiv( m_pProgram->GetProgramObject(), 1, & p_pVariable->m_uiIndex, GL_UNIFORM_SIZE, & (p_pVariable->m_iSize)), CU_T( "GlUniformBufferObject :: GetVariable - ") + m_strUniformBlockName + CU_T( " - glGetActiveUniformsiv GL_UNIFORM_SIZE - " + p_pVariable->m_strName));

		if (p_pVariable->m_iSize == 1)
		{
			p_pVariable->m_iSize = p_pVariable->GetSize();
		}

		if (p_pVariable->GetSize() + p_pVariable->m_iOffset <= m_totalSize)
		{
			m_mapVariables.insert( GlUboVariablePtrStrMap::value_type( p_pVariable->m_strName, p_pVariable));
			l_bReturn = true;
			m_bChanged = true;
		}
	}

	return l_bReturn;
}

//******************************************************************************

GlVertexArrayObjects :: GlVertexArrayObjects()
	:	m_uiIndex( GL_INVALID_INDEX)
{
}

GlVertexArrayObjects :: ~GlVertexArrayObjects()
{
	Cleanup();
}

void GlVertexArrayObjects :: Cleanup()
{
	if (m_uiIndex != GL_INVALID_INDEX)
	{
		CheckGlError( glDeleteVertexArrays( 1, & m_uiIndex), CU_T( "GlVertexArrayObjects :: Cleanup - glDeleteVertexArrays"));
		m_uiIndex = GL_INVALID_INDEX;
	}
}

void GlVertexArrayObjects :: Initialise()
{
	if (m_uiIndex == GL_INVALID_INDEX)
	{
		CheckGlError( glGenVertexArrays( 1, & m_uiIndex), CU_T( "GlVertexArrayObjects :: Initialise - glGenVertexArrays"));
	}
}

void GlVertexArrayObjects :: Activate()
{
	if (m_uiIndex != GL_INVALID_INDEX)
	{
		CheckGlError( glBindVertexArray( m_uiIndex), CU_T( "GlVertexArrayObjects :: Activate - glBindVertexArray"));
	}
}

void GlVertexArrayObjects :: Deactivate()
{
	CheckGlError( glBindVertexArray( 0), CU_T( "GlVertexArrayObjects :: Deactivate - glBindVertexArray( 0)"));
}

//******************************************************************************

GlTextureBufferObject :: GlTextureBufferObject()
	:	TextureBufferObject(),
		m_uiIndex( GL_INVALID_INDEX)
{
}

GlTextureBufferObject :: ~GlTextureBufferObject()
{
	Cleanup();
}

void GlTextureBufferObject :: Cleanup()
{
	if (m_uiIndex != GL_INVALID_INDEX)// && glIsBuffer( m_uiIndex))
	{
		BufferManager::UnassignBuffer<unsigned char>( m_uiIndex, this);
		CheckGlError( glDeleteBuffers( 1, & m_uiIndex), CU_T( "GlTextureBufferObject :: Cleanup - glDeleteBuffers"));
	}

	m_uiIndex = GL_INVALID_INDEX;
}

void GlTextureBufferObject :: Initialise( const ePIXEL_FORMAT & p_format, size_t p_uiSize, const unsigned char * p_pBytes)
{
	TextureBufferObject::Initialise( p_format, p_uiSize, p_pBytes);

	CheckGlError( glGenBuffers( 1, & m_uiIndex), CU_T( "GlTextureBufferObject :: Initialise - glGenBuffers"));

	if (m_uiIndex != GL_INVALID_INDEX)// && glIsBuffer( m_uiIndex))
	{
		CheckGlError( glBindBuffer( GL_TEXTURE_BUFFER, m_uiIndex), CU_T( "GlTextureBufferObject :: Initialise - glBindBuffer"));
		CheckGlError( glBufferData( GL_TEXTURE_BUFFER, m_uiSize * Castor::Resources::GetBytesPerPixel( m_pixelFormat) / 8, m_pBytes, GL_STREAM_DRAW), CU_T( "GlTextureBufferObject :: Initialise - glBufferData"));
		CheckGlError( glBindBuffer( GL_TEXTURE_BUFFER, 0), CU_T( "GlTextureBufferObject :: Initialise - glBindBuffer"));
		BufferManager::AssignBuffer<unsigned char>( m_uiIndex, this);
	}
	else
	{
		m_uiIndex = GL_INVALID_INDEX;
	}
}

void * GlTextureBufferObject :: Lock( size_t p_uiOffset, size_t p_uiSize, size_t p_uiFlags)
{
	CheckGlError( glBindBuffer( GL_TEXTURE_BUFFER, m_uiIndex), CU_T( "GlUniformBufferObject :: Lock - glBindBuffer"));
	unsigned char * l_pBuffer;
	CheckGlError( l_pBuffer = reinterpret_cast<unsigned char*>( glMapBuffer( GL_TEXTURE_BUFFER, GlEnum::GetLockFlags( p_uiFlags))), CU_T( "GlUniformBufferObject :: Lock - glMapBuffer"));
	return l_pBuffer + p_uiOffset;
}

void GlTextureBufferObject :: Unlock()
{
	CheckGlError( glUnmapBuffer( GL_TEXTURE_BUFFER), CU_T( "GlUniformBufferObject :: Unlock - glUnmapBuffer"));
}

//******************************************************************************
