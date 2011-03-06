//*************************************************************************************************

template <class T>
void * MemoryTraced<T> :: Alloc( size_t p_size)
{
	return TAlloc<T>( p_size);
}

template <class T>
void MemoryTraced<T> :: Dealloc( void * p_pointer)
{
	TDealloc<T>( p_pointer);
}

template <class T>
void * MemoryTraced<T> :: AllocArray( size_t p_size)
{
	return TAllocArray<T>( p_size);
}

template <class T>
void MemoryTraced<T> :: DeallocArray( void * p_pointer)
{
	TDeallocArray<T>( p_pointer);
}

#ifndef ____CASTOR_NO_MEMORY_DEBUG____

//*************************************************************************************************

template <class T>
void * TAlloc( size_t p_size)
{
	T * l_ptr = (T *)malloc( p_size);
	Castor::Utils::File l_file( ALLOG_LOCATION, Castor::Utils::File::eAppend);
	MemoryTracedBase::TotalAllocatedSize += p_size;
	MemoryTracedBase::TotalAllocatedObjectCount++;
	l_file << "TAlloc" << " - " << l_ptr << " - " << p_size << " -\t" << typeid( T).name() << "\n";

	if (Castor::MemoryManager::Exists())
	{
		if ( ! Castor::MemoryManager::IsLocked())
		{
			Castor::MemoryManager::Lock();

			if (l_ptr != NULL)
			{
				Castor::MemoryManager::GetSingleton().AddLocation( p_size, l_ptr, false);
			}
			else
			{
				Castor::MemoryManager::GetSingleton().FailedAlloc( p_size, false);
			}

			Castor::MemoryManager::Unlock();
		}
	}

	return l_ptr;
}

template <class T>
void TDealloc( void * p_pointer)
{
	Castor::Utils::File l_file( DEALLOG_LOCATION, Castor::Utils::File::eAppend);
	l_file << "TDealloc" << " - " << p_pointer << " - " << sizeof( T) << " -\t" << typeid( T).name() << "\n";

	if (MemoryManager::Exists())
	{
		if ( ! Castor::MemoryManager::IsLocked())
		{
			Castor::MemoryManager::Lock();

			if (Castor::MemoryManager::GetSingleton().RemoveLocation( p_pointer, false))
			{
				free( p_pointer);
			}
			else
			{
			}

			Castor::MemoryManager::Unlock();
		}
		else
		{
			free( p_pointer);
		}
	}
	else
	{
		free( p_pointer);
	}
}

template <class T>
void * TAllocArray( size_t p_size)
{
	MemoryTracedBase::TotalAllocatedSize += p_size;
	MemoryTracedBase::TotalAllocatedArrayCount++;
	T * l_ptr = (T *)malloc( p_size);
	Castor::Utils::File l_file( ALLOG_LOCATION, Castor::Utils::File::eAppend);
	l_file << "TAllocArray" << " - " << l_ptr << " - " << p_size << " - " << typeid( T).name() << "\n";

	if (MemoryManager::Exists())
	{
		if ( ! MemoryManager::IsLocked())
		{
			MemoryManager::Lock();

			if (l_ptr != NULL)
			{
				MemoryManager::GetSingleton().AddLocation( p_size, l_ptr, true);
			}
			else
			{
				MemoryManager::GetSingleton().FailedAlloc( p_size, true);
			}

			MemoryManager::Unlock();
		}
	}

	return l_ptr;
}

template <class T>
void TDeallocArray( void * p_pointer)
{
	Castor::Utils::File l_file( DEALLOG_LOCATION, Castor::Utils::File::eAppend);
	l_file << "TDeallocArray" << " - " << p_pointer << " - " << typeid( T).name() << "\n";

	if (MemoryManager::Exists())
	{
		if ( ! MemoryManager::IsLocked())
		{
			MemoryManager::Lock();

			if (MemoryManager::GetSingleton().RemoveLocation( p_pointer, true))
			{
				free( p_pointer);
			}
			else
			{
			}

			MemoryManager::Unlock();
		}
		else
		{
			free( p_pointer);
		}
	}
	else
	{
		free( p_pointer);
	}
}

//*************************************************************************************************

template <class T>
Castor::MemoryBlockImpl<T> :: MemoryBlockImpl()
	:	ptr( NULL)
{
}

template <class T>
Castor::MemoryBlockImpl<T> :: MemoryBlockImpl( const char * p_file, const char * p_function, unsigned int p_line)
	:	MemoryBlockBase( p_file, p_function, p_line)
	,	ptr( NULL)
{
}

template <class T>
Castor::MemoryBlockImpl<T> :: MemoryBlockImpl( T * p_ptr, size_t p_size, bool p_array)
	:	MemoryBlockBase( p_size, p_array)
	,	ptr( p_ptr)
{
}

template <class T>
const char * Castor::MemoryBlockImpl<T> :: GetBlockType()const
{
	return typeid( T).name();
}

template <class T>
void Castor::MemoryBlockImpl<T> :: Clear()
{
	MemoryBlockBase::Clear();
	ptr = NULL;
}

//*************************************************************************************************

template <typename T>
void Castor::MemoryManager :: AddLocation( size_t p_size, T * p_pointer, bool p_typeArray)
{
	RecordAllocation( p_size);
	_addMemoryBlock( p_pointer, new MemoryBlockImpl<T>( p_pointer, p_size, p_typeArray));
}

template <typename T>
T * Castor::MemoryManager :: operator <<( T * p_ptr)
{
	return static_cast<T *>( _parse( p_ptr));
}

//*************************************************************************************************

#else

//*************************************************************************************************

template <class T>
void * TAlloc( size_t p_size)
{
	void * l_pointer = malloc( p_size);

	if (l_pointer == NULL)
	{
		throw std::bad_alloc();
	}

	return l_pointer;
}

template <class T>
void TDealloc( void * p_pointer)
{
	free( p_pointer);
}

template <class T>
void * TAllocArray( size_t p_size)
{
	void * l_pointer = malloc( p_size);

	if (l_pointer == NULL)
	{
		throw std::bad_alloc();
	}

	return l_pointer;
}

template <class T>
void TDeallocArray( void * p_pointer)
{
	free( p_pointer);
}

//*************************************************************************************************

#endif