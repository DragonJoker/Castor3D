#include "CastorUtils/PrecompiledHeader.hpp"

#include "CastorUtils/Memory.hpp"

using namespace Castor::Theory;
using namespace Castor::Utils;

#undef new
/*
void * operator new( size_t p_size)
{
	void * l_ptr = malloc( p_size);

	if (MemoryManager::Exists())
	{
		CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( MemoryManager::GetSingleton().m_mutex);

		if ( ! MemoryManager::IsLocked())
		{
			MemoryManager::Lock();

			if (l_ptr)
			{
				MemoryManager::GetSingleton().AddLocation( p_size, l_ptr, false);
			}
			else
			{
				MemoryManager::GetSingleton().FailedAlloc( p_size, false);
			}

			MemoryManager::Unlock();
		}
	}

	return l_ptr;
}

void operator delete( void * p_pointer)
{
	if (MemoryManager::Exists())
	{
		CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( MemoryManager::GetSingleton().m_mutex);

		if ( ! MemoryManager::IsLocked())
		{
			MemoryManager::Lock();

			if (MemoryManager::GetSingleton().RemoveLocation( p_pointer, false))
			{
				free( p_pointer);
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
/**/
/*
void * operator new[]( size_t p_size)
{
	void * l_ptr = malloc( p_size);

	if (MemoryManager::Exists())
	{
		CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( MemoryManager::GetSingleton().m_mutex);

		if ( ! MemoryManager::IsLocked())
		{
			MemoryManager::Lock();

			if (l_ptr)
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

void operator delete[]( void * p_pointer)
{
	if (MemoryManager::Exists())
	{
		CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( MemoryManager::GetSingleton().m_mutex);

		if ( ! MemoryManager::IsLocked())
		{
			MemoryManager::Lock();

			if (MemoryManager::GetSingleton().RemoveLocation( p_pointer, true))
			{
				free( p_pointer);
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
/**/
