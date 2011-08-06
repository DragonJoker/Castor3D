//*************************************************************************************************

template <typename T, class D3dBufferObject>
Dx9BufferObject<T, D3dBufferObject> :: Dx9BufferObject( unsigned int & p_uiFilled, T *& p_pBuffer)
	:	m_uiIndex( BuffersCount)
	,	m_uiFilled( p_uiFilled)
	,	m_pBuffer( p_pBuffer)
	,	m_pBufferObject( NULL)
{
}

template <typename T, class D3dBufferObject>
Dx9BufferObject<T, D3dBufferObject> :: ~Dx9BufferObject()
{
}

template <typename T, class D3dBufferObject>
void Dx9BufferObject<T, D3dBufferObject> :: BufferCleanup( bool p_bAssigned, Buffer3D<T> * p_pBuffer)
{
	if (p_bAssigned)
	{
		Root::GetSingleton()->GetBufferManager()->UnassignBuffer<T>( m_uiIndex, p_pBuffer);
	}

	if (m_pBufferObject != NULL)
	{
		m_pBufferObject->Release();
		m_pBufferObject = NULL;
	}
}

template <typename T, class D3dBufferObject>
void * Dx9BufferObject<T, D3dBufferObject> :: BufferLock( size_t p_uiOffset, size_t p_uiSize, size_t p_uiFlags)
{
	void * l_pData = NULL;
	CheckDxError( m_pBufferObject->Lock( p_uiOffset, p_uiSize, & l_pData, D3dEnum::GetLockFlags( p_uiFlags)), "Dx9BufferObject :: Lock", false);
	return l_pData;
}

template <typename T, class D3dBufferObject>
void Dx9BufferObject<T, D3dBufferObject> :: BufferUnlock()
{
	CheckDxError( m_pBufferObject->Unlock(), "Dx9BufferObject :: Unlock", false);
}

//*************************************************************************************************
