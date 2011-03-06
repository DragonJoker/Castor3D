#include "OpenGlCommon/PrecompiledHeader.h"

#include "OpenGlCommon/GlBuffer.h"

using namespace Castor3D;

//******************************************************************************

GlAttribsBase :: GlAttribsBase( const String & p_strAttribName)
	:	m_strAttribName( p_strAttribName)
	,	m_iAttribLocation( GL_INVALID_INDEX)
{
}

GlAttribsBase :: ~GlAttribsBase()
{
	CleanupAttribute();
}

void GlAttribsBase :: SetShaderProgram( ShaderProgramPtr p_pProgram)
{
	m_pProgram = static_pointer_cast<GlShaderProgram>( p_pProgram);
}

void GlAttribsBase :: CleanupAttribute()
{
	m_iAttribLocation = GL_INVALID_INDEX;
	m_pProgram.reset();
}

bool GlAttribsBase :: InitialiseAttribute()
{
	bool l_bReturn = ! RenderSystem::UseShaders();

	if ( ! m_pProgram.expired() && m_pProgram.lock()->IsLinked() && ! l_bReturn)
	{
		CheckGlError( m_iAttribLocation = glGetAttribLocation( m_pProgram.lock()->GetProgramObject(), m_strAttribName.char_str()), CU_T( "GlVertexAttribs :: Initialise - glGetAttribLocation"));

		if (m_iAttribLocation >= 0 && m_iAttribLocation != GL_INVALID_INDEX)
		{
			l_bReturn = true;
		}
	}

	return l_bReturn;
}

void GlAttribsBase :: ActivateAttribute( bool p_bNormalised)
{
	CheckGlError( glEnableVertexAttribArray( m_iAttribLocation), CU_T( "GlVertexAttribs :: Activate - ") + m_strAttribName + CU_T( " - glEnableVertexAttribArray"));
	CheckGlError( glVertexAttribPointer( m_iAttribLocation, m_uiCount, m_iGlType, p_bNormalised, m_uiStride, BUFFER_OFFSET( m_uiOffset)), CU_T( "GlVertexAttribs :: Activate - ") + m_strAttribName + CU_T( " - glVertexAttribPointer"));
}

void GlAttribsBase :: DeactivateAttribute()
{
	CheckGlError( glDisableVertexAttribArray( m_iAttribLocation), CU_T( "GlVertexAttribs :: Deactivate - ") + m_strAttribName + CU_T( " - glDisableVertexAttribArray"));
}

//******************************************************************************

GlIndexBuffer :: GlIndexBuffer()
{
}

GlIndexBuffer :: ~GlIndexBuffer()
{
}

//******************************************************************************

GlVertexBuffer :: GlVertexBuffer( const BufferElementDeclaration * p_pElements, size_t p_uiNbElements)
	:	VertexBuffer( p_pElements, p_uiNbElements)
{
}

GlVertexBuffer :: ~GlVertexBuffer()
{
	Cleanup();
}

//******************************************************************************

GlVboIndexBuffer :: GlVboIndexBuffer()
	:	m_uiFilled( IndexBuffer::m_filled)
	,	m_pBuffer( IndexBuffer::m_buffer)
{
}

GlVboIndexBuffer :: ~GlVboIndexBuffer()
{
	Cleanup();
}

void GlVboIndexBuffer :: Cleanup()
{
	Buffer3D<unsigned int>::Cleanup();
	_cleanupBufferObject();
}

void GlVboIndexBuffer :: Initialise( eBUFFER_MODE p_eMode)
{
	CheckGlError( glGenBuffers( 1, & m_uiIndex), CU_T( "GlIndexBuffer :: Initialise - glGenBuffers"));

	if (m_uiIndex != GL_INVALID_INDEX)// && glIsBuffer( m_uiIndex))
	{
		CheckGlError( glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_uiIndex), CU_T( "GlIndexBuffer :: Initialise - glBindBuffer"));
		CheckGlError( glBufferData( GL_ELEMENT_ARRAY_BUFFER, m_uiFilled * sizeof( unsigned int), m_pBuffer, GlEnum::GetBufferFlags( p_eMode)), CU_T( "GlIndexBuffer :: Initialise - glBufferData"));
		CheckGlError( glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0), CU_T( "GlIndexBuffer :: Initialise - glBindBuffer( NULL)"));
	}
	else
	{
		m_uiIndex = GL_INVALID_INDEX;
	}

	m_bAssigned = (m_uiIndex != GL_INVALID_INDEX);

	if (m_bAssigned)
	{
		BufferManager::AssignBuffer<unsigned int>( m_uiIndex, this);
	}
}

void GlVboIndexBuffer :: Activate()
{
	if (m_bAssigned)
	{
		CheckGlError( glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_uiIndex), CU_T( "GlVboIndexBuffer :: Activate - glBindBuffer"));
	}
}

void GlVboIndexBuffer :: Deactivate()
{
}

void * GlVboIndexBuffer :: Lock( size_t p_uiOffset, size_t p_uiSize, size_t p_uiFlags)
{
	CheckGlError( glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_uiIndex), CU_T( "GlVboIndexBuffer :: Lock - glBindBuffer"));
	unsigned char * l_pBuffer;
	CheckGlError( l_pBuffer = reinterpret_cast<unsigned char*>( glMapBuffer( GL_ELEMENT_ARRAY_BUFFER, GlEnum::GetLockFlags( p_uiFlags))), CU_T( "GlVboIndexBuffer :: Lock - glMapBuffer"));
	return l_pBuffer + p_uiOffset;
}

void GlVboIndexBuffer :: Unlock()
{
	CheckGlError( glUnmapBuffer( GL_ELEMENT_ARRAY_BUFFER), CU_T( "GlVboIndexBuffer :: Unlock - glUnmapBuffer"));
}

void GlVboIndexBuffer :: CleanupVbo()
{
	_cleanupBufferObject();
}

void GlVboIndexBuffer :: _cleanupBufferObject()
{
	if (this->m_bAssigned)
	{
		BufferManager::UnassignBuffer<unsigned int>( m_uiIndex, this);
	}

	if (m_uiIndex != GL_INVALID_INDEX)// && glIsBuffer( m_uiIndex))
	{
		CheckGlError( glDeleteBuffers( 1, & m_uiIndex), CU_T( "GlIndexBuffer :: Cleanup - glDeleteBuffers"));
	}

	m_uiIndex = GL_INVALID_INDEX;
	m_bAssigned = false;
}

//******************************************************************************

GlVboVertexBuffer :: GlVboVertexBuffer( const BufferElementDeclaration * p_pElements, size_t p_uiNbElements)
	:	GlVertexBuffer( p_pElements, p_uiNbElements)
	,	GlVertexBufferObject<real>( VertexBuffer::m_filled, VertexBuffer::m_buffer, VertexBuffer::m_bufferDeclaration)
{
	GlVertexAttribsPtr l_pBuffer;

	for (BufferDeclaration::const_iterator l_it = m_bufferDeclaration.Begin() ; l_it != m_bufferDeclaration.End() ; ++l_it)
	{
		switch (( * l_it).m_eUsage)
		{
		case eUsagePosition:
			l_pBuffer.reset( new GlVertexAttribs3r( "vertex"));
			break;

		case eUsageNormal:
			l_pBuffer.reset( new GlVertexAttribs3r( "normal"));
			break;

		case eUsageTangent:
			l_pBuffer.reset( new GlVertexAttribs3r( "tangent"));
			break;

		case eUsageTexCoords0:
		case eUsageTexCoords1:
		case eUsageTexCoords2:
		case eUsageTexCoords3:
			l_pBuffer.reset( new GlVertexAttribs2r( "texture"));
			break;
		}

		l_pBuffer->SetOffset( ( * l_it).m_uiOffset);
		l_pBuffer->SetStride( m_bufferDeclaration.GetStride());
		m_arrayAttribs.push_back( l_pBuffer);
	}
}

GlVboVertexBuffer :: ~GlVboVertexBuffer()
{
	Cleanup();
}

void GlVboVertexBuffer :: Cleanup()
{
	Buffer<real>::Cleanup();
	_cleanupAttribute();
	_cleanupBufferObject();
}

void GlVboVertexBuffer :: Initialise( eBUFFER_MODE p_eMode)
{
	if (m_pProgram.expired() || _initialiseAttribute())
	{
		_initialiseBufferObject( p_eMode);
		m_bAssigned = (m_uiIndex != GL_INVALID_INDEX);

		if (m_bAssigned)
		{
			BufferManager::AssignBuffer<real>( m_uiIndex, this);
		}
	}
}

void GlVboVertexBuffer :: Activate()
{
	if (m_bAssigned)
	{
		_activateBufferObject();

		if (RenderSystem::UseShaders() && ! m_pProgram.expired())
		{
			_activateAttribute();
		}
	}
}

void GlVboVertexBuffer :: Deactivate()
{
	if (m_bAssigned)
	{
		if (RenderSystem::UseShaders() && ! m_pProgram.expired())
		{
			_deactivateAttribute();
		}
	}
}

void * GlVboVertexBuffer :: Lock( size_t p_uiOffset, size_t p_uiSize, size_t p_uiFlags)
{
	return GlVertexBufferObject<real>::Lock( p_uiOffset, p_uiSize, p_uiFlags);
}

void GlVboVertexBuffer :: Unlock()
{
	GlVertexBufferObject<real>::Unlock();
}

void GlVboVertexBuffer :: SetShaderProgram( ShaderProgramPtr p_pProgram)
{
	GlShaderProgramPtr l_pProgram = static_pointer_cast<GlShaderProgram>( p_pProgram);

	if ((this->m_pProgram.expired() && l_pProgram != NULL) || (l_pProgram == NULL && ! this->m_pProgram.expired()) || this->m_pProgram.lock() != l_pProgram)
	{
		_cleanupBufferObject();
		_cleanupAttribute();

		for (size_t i = 0 ; i < m_arrayAttribs.size() ; i++)
		{
			m_arrayAttribs[i]->SetShaderProgram( l_pProgram);
		}

		m_pProgram = l_pProgram;
		Initialise( eBufferDynamic);
	}
}

void GlVboVertexBuffer :: CleanupVbo()
{
	_cleanupBufferObject();
	_cleanupAttribute();
}

void GlVboVertexBuffer :: _cleanupBufferObject()
{
	if (this->m_bAssigned)
	{
		BufferManager::UnassignBuffer<real>( m_uiIndex, this);
	}

	GlVertexBufferObject<real>::_cleanupBufferObject();
	m_bAssigned = false;
}

void GlVboVertexBuffer :: _activateBufferObject()
{
	if (RenderSystem::UseShaders() && ! m_pProgram.expired())
	{
		CheckGlError( glBindBuffer( GL_ARRAY_BUFFER, m_uiIndex), CU_T( "GlVboVertexBuffer :: Activate - glBindBuffer"));
	}
	else
	{
		GlVertexBufferObject<real>::_activateBufferObject();
	}
}

void GlVboVertexBuffer :: _cleanupAttribute()
{
	for (size_t i = 0 ; i < m_arrayAttribs.size() ; i++)
	{
		m_arrayAttribs[i]->CleanupAttribute();
	}
}

bool GlVboVertexBuffer :: _initialiseAttribute()
{
	m_uiValid = 0;

	for (size_t i = 0 ; i < m_arrayAttribs.size() ; i++)
	{
		m_uiValid += (m_arrayAttribs[i]->InitialiseAttribute() ? 1 : 0);
	}

	return m_uiValid > 0;
}

void GlVboVertexBuffer :: _activateAttribute()
{
	for (size_t i = 0 ; i < m_arrayAttribs.size() ; i++)
	{
		if (m_arrayAttribs[i]->GetAttribLocation() != GL_INVALID_INDEX)
		{
			m_arrayAttribs[i]->ActivateAttribute( false);
		}
	}
}

void GlVboVertexBuffer :: _deactivateAttribute()
{
	for (size_t i = 0 ; i < m_arrayAttribs.size() ; i++)
	{
		if (m_arrayAttribs[i]->GetAttribLocation() != GL_INVALID_INDEX)
		{
			m_arrayAttribs[i]->DeactivateAttribute();
		}
	}
}

//******************************************************************************
