//*************************************************************************************************

template <typename T, class D3dBufferObject>
Dx9VertexBufferObject<T, D3dBufferObject> :: Dx9VertexBufferObject( unsigned int & p_uiFilled, T *& p_pBuffer, const BufferDeclaration & p_declaration)
	:	m_uiIndex( BuffersCount)
	,	m_uiFilled( p_uiFilled)
	,	m_pBuffer( p_pBuffer)
	,	m_pBufferObject( NULL)
	,	m_declaration( p_declaration)
{
}

template <typename T, class D3dBufferObject>
Dx9VertexBufferObject<T, D3dBufferObject> :: ~Dx9VertexBufferObject()
{
	CleanupBufferObject();
}

template <typename T, class D3dBufferObject>
void Dx9VertexBufferObject<T, D3dBufferObject> :: CleanupBufferObject()
{
	if (m_pBufferObject != NULL)
	{
		m_pBufferObject->Release();
		m_pBufferObject = NULL;
	}
}

template <typename T, class D3dBufferObject>
void Dx9VertexBufferObject<T, D3dBufferObject> :: InitialiseBufferObject()
{
	CASTOR_ASSERT( FALSE);
}

template <typename T, class D3dBufferObject>
void Dx9VertexBufferObject<T, D3dBufferObject> :: ActivateBufferObject()
{
	CASTOR_ASSERT( FALSE);
}

//*************************************************************************************************