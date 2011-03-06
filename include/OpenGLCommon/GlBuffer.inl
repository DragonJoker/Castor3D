//*************************************************************************************************

template <typename T>
int GlBufferBase :: _getGlType()
{
	if (typeid( T) == typeid( int))
	{
		return GL_INT;
	}
	if (typeid( T) == typeid( size_t))
	{
		return GL_UNSIGNED_INT;
	}
	if (typeid( T) == typeid( short))
	{
		return GL_SHORT;
	}
	if (typeid( T) == typeid( unsigned short))
	{
		return GL_UNSIGNED_SHORT;
	}
	if (typeid( T) == typeid( char))
	{
		return GL_BYTE;
	}
	if (typeid( T) == typeid( unsigned char))
	{
		return GL_UNSIGNED_BYTE;
	}
	if (typeid( T) == typeid( real))
	{
		return GL_REAL;
	}
	if (typeid( T) == typeid( float))
	{
		return GL_FLOAT;
	}
	if (typeid( T) == typeid( double))
	{
		return GL_DOUBLE;
	}

	return 0;
}

//*************************************************************************************************

template <typename T, size_t Count>
GlVertexAttribs<T, Count> :: GlVertexAttribs( const String & p_strAttribName)
	:	GlAttribsBase( p_strAttribName)
{
	m_iGlType = _getGlType<T>();
	m_uiCount = Count;
}

template <typename T, size_t Count>
GlVertexAttribs<T, Count> :: ~GlVertexAttribs()
{
}

//*************************************************************************************************

template <typename T>
GlVertexBufferObject<T> :: GlVertexBufferObject( unsigned int & p_uiFilled, T *& p_pBuffer, const BufferDeclaration & p_declaration)
	:	m_uiFilled( p_uiFilled)
	,	m_pBuffer( p_pBuffer)
	,	m_declaration( p_declaration)
	,	m_uiIndex( GL_INVALID_INDEX)
{
	m_iGlType = _getGlType<T>();
}

template <typename T>
GlVertexBufferObject<T> :: ~GlVertexBufferObject()
{
	_cleanupBufferObject();
}

template <typename T>
void * GlVertexBufferObject<T> :: Lock( size_t p_uiOffset, size_t p_uiSize, size_t p_uiFlags)
{
	CheckGlError( glBindBuffer( GL_ARRAY_BUFFER, m_uiIndex), CU_T( "GlVertexBufferObject :: Lock - glBindBuffer"));
	unsigned char * l_pBuffer;
	CheckGlError( l_pBuffer = reinterpret_cast<unsigned char*>( glMapBuffer( GL_ARRAY_BUFFER, GlEnum::GetLockFlags( p_uiFlags))), CU_T( "GlVertexBufferObject :: Lock - glMapBuffer"));
	return l_pBuffer + p_uiOffset;
}

template <typename T>
void GlVertexBufferObject<T> :: Unlock()
{
	CheckGlError( glUnmapBuffer( GL_ARRAY_BUFFER), CU_T( "GlVertexBufferObject :: Unlock - glUnmapBuffer"));
}

template <typename T>
void GlVertexBufferObject<T> :: _cleanupBufferObject()
{
	if (m_uiIndex != GL_INVALID_INDEX)// && glIsBuffer( m_uiIndex))
	{
		CheckGlError( glDeleteBuffers( 1, & m_uiIndex), CU_T( "GlVertexBufferObject :: Cleanup - glDeleteBuffers"));
	}

	m_uiIndex = GL_INVALID_INDEX;
}

template <typename T>
void GlVertexBufferObject<T> :: _initialiseBufferObject( eBUFFER_MODE p_eMode)
{
	CheckGlError( glGenBuffers( 1, & m_uiIndex), CU_T( "GlVertexBufferObject :: Initialise - glGenBuffers"));

	if (m_uiIndex != GL_INVALID_INDEX)// && glIsBuffer( m_uiIndex))
	{
		CheckGlError( glBindBuffer( GL_ARRAY_BUFFER, m_uiIndex), CU_T( "GlVertexBufferObject :: Initialise - glBindBuffer"));
		CheckGlError( glBufferData( GL_ARRAY_BUFFER, m_uiFilled * sizeof( T), m_pBuffer, GlEnum::GetBufferFlags( p_eMode)), CU_T( "GlVertexBufferObject :: Initialise - glBufferData"));
		CheckGlError( glBindBuffer( GL_ARRAY_BUFFER, 0), CU_T( "GlVertexBufferObject :: Initialise - glBindBuffer(NULL)"));
	}
	else
	{
		m_uiIndex = GL_INVALID_INDEX;
	}
}

template <typename T>
void GlVertexBufferObject<T> :: _activateBufferObject()
{
	CheckGlError( glBindBuffer( GL_ARRAY_BUFFER, m_uiIndex), CU_T( "GlVertexBufferObject :: Activate - glBindBuffer"));

	static const unsigned int s_arraySize[] = {1, 2, 3, 4, 4};
	static const unsigned int s_arrayType[] = {GL_FLOAT, GL_FLOAT, GL_FLOAT, GL_FLOAT, GL_UNSIGNED_BYTE};

	for (BufferDeclaration::const_iterator l_it = m_declaration.Begin() ; l_it != m_declaration.End() ; ++l_it)
	{
		switch (( * l_it).m_eUsage)
		{
		case eUsagePosition:
			CheckGlError( glEnableClientState( GL_VERTEX_ARRAY), CU_T( "GlVertexBufferObject :: Activate"));
			CheckGlError( glVertexPointer( s_arraySize[( * l_it).m_eDataType], s_arrayType[( * l_it).m_eDataType], m_declaration.GetStride(), BUFFER_OFFSET( ( * l_it).m_uiOffset)), CU_T( "GlVertexBufferObject :: Activate"));
			break;

		case eUsageNormal:
			CheckGlError( glEnableClientState( GL_NORMAL_ARRAY), CU_T( "GlVertexBufferObject :: Activate"));
			CheckGlError( glNormalPointer( s_arrayType[( * l_it).m_eDataType], m_declaration.GetStride(), BUFFER_OFFSET( ( * l_it).m_uiOffset)), CU_T( "GlVertexBufferObject :: Activate"));
			break;

		case eUsageTangent:
			if (glTangentPointerEXT != NULL)
			{
				CheckGlError( glEnableClientState( GL_TANGENT_ARRAY_EXT), CU_T( "GlVertexBufferObject :: Activate"));
				CheckGlError( glTangentPointerEXT( s_arrayType[( * l_it).m_eDataType], m_declaration.GetStride(), BUFFER_OFFSET( ( * l_it).m_uiOffset)), CU_T( "GlVertexBufferObject :: Activate"));
			}
			break;

		case eUsageDiffuse:
			CheckGlError( glEnableClientState( GL_COLOR_ARRAY), CU_T( "GlVertexBufferObject :: Activate"));
			CheckGlError( glColorPointer( s_arraySize[( * l_it).m_eDataType], s_arrayType[( * l_it).m_eDataType], m_declaration.GetStride(), BUFFER_OFFSET( ( * l_it).m_uiOffset)), CU_T( "GlVertexBufferObject :: Activate"));
			break;

		case eUsageTexCoords0:
			CheckGlError( glActiveTexture( GL_TEXTURE0), CU_T( "GlVertexBufferObject :: Activate"));
			CheckGlError( glClientActiveTexture( GL_TEXTURE0), CU_T( "GlVertexBufferObject :: Activate"));
			CheckGlError( glEnableClientState( GL_TEXTURE_COORD_ARRAY), CU_T( "GlVertexBufferObject :: Activate"));
			CheckGlError( glTexCoordPointer( s_arraySize[( * l_it).m_eDataType], s_arrayType[( * l_it).m_eDataType], m_declaration.GetStride(), BUFFER_OFFSET( ( * l_it).m_uiOffset)), CU_T( "GlVertexBufferObject :: Activate"));
			break;

		case eUsageTexCoords1:
			CheckGlError( glActiveTexture( GL_TEXTURE1), CU_T( "GlVertexBufferObject :: Activate"));
			CheckGlError( glClientActiveTexture( GL_TEXTURE1), CU_T( "GlVertexBufferObject :: Activate"));
			CheckGlError( glEnableClientState( GL_TEXTURE_COORD_ARRAY), CU_T( "GlVertexBufferObject :: Activate"));
			CheckGlError( glTexCoordPointer( s_arraySize[( * l_it).m_eDataType], s_arrayType[( * l_it).m_eDataType], m_declaration.GetStride(), BUFFER_OFFSET( ( * l_it).m_uiOffset)), CU_T( "GlVertexBufferObject :: Activate"));
			break;

		case eUsageTexCoords2:
			CheckGlError( glActiveTexture( GL_TEXTURE2), CU_T( "GlVertexBufferObject :: Activate"));
			CheckGlError( glClientActiveTexture( GL_TEXTURE2), CU_T( "GlVertexBufferObject :: Activate"));
			CheckGlError( glEnableClientState( GL_TEXTURE_COORD_ARRAY), CU_T( "GlVertexBufferObject :: Activate"));
			CheckGlError( glTexCoordPointer( s_arraySize[( * l_it).m_eDataType], s_arrayType[( * l_it).m_eDataType], m_declaration.GetStride(), BUFFER_OFFSET( ( * l_it).m_uiOffset)), CU_T( "GlVertexBufferObject :: Activate"));
			break;

		case eUsageTexCoords3:
			CheckGlError( glActiveTexture( GL_TEXTURE3), CU_T( "GlVertexBufferObject :: Activate"));
			CheckGlError( glClientActiveTexture( GL_TEXTURE3), CU_T( "GlVertexBufferObject :: Activate"));
			CheckGlError( glEnableClientState( GL_TEXTURE_COORD_ARRAY), CU_T( "GlVertexBufferObject :: Activate"));
			CheckGlError( glTexCoordPointer( s_arraySize[( * l_it).m_eDataType], s_arrayType[( * l_it).m_eDataType], m_declaration.GetStride(), BUFFER_OFFSET( ( * l_it).m_uiOffset)), CU_T( "GlVertexBufferObject :: Activate"));
			break;
		}
	}
}

template <typename T>
void GlVertexBufferObject<T> :: _deactivateBufferObject()
{
	CheckGlError( glDisableClientState( GL_VERTEX_ARRAY), CU_T( "GlVertexBufferObject :: DeactivateBufferObject - glDisableClientState GL_VERTEX_ARRAY"));
	CheckGlError( glDisableClientState( GL_NORMAL_ARRAY), CU_T( "GlVertexBufferObject :: DeactivateBufferObject - glDisableClientState GL_NORMAL_ARRAY"));
	CheckGlError( glDisableClientState( GL_COLOR_ARRAY), CU_T( "GlVertexBufferObject :: DeactivateBufferObject - glDisableClientState GL_COLOR_ARRAY"));

	for (int i = 0; i < 4; ++i)
	{
		CheckGlError( glActiveTexture(GL_TEXTURE0 + i), CU_T( "GlVertexBufferObject :: DeactivateBufferObject - glActiveTexture"));
		CheckGlError( glDisableClientState( GL_TEXTURE_COORD_ARRAY), CU_T( "GlVertexBufferObject :: DeactivateBufferObject - glDisableClientState GL_TEXTURE_COORD_ARRAY"));
	}
}

//*************************************************************************************************

template <typename T, size_t Count>
GlVertexAttribsBuffer<T, Count> :: GlVertexAttribsBuffer( const String & p_strAttribName)
	:	VertexAttribsBuffer<T>( p_strAttribName)
	,	GlVertexAttribs<T, Count>( p_strAttribName)
	,	m_uiFilled( VertexAttribsBuffer<T>::m_filled)
	,	m_pBuffer( VertexAttribsBuffer<T>::m_buffer)
{
	GlBufferBase::m_iGlType = GlBufferBase::_getGlType<T>();
}

template <typename T, size_t Count>
GlVertexAttribsBuffer<T, Count> :: ~GlVertexAttribsBuffer()
{
	Cleanup();
}

template <typename T, size_t Count>
void GlVertexAttribsBuffer<T, Count> :: Cleanup()
{
	Buffer<T>::Cleanup();
	_cleanupBufferObject();
	GlAttribsBase::CleanupAttribute();
}

template <typename T, size_t Count>
void GlVertexAttribsBuffer<T, Count> :: Initialise( eBUFFER_MODE p_eMode)
{
	if (GlAttribsBase::InitialiseAttribute())
	{
		_initialiseBufferObject( p_eMode);
		Buffer3D<T>::m_bAssigned = m_uiIndex != GL_INVALID_INDEX;

		if (Buffer3D<T>::m_bAssigned)
		{
			BufferManager::AssignBuffer<T>( m_uiIndex, this);
		}
	}
}

template <typename T, size_t Count>
void GlVertexAttribsBuffer<T, Count> :: Activate()
{
	if (Buffer3D<T>::m_bAssigned)
	{
		_activateBufferObject();
		GlAttribsBase::ActivateAttribute();
	}
}

template <typename T, size_t Count>
void GlVertexAttribsBuffer<T, Count> :: Deactivate()
{
	if (Buffer3D<T>::m_bAssigned)
	{
		GlAttribsBase::DeactivateAttribute();
	}
}

template <typename T, size_t Count>
void * GlVertexAttribsBuffer<T, Count> :: Lock( size_t p_uiOffset, size_t p_uiSize, size_t p_uiFlags)
{
	CheckGlError( glBindBuffer( GL_ARRAY_BUFFER, m_uiIndex), CU_T( "GlVertexAttribsBuffer :: Lock - glBindBuffer"));
	unsigned char * l_pBuffer;
	CheckGlError( l_pBuffer = reinterpret_cast<unsigned char*>( glMapBuffer( GL_ARRAY_BUFFER, GlEnum::GetLockFlags( p_uiFlags))), CU_T( "GlVertexAttribsBuffer :: Lock - glMapBuffer"));
	return l_pBuffer + p_uiOffset;
}

template <typename T, size_t Count>
void GlVertexAttribsBuffer<T, Count> :: Unlock()
{
	CheckGlError( glUnmapBuffer( GL_ARRAY_BUFFER), CU_T( "GlVertexAttribsBuffer :: Unlock - glUnmapBuffer"));
}

template <typename T, size_t Count>
void GlVertexAttribsBuffer<T, Count> :: SetShaderProgram( ShaderProgramPtr p_pProgram)
{
	GlShaderProgramPtr l_pProgram = static_pointer_cast<GlShaderProgram>( p_pProgram);

	if ((GlAttribsBase::m_pProgram.expired() && l_pProgram != NULL) || (l_pProgram == NULL && ! GlAttribsBase::m_pProgram.expired()) || GlAttribsBase::m_pProgram.lock() != l_pProgram)
	{
		GlAttribsBase::CleanupAttribute();
		_cleanupBufferObject();
		GlAttribsBase::m_pProgram = l_pProgram;
		Initialise( eBufferStatic);
	}
}

template <typename T, size_t Count>
void GlVertexAttribsBuffer<T, Count> :: _cleanupBufferObject()
{
	if (Buffer3D<T>::m_bAssigned)
	{
		BufferManager::UnassignBuffer<T>( m_uiIndex, this);
	}

	if (m_uiIndex != GL_INVALID_INDEX)// && glIsBuffer( m_uiIndex))
	{
		CheckGlError( glDeleteBuffers( 1, & m_uiIndex), CU_T( "GlVertexBufferObject :: Cleanup - glDeleteBuffers"));
	}

	m_uiIndex = GL_INVALID_INDEX;
	Buffer3D<T>::m_bAssigned = false;
}

template <typename T, size_t Count>
void GlVertexAttribsBuffer<T, Count> :: _initialiseBufferObject( eBUFFER_MODE p_eMode)
{
	CheckGlError( glGenBuffers( 1, & m_uiIndex), CU_T( "GlVertexBufferObject :: Initialise - glGenBuffers"));

	if (m_uiIndex != GL_INVALID_INDEX)// && glIsBuffer( m_uiIndex))
	{
		CheckGlError( glBindBuffer( GL_ARRAY_BUFFER, m_uiIndex), CU_T( "GlVertexBufferObject :: Initialise - glBindBuffer"));
		CheckGlError( glBufferData( GL_ARRAY_BUFFER, m_uiFilled * sizeof( T), m_pBuffer, GlEnum::GetBufferFlags( p_eMode)), CU_T( "GlVertexBufferObject :: Initialise - glBufferData"));
		CheckGlError( glBindBuffer( GL_ARRAY_BUFFER, 0), CU_T( "GlVertexBufferObject :: Initialise - glBindBuffer( NULL)"));
	}
	else
	{
		m_uiIndex = GL_INVALID_INDEX;
	}
}

template <typename T, size_t Count>
void GlVertexAttribsBuffer<T, Count> :: _activateBufferObject()
{
	CheckGlError( glBindBuffer( GL_ARRAY_BUFFER, m_uiIndex), CU_T( "GlVertexAttribsBuffer :: Activate - glBindBuffer"));
}

template <typename T, size_t Count>
void GlVertexAttribsBuffer<T, Count> ::_deactivateBufferObject()
{
}

//*************************************************************************************************
