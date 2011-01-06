//*************************************************************************************************

template <typename T>
void BufferManager::TBuffers<T> :: Cleanup()
{
	for (size_t i = 0 ; i < m_arrayBuffers.size() ; i++)
	{
		DeleteBuffer( m_arrayBuffers[i]);
	}

	m_arrayBuffers.clear();
	m_mapBuffersIndexes.clear();
	m_mapBuffers.clear();
}

template <typename T>
void BufferManager::TBuffers<T> :: Update()
{
	for (size_t i = 0 ; i < m_arrayBuffersToDelete.size() ; i++)
	{
		m_arrayBuffersToDelete[i]->Cleanup();
	}

	m_arrayBuffersToDelete.clear();
}

template <typename T>
bool BufferManager::TBuffers<T> :: AddBuffer( typename SmartPtr< Buffer3D<T> >::Shared p_pBuffer, size_t p_uiID)
{
	bool l_bReturn = false;

	CASTOR_ASSERT( ! p_pBuffer == NULL);

	if (m_mapBuffersIndexes.find( p_pBuffer.get()) == m_mapBuffersIndexes.end())
	{
		m_arrayBuffers.push_back( p_pBuffer);
		l_bReturn = true;
	}

	return l_bReturn;
}

template <typename T>
bool BufferManager::TBuffers<T> :: AssignBuffer( typename Buffer3D<T> * p_pBuffer, size_t p_uiID)
{
	bool l_bReturn = false;
	CASTOR_ASSERT( p_pBuffer != NULL && p_uiID != 0);

	if (m_mapBuffers.find( p_uiID) == m_mapBuffers.end())
	{
		for (size_t i = 0 ; i < m_arrayBuffers.size() && ! l_bReturn ; i++)
		{
			if (m_arrayBuffers[i].get() == p_pBuffer)
			{
				m_mapBuffers.insert( Buffer3DPtrUIntMap::value_type( p_uiID, p_pBuffer));
				m_mapBuffersIndexes.insert( UIntBuffer3DPtrMap::value_type( p_pBuffer, p_uiID));
				l_bReturn = true;
			}
		}
	}

	return l_bReturn;
}

template <typename T>
bool BufferManager::TBuffers<T> :: UnassignBuffer( typename Buffer3D<T> * p_pBuffer, size_t p_uiID)
{
	bool l_bReturn = false;
	CASTOR_ASSERT( p_pBuffer != NULL && p_uiID != 0);
	Buffer3DPtrUIntMap::iterator l_it = m_mapBuffers.find( p_uiID);

	if (l_it != m_mapBuffers.end())
	{
		l_bReturn = true;

		if (m_mapBuffersIndexes.find( p_pBuffer) != m_mapBuffersIndexes.end())
		{
			m_mapBuffersIndexes.erase( m_mapBuffersIndexes.find( p_pBuffer));
		}
	}

	return l_bReturn;
}

template <typename T>
bool BufferManager::TBuffers<T> :: DeleteBuffer( typename SmartPtr< Buffer3D<T> >::Shared p_pBuffer, size_t p_uiID)
{
	bool l_bReturn = false;
	CASTOR_ASSERT( ! p_pBuffer == NULL);
	UIntBuffer3DPtrMap::iterator l_it = m_mapBuffersIndexes.find( p_pBuffer.get());

	if (l_it != m_mapBuffersIndexes.end())
	{
		size_t l_uiID = l_it->second;
		vector::eraseValue( m_arrayBuffers, p_pBuffer);
		Buffer3DPtrUIntMap::iterator l_it2 = m_mapBuffers.find( l_it->second);

		if (l_it2 != m_mapBuffers.end())
		{
			if (p_pBuffer->IsAssigned())
			{
				UnassignBuffer( p_pBuffer.get(), l_uiID);
			}

			m_mapBuffers.erase( m_mapBuffers.find( l_uiID));
		}

		m_arrayBuffersToDelete.push_back( p_pBuffer);
	}

	return l_bReturn;
}

//*************************************************************************************************

template <typename T, typename _Ty>
typename SmartPtr<_Ty>::Shared BufferManager :: CreateBuffer()
{
	SmartPtr<_Ty>::Shared l_pBuffer = SmartPtr<_Ty>::Shared( new _Ty);

	if ( ! _applyFunction( SmartPtr< Buffer3D<T> >::Shared( l_pBuffer), 0, & TBuffers<T>::AddBuffer))
	{
		l_pBuffer.reset();
	}

	return l_pBuffer;
}

template <typename T, typename _Ty>
typename SmartPtr<_Ty>::Shared BufferManager :: CreateBuffer( const String & p_strArg)
{
	SmartPtr<_Ty>::Shared l_pBuffer = SmartPtr<_Ty>::Shared( new _Ty( p_strArg));

	if ( ! _applyFunction( SmartPtr< Buffer3D<T> >::Shared( l_pBuffer), 0, & TBuffers<T>::AddBuffer))
	{
		l_pBuffer.reset();
	}

	return l_pBuffer;
}

template <typename T>
bool BufferManager :: AssignBuffer( size_t p_uiID, Buffer3D<T> * p_pBuffer)
{
	return _applyFunctionPtr( p_pBuffer, p_uiID, & TBuffers<T>::AssignBuffer);
}

template <typename T>
bool BufferManager :: UnassignBuffer( size_t p_uiID, Buffer3D<T> * p_pBuffer)
{
	return _applyFunctionPtr( p_pBuffer, p_uiID, & TBuffers<T>::UnassignBuffer);
}

template <typename T>
bool BufferManager :: DeleteBuffer( typename SmartPtr< Buffer3D<T> >::Shared p_pBuffer)
{
	return _applyFunction( p_pBuffer, 0, & TBuffers<T>::DeleteBuffer);
}

template <typename T>
bool BufferManager :: _applyFunction( typename SmartPtr< Buffer3D<T> >::Shared p_pBuffer, size_t p_uiID,
							bool( TBuffers<T>::* p_function)( typename SmartPtr< Buffer3D<T> >::Shared, size_t))
{
	BufferManager & l_pThis = GetSingleton();
	return l_pThis._apply( p_pBuffer, p_uiID, p_function);
}
template <typename T>
bool BufferManager :: _applyFunctionPtr( Buffer3D<T> * p_pBuffer, size_t p_uiID, bool( TBuffers<T>::* p_function)( Buffer3D<T> *, size_t))
{
	BufferManager & l_pThis = GetSingleton();
	return l_pThis._applyPtr( p_pBuffer, p_uiID, p_function);
}

template <> bool BufferManager :: _apply( SmartPtr< Buffer3D<float> >::Shared p_pBuffer, size_t p_uiID, bool( TBuffers<float>::* p_function)( SmartPtr< Buffer3D<float> >::Shared, size_t))
{
	return (m_stFloatBuffers.*p_function)( p_pBuffer, p_uiID);
}

template <> bool BufferManager :: _apply( SmartPtr< Buffer3D<double> >::Shared p_pBuffer, size_t p_uiID, bool( TBuffers<double>::* p_function)( SmartPtr< Buffer3D<double> >::Shared, size_t))
{
	return (m_stDoubleBuffers.*p_function)( p_pBuffer, p_uiID);
}

template <> bool BufferManager :: _apply( SmartPtr< Buffer3D<int> >::Shared p_pBuffer, size_t p_uiID, bool( TBuffers<int>::* p_function)( SmartPtr< Buffer3D<int> >::Shared, size_t))
{
	return (m_stIntBuffers.*p_function)( p_pBuffer, p_uiID);
}

template <> bool BufferManager :: _apply( SmartPtr< Buffer3D<size_t> >::Shared p_pBuffer, size_t p_uiID, bool( TBuffers<size_t>::* p_function)( SmartPtr< Buffer3D<size_t> >::Shared, size_t))
{
	return (m_stUIntBuffers.*p_function)( p_pBuffer, p_uiID);
}

template <> bool BufferManager :: _apply( SmartPtr< Buffer3D<bool> >::Shared p_pBuffer, size_t p_uiID, bool( TBuffers<bool>::* p_function)( SmartPtr< Buffer3D<bool> >::Shared, size_t))
{
	return (m_stBoolBuffers.*p_function)( p_pBuffer, p_uiID);
}

template <> bool BufferManager :: _apply( SmartPtr< Buffer3D<char> >::Shared p_pBuffer, size_t p_uiID, bool( TBuffers<char>::* p_function)( SmartPtr< Buffer3D<char> >::Shared, size_t))
{
	return (m_stCharBuffers.*p_function)( p_pBuffer, p_uiID);
}

template <> bool BufferManager :: _apply( SmartPtr< Buffer3D<unsigned char> >::Shared p_pBuffer, size_t p_uiID, bool( TBuffers<unsigned char>::* p_function)( SmartPtr< Buffer3D<unsigned char> >::Shared, size_t))
{
	return (m_stUCharBuffers.*p_function)( p_pBuffer, p_uiID);
}

template <> bool BufferManager :: _apply( SmartPtr< Buffer3D<long> >::Shared p_pBuffer, size_t p_uiID, bool( TBuffers<long>::* p_function)( SmartPtr< Buffer3D<long> >::Shared, size_t))
{
	return (m_stLongBuffers.*p_function)( p_pBuffer, p_uiID);
}

template <> bool BufferManager :: _apply( SmartPtr< Buffer3D<unsigned long> >::Shared p_pBuffer, size_t p_uiID, bool( TBuffers<unsigned long>::* p_function)( SmartPtr< Buffer3D<unsigned long> >::Shared, size_t))
{
	return (m_stULongBuffers.*p_function)( p_pBuffer, p_uiID);
}

template <> bool BufferManager :: _apply( SmartPtr< Buffer3D<long long> >::Shared p_pBuffer, size_t p_uiID, bool( TBuffers<long long>::* p_function)( SmartPtr< Buffer3D<long long> >::Shared, size_t))
{
	return (m_stLongLongBuffers.*p_function)( p_pBuffer, p_uiID);
}

template <> bool BufferManager :: _apply( SmartPtr< Buffer3D<unsigned long long> >::Shared p_pBuffer, size_t p_uiID, bool( TBuffers<unsigned long long>::* p_function)( SmartPtr< Buffer3D<unsigned long long> >::Shared, size_t))
{
	return (m_stULongLongBuffers.*p_function)( p_pBuffer, p_uiID);
}

template <> bool BufferManager :: _applyPtr( Buffer3D<float> * p_pBuffer, size_t p_uiID, bool( TBuffers<float>::* p_function)( Buffer3D<float> *, size_t))
{
	return (m_stFloatBuffers.*p_function)( p_pBuffer, p_uiID);
}

template <> bool BufferManager :: _applyPtr( Buffer3D<double> * p_pBuffer, size_t p_uiID, bool( TBuffers<double>::* p_function)( Buffer3D<double> *, size_t))
{
	return (m_stDoubleBuffers.*p_function)( p_pBuffer, p_uiID);
}

template <> bool BufferManager :: _applyPtr( Buffer3D<int> * p_pBuffer, size_t p_uiID, bool( TBuffers<int>::* p_function)( Buffer3D<int> *, size_t))
{
	return (m_stIntBuffers.*p_function)( p_pBuffer, p_uiID);
}

template <> bool BufferManager :: _applyPtr( Buffer3D<size_t> * p_pBuffer, size_t p_uiID, bool( TBuffers<size_t>::* p_function)( Buffer3D<size_t> *, size_t))
{
	return (m_stUIntBuffers.*p_function)( p_pBuffer, p_uiID);
}

template <> bool BufferManager :: _applyPtr( Buffer3D<bool> * p_pBuffer, size_t p_uiID, bool( TBuffers<bool>::* p_function)( Buffer3D<bool> *, size_t))
{
	return (m_stBoolBuffers.*p_function)( p_pBuffer, p_uiID);
}

template <> bool BufferManager :: _applyPtr( Buffer3D<char> * p_pBuffer, size_t p_uiID, bool( TBuffers<char>::* p_function)( Buffer3D<char> *, size_t))
{
	return (m_stCharBuffers.*p_function)( p_pBuffer, p_uiID);
}

template <> bool BufferManager :: _applyPtr( Buffer3D<unsigned char> * p_pBuffer, size_t p_uiID, bool( TBuffers<unsigned char>::* p_function)( Buffer3D<unsigned char> *, size_t))
{
	return (m_stUCharBuffers.*p_function)( p_pBuffer, p_uiID);
}

template <> bool BufferManager :: _applyPtr( Buffer3D<long> * p_pBuffer, size_t p_uiID, bool( TBuffers<long>::* p_function)( Buffer3D<long> *, size_t))
{
	return (m_stLongBuffers.*p_function)( p_pBuffer, p_uiID);
}

template <> bool BufferManager :: _applyPtr( Buffer3D<unsigned long> * p_pBuffer, size_t p_uiID, bool( TBuffers<unsigned long>::* p_function)( Buffer3D<unsigned long> *, size_t))
{
	return (m_stULongBuffers.*p_function)( p_pBuffer, p_uiID);
}

template <> bool BufferManager :: _applyPtr( Buffer3D<long long> * p_pBuffer, size_t p_uiID, bool( TBuffers<long long>::* p_function)( Buffer3D<long long> *, size_t))
{
	return (m_stLongLongBuffers.*p_function)( p_pBuffer, p_uiID);
}

template <> bool BufferManager :: _applyPtr( Buffer3D<unsigned long long> * p_pBuffer, size_t p_uiID, bool( TBuffers<unsigned long long>::* p_function)( Buffer3D<unsigned long long> *, size_t))
{
	return (m_stULongLongBuffers.*p_function)( p_pBuffer, p_uiID);
}