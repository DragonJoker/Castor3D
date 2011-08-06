//*************************************************************************************************

template <typename T> GLenum _getGlType()
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

template <typename T>
GlBuffer<T> :: GlBuffer( unsigned int & p_uiFilled, T *& p_pBuffer, GLenum p_target)
	:	m_uiFilled( p_uiFilled)
	,	m_pBuffer( p_pBuffer)
	,	m_glBuffer( p_target)
{
}

template <typename T>
GlBuffer<T> :: ~GlBuffer()
{
}

template <typename T>
void GlBuffer<T> :: BufferObjectCleanup( bool & p_bAssigned, Buffer3D<T> * p_pBuffer)
{
	if (p_bAssigned)
	{
		Root::GetSingleton()->GetBufferManager()->UnassignBuffer<T>( m_glBuffer.GetGlIndex(), p_pBuffer);
		p_bAssigned = false;
	}

	m_glBuffer.Cleanup();
}

template <typename T>
bool GlBuffer<T> :: BufferObjectInitialise( eBUFFER_MODE p_eMode, Buffer3D<T> * p_pBuffer)
{
	bool l_bReturn = m_glBuffer.Initialise( m_pBuffer, m_uiFilled * sizeof( T), p_eMode);

	if (l_bReturn)
	{
		Root::GetSingleton()->GetBufferManager()->AssignBuffer<T>( m_glBuffer.GetGlIndex(), p_pBuffer);
	}

	return l_bReturn;
}

template <typename T>
bool GlBuffer<T> :: BufferObjectBind()
{
	return m_glBuffer.Bind();
}

template <typename T>
bool GlBuffer<T> :: BufferObjectUnbind()
{
	return m_glBuffer.Unbind();
}

template <typename T>
bool GlBuffer<T> :: BufferObjectFill( void * p_pBuffer, GLsizeiptr p_iSize, GLenum p_mode)
{
	return m_glBuffer.Fill( p_pBuffer, p_iSize, p_mode);
}

template <typename T>
void * GlBuffer<T> :: BufferObjectLock( size_t p_uiOffset, size_t p_uiSize, size_t p_uiFlags)
{
	return m_glBuffer.Lock( p_uiOffset, p_uiSize, p_uiFlags);
}

template <typename T>
void * GlBuffer<T> :: BufferObjectLock( GLenum p_access)
{
	return m_glBuffer.Lock( p_access);
}

template <typename T>
bool GlBuffer<T> :: BufferObjectUnlock()
{
	return m_glBuffer.Unlock();
}

//*************************************************************************************************

template <typename T, size_t Count>
GlAttribute<T, Count> :: GlAttribute( String const & p_strAttribName)
	:	GlAttributeBase( p_strAttribName, _getGlType<T>(), Count)
{
}

template <typename T, size_t Count>
GlAttribute<T, Count> :: ~GlAttribute()
{
}

//*************************************************************************************************
template <typename T>
shared_ptr<T> GlUniformBufferObject :: CreateVariable( String const & p_strName, size_t p_uiCount)
{
	shared_ptr<T> l_pReturn;

	if (m_mapVariables.find( p_strName) != m_mapVariables.end())
	{
		l_pReturn = static_pointer_cast<T>( m_mapVariables.find( p_strName)->second);
	}
	else
	{
		l_pReturn = shared_ptr<T>( new T( this, p_strName, p_uiCount));

		if (AddVariable( l_pReturn))
		{
			l_pReturn->SetBuffer( & m_buffer[l_pReturn->m_iOffset]);
		}
		else
		{
			l_pReturn.reset();
		}
	}

	return l_pReturn;
}

template <typename T>
shared_ptr<T> GlUniformBufferObject :: GetVariable( String const & p_strName)
{
	shared_ptr<T> l_pReturn;

	if (m_mapVariables.find( p_strName) != m_mapVariables.end())
	{
		l_pReturn = static_pointer_cast<T>( m_mapVariables.find( p_strName)->second);
	}

	return l_pReturn;
}

//*************************************************************************************************

template <typename T>
GlUboOneVariable<T> :: GlUboOneVariable( GlUniformBufferObject * p_pParent, String const & p_strName, size_t p_uiCount)
	:	GlUboVariableBase( p_pParent, p_strName, sizeof( T), p_uiCount)
{
	m_tValue = new T*[p_uiCount];
}

template <typename T>
GlUboOneVariable<T> :: ~GlUboOneVariable()
{
	delete [] m_tValue;
}

template <typename T>
void GlUboOneVariable<T> :: SetValue( T const & p_tValue, size_t p_uiIndex)
{
	m_pParent->SetModified( m_pParent->IsModified() || *(m_tValue[p_uiIndex]) != p_tValue);
	*(m_tValue[p_uiIndex]) = p_tValue;
}

template <typename T>
void GlUboOneVariable<T> :: SetValues( T * p_tValues)
{
	for (size_t i = 0 ; i < m_uiCount ; i++)
	{
		m_pParent->SetModified( m_pParent->IsModified() || *(m_tValue[i]) != p_tValues[i]);
		*(m_tValue[i]) = p_tValues[i];
	}
}

template <typename T>
void GlUboOneVariable<T> :: SetBuffer( void * p_pBuffer)
{
	GlUboVariableBase::SetBuffer( p_pBuffer);

	for (size_t i = 0 ; i < m_uiCount ; i++)
	{
		m_tValue[i] = & ((T *)m_pBuffer)[i];
	}
}

//*************************************************************************************************

template <typename T, size_t Count>
GlUboPointVariable<T, Count> :: GlUboPointVariable( GlUniformBufferObject * p_pParent, String const & p_strName, size_t p_uiCount)
	:	GlUboVariableBase( p_pParent, p_strName, sizeof( T) * Count, p_uiCount)
{
	m_ptValue = new Point<T, Count>[p_uiCount];
}

template <typename T, size_t Count>
GlUboPointVariable<T, Count> :: ~GlUboPointVariable()
{
	delete [] m_ptValue;
}

template <typename T, size_t Count>
void GlUboPointVariable<T, Count> :: SetValue( Point<T, Count> const & p_ptValue, size_t p_uiIndex)
{
	m_pParent->SetModified( m_pParent->IsModified() || m_ptValue[p_uiIndex] != p_ptValue);
	m_ptValue[p_uiIndex] = p_ptValue;
}

template <typename T, size_t Count>
void GlUboPointVariable<T, Count> :: SetValues( Point<T, Count> * p_ptValues)
{
	for (size_t i = 0 ; i < m_uiCount ; i++)
	{
		m_pParent->SetModified( m_pParent->IsModified() || m_ptValue[i] != p_ptValues[i]);
		m_ptValue[i] = p_ptValues[i];
	}
}

template <typename T, size_t Count>
void GlUboPointVariable<T, Count> :: SetBuffer( void * p_pBuffer)
{
	GlUboVariableBase::SetBuffer( p_pBuffer);

	for (size_t i = 0 ; i < m_uiCount ; i++)
	{
		m_ptValue[i].LinkCoords( & ((T *)p_pBuffer)[i * Count]);
	}
}

//*************************************************************************************************

template <typename T, size_t Rows, size_t Columns>
GlUboMatrixVariable<T, Rows, Columns> :: GlUboMatrixVariable( GlUniformBufferObject * p_pParent, String const & p_strName, size_t p_uiCount)
	:	GlUboVariableBase( p_pParent, p_strName, sizeof( T) * Rows * Columns, p_uiCount)
{
	m_mtxValue = new Matrix<T, Rows, Columns>[p_uiCount];
}

template <typename T, size_t Rows, size_t Columns>
GlUboMatrixVariable<T, Rows, Columns> :: ~GlUboMatrixVariable()
{
	delete [] m_mtxValue;
}

template <typename T, size_t Rows, size_t Columns>
void GlUboMatrixVariable<T, Rows, Columns> :: SetValue( Matrix<T, Rows, Columns> const & p_mtxValue, size_t p_uiIndex)
{
	m_pParent->SetModified( m_pParent->IsModified() || m_mtxValue[p_uiIndex] != p_mtxValue);
	m_mtxValue[p_uiIndex] = p_mtxValue;
}

template <typename T, size_t Rows, size_t Columns>
void GlUboMatrixVariable<T, Rows, Columns> :: SetValues( Matrix<T, Rows, Columns> * p_mtxValues)
{
	for (size_t i = 0 ; i < m_uiCount ; i++)
	{
		m_pParent->SetModified( m_pParent->IsModified() || m_mtxValue[i] != p_mtxValues[i]);
		m_mtxValue[i] = p_mtxValues[i];
	}
}

template <typename T, size_t Rows, size_t Columns>
void GlUboMatrixVariable<T, Rows, Columns> :: SetBuffer( void * p_pBuffer)
{
	GlUboVariableBase::SetBuffer( p_pBuffer);

	for (size_t i = 0 ; i < m_uiCount ; i++)
	{
		m_mtxValue[i].LinkCoords( & ((T *)p_pBuffer)[i * Rows * Columns]);
	}
}

//*************************************************************************************************