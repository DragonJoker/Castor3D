#include "PrecompiledHeader.h"

#include "GLBuffer.h"
#include "Module_GL.h"
#include "GLRenderSystem.h"
#include "GLUniformVariable.h"
#include "GLSLProgram.h"

using namespace Castor3D;

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
		if (m_pProgram != NULL)
		{
			ActivateBufferObject();
			ActivateAttribute();
		}
	}
}

void GLVBOVertexBuffer :: Deactivate()  
{
	if (m_bAssigned)
	{
		if (m_pProgram != NULL)
		{
			DeactivateAttribute();
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
		if (m_pProgram != NULL)
		{
			ActivateBufferObject();
			ActivateAttribute();
		}
	}
}

void GLVBONormalsBuffer :: Deactivate()
{
	if (m_bAssigned)
	{
		if (m_pProgram != NULL)
		{
			DeactivateAttribute();
		}
	}
}

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

void GLVBOTextureBuffer :: SetShaderProgram( ShaderProgram * p_pProgram)
{
	for (unsigned int i = 0 ; i < m_arrayBuffers.size() ; i++)
	{
		m_arrayBuffers[i]->SetShaderProgram( p_pProgram);
	}
}

//******************************************************************************

GLUBOVariable :: GLUBOVariable()
	:	m_uiIndex( GL_INVALID_INDEX)
{
}

GLUBOVariable :: ~GLUBOVariable()
{
}

void GLUBOVariable :: Activate()
{
	glBufferSubData( GL_UNIFORM_BUFFER_EXT, m_iOffset, m_iSize, m_pValue);
	CheckGLError( "UBOVariable :: Activate - glBufferSubData");
}

void GLUBOVariable :: Deactivate()
{
}

void GLUBOVariable :: SetValue( const void * p_pValue)
{
	memcpy( m_pValue, p_pValue, m_uiValueSize);
}

void GLUBOVariable :: SetIndexValue( const void * p_pValue, int p_iIndex)
{
	size_t l_uiSize = m_uiValueSize / m_iSize;
	memcpy( & m_pValue[p_iIndex * l_uiSize], p_pValue, l_uiSize);
}

//******************************************************************************

GLUniformBufferObject :: GLUniformBufferObject( const String & p_strUniformBlockName, GLShaderProgram * p_pProgram)
	:	Buffer3D(),
		m_uiIndex( GL_INVALID_INDEX),
		m_pProgram( p_pProgram),
		m_strUniformBlockName( p_strUniformBlockName),
		m_iUniformBlockIndex( GL_INVALID_INDEX)
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
		glDeleteBuffers( 1, & m_uiIndex);
		m_uiIndex = GL_INVALID_INDEX;
	}

	Buffer3D::Cleanup();
}

void GLUniformBufferObject :: Initialise()
{
	if (m_pProgram != NULL && m_pProgram->GetProgramObject() > 0)
	{
		if (m_uiIndex == GL_INVALID_INDEX)
		{
			glGenBuffers( 1, & m_uiIndex);
			CheckGLError( "GLUniformBufferObject :: Initialise - " + m_strUniformBlockName + " - glGenBuffers");
		}

		if (m_iUniformBlockIndex == GL_INVALID_INDEX)
		{
			m_iUniformBlockIndex = glGetUniformBlockIndex( m_pProgram->GetProgramObject(), m_strUniformBlockName.c_str());
			CheckGLError( "GLUniformBufferObject :: Initialise - " + m_strUniformBlockName + " - glGetUniformBlockIndex");

			if (m_iUniformBlockIndex != GL_INVALID_INDEX)
			{
				glGetActiveUniformBlockiv( m_pProgram->GetProgramObject(), m_iUniformBlockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, & m_iUniformBlockSize);
				CheckGLError( "GLUniformBufferObject :: Initialise - " + m_strUniformBlockName + " - glGetActiveUniformBlockiv");
				InitialiseBuffer( m_iUniformBlockSize, 0);
				glBindBuffer( GL_UNIFORM_BUFFER, m_uiIndex);
				CheckGLError( "GLUniformBufferObject :: Initialise - " + m_strUniformBlockName + " - glBindBuffer");
				glBufferData( GL_UNIFORM_BUFFER, m_iUniformBlockSize, NULL, GL_DYNAMIC_DRAW);
				CheckGLError( "GLUniformBufferObject :: Initialise - " + m_strUniformBlockName + " - glBufferData");
				glBindBufferBase (GL_UNIFORM_BUFFER, 0, m_uiIndex);
				CheckGLError( "GLUniformBufferObject :: Initialise - " + m_strUniformBlockName + " - glBindBufferBase");
				glUniformBlockBinding( m_pProgram->GetProgramObject(), m_iUniformBlockIndex, 0);
				CheckGLError( "GLUniformBufferObject :: Initialise - " + m_strUniformBlockName + " - glUniformBlockBinding");
			}
		}
	}
}

void GLUniformBufferObject :: Activate()
{
	if (m_iUniformBlockIndex == GL_INVALID_INDEX)
	{
		Initialise();
	}

	if (m_iUniformBlockIndex != GL_INVALID_INDEX)
	{
		glBindBuffer( GL_UNIFORM_BUFFER, m_uiIndex);
		CheckGLError( "GLUniformBufferObject :: Activate - " + m_strUniformBlockName + " - glBindBuffer");

		for (GLUBOVariablePtrStrMap::iterator l_it = m_mapVariables.begin() ; l_it != m_mapVariables.end() ; ++l_it)
		{
			l_it->second->Activate();
		}
	}
}

void GLUniformBufferObject :: Deactivate()
{
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	CheckGLError( "GLUniformBufferObject :: Deactivate - " + m_strUniformBlockName + " - glBindBuffer");
}

GLUBOVariablePtr GLUniformBufferObject :: AddVariable( const String & p_strName, unsigned char * p_pValue, size_t p_uiValueSize)
{
	GLUBOVariablePtr l_pReturn;

	if (m_mapVariables.find( p_strName) != m_mapVariables.end())
	{
		l_pReturn = m_mapVariables.find( p_strName)->second;
	}
	else
	{
		l_pReturn = new GLUBOVariable;
		l_pReturn->m_pValue = p_pValue;
		l_pReturn->m_uiValueSize = p_uiValueSize;
		const Char * l_pChar = p_strName.c_str();
		glGetUniformIndices( m_pProgram->GetProgramObject(), 1, & l_pChar, & (l_pReturn->m_uiIndex));
		CheckGLError( "GLUniformBufferObject :: GetVariable - " + m_strUniformBlockName + " - glGetUniformIndices - " + p_strName);

		if (l_pReturn->m_uiIndex != GL_INVALID_INDEX)
		{
			glGetActiveUniformsiv( m_pProgram->GetProgramObject(), 1, & (l_pReturn->m_uiIndex), GL_UNIFORM_OFFSET, & (l_pReturn->m_iOffset));
			CheckGLError( "GLUniformBufferObject :: GetVariable - " + m_strUniformBlockName + " - glGetActiveUniformsiv GL_UNIFORM_OFFSET - " + p_strName);
			glGetActiveUniformsiv( m_pProgram->GetProgramObject(), 1, & (l_pReturn->m_uiIndex), GL_UNIFORM_SIZE, & (l_pReturn->m_iSize));
			CheckGLError( "GLUniformBufferObject :: GetVariable - " + m_strUniformBlockName + " - glGetActiveUniformsiv GL_UNIFORM_SIZE - " + p_strName);
			AddArray( p_pValue, p_uiValueSize * l_pReturn->m_iSize);
			m_mapVariables.insert( GLUBOVariablePtrStrMap::value_type( p_strName, l_pReturn));
		}
	}

	return l_pReturn;
}

GLUBOVariablePtr GLUniformBufferObject :: GetVariable( const String & p_strName)
{
	GLUBOVariablePtr l_pReturn;

	if (m_mapVariables.find( p_strName) != m_mapVariables.end())
	{
		l_pReturn = m_mapVariables.find( p_strName)->second;
	}

	return l_pReturn;
}

void GLUniformBufferObject :: SetVariableValue( const String & p_strName, const void * p_pValue)
{
	GLUBOVariablePtr l_pVar;
	GLUBOVariablePtrStrMap::iterator l_it = m_mapVariables.find( p_strName);

	if (l_it != m_mapVariables.end())
	{
		l_pVar = l_it->second;
		l_pVar->SetValue( p_pValue);
	}
}

//******************************************************************************