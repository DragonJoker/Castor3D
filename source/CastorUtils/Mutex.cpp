#include "CastorUtils/PrecompiledHeader.hpp"

#include "CastorUtils/Mutex.hpp"

#if CHECK_MEMORYLEAKS
#	include "CastorUtils/Memory.hpp"
using namespace Castor::Utils;
#endif

using namespace Castor::MultiThreading;

//*************************************************************************************************

Mutex :: Mutex()
{
}

Mutex :: ~Mutex()
{
}

void Mutex :: Lock()const
{
	m_mutex.lock();
}

bool Mutex :: TryLock()const
{
	return m_mutex.try_lock();
}

void Mutex :: Unlock()const
{
	m_mutex.unlock();
}

//*************************************************************************************************

RecursiveMutex :: RecursiveMutex()
{
}

RecursiveMutex :: ~RecursiveMutex()
{
}

void RecursiveMutex :: Lock()const
{
	m_mutex.lock();
}

bool RecursiveMutex :: TryLock()const
{
	return m_mutex.try_lock();
}

void RecursiveMutex :: Unlock()const
{
	m_mutex.unlock();
}

//*************************************************************************************************
