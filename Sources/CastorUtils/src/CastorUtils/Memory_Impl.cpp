#include "Memory_Impl.h"

#include "Memory.h"

using namespace General::Theory;
using namespace General::Utils;

#undef new

void * operator new( size_t p_size)
{
	void * l_ptr = malloc( p_size);

	if (MemoryManager::Exists())
	{
		boost::recursive_mutex::scoped_lock scoped_lock( MemoryManager::GetSingleton().m_mutex);

		if ( ! MemoryManager::IsLocked())
		{
			MemoryManager::Lock();

			if (l_ptr != NULL)
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

void * operator new[]( size_t p_size)
{
	void * l_ptr = malloc( p_size);
	
	if (MemoryManager::Exists())
	{
		boost::recursive_mutex::scoped_lock scoped_lock( MemoryManager::GetSingleton().m_mutex);

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

void operator delete( void * p_pointer)
{
	if (MemoryManager::Exists())
	{
		boost::recursive_mutex::scoped_lock scoped_lock( MemoryManager::GetSingleton().m_mutex);

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

void operator delete[]( void * p_pointer)
{
	if (MemoryManager::Exists())
	{
		boost::recursive_mutex::scoped_lock scoped_lock( MemoryManager::GetSingleton().m_mutex);

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
