#include "Mutex.h"

using namespace General::MultiThreading;

#if GENLIB_MT_USE_MFC && ! defined( __GNUG__)

MutexScopedLock :: MutexScopedLock( Mutex & p_mutex, bool p_startLocked)
	:	m_mutex		(p_mutex),
		m_locked	(false)
{
	if (p_startLocked)
	{
		Lock();
	}
}
MutexScopedLock :: ~MutexScopedLock()
{
	if (m_locked)
	{
		Unlock();
	}
}

void MutexScopedLock :: Lock()
{
	if ( ! m_locked)
	{
		m_locked = true;
		m_mutex.Lock();
	}
}

void MutexScopedLock :: Unlock()
{
	if (m_locked)
	{
		m_locked = false;
		m_mutex.Unlock();
	}
}

#endif