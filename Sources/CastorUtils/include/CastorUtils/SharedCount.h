/*
This source file is part of Castor3D (http://dragonjoker.co.cc

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___Castor_SharedCount___
#define ___Castor_SharedCount___

#include "MultiThreadConfig.h"

#if CASTOR_MT_USE_BOOST
#	define CASTOR_INTERLOCKED_INCREMENT( x)		BOOST_INTERLOCKED_INCREMENT( x)
#	define CASTOR_INTERLOCKED_DECREMENT( x)		BOOST_INTERLOCKED_DECREMENT( x)
#elif CASTOR_MT_USE_MFC
#	define CASTOR_INTERLOCKED_INCREMENT( x)		_InterlockedIncrement( x)
#	define CASTOR_INTERLOCKED_DECREMENT( x)		_InterlockedDecrement( x)
#elif CASTOR_MT_USE_WIN32
#	include <Windows.h>
#	define CASTOR_INTERLOCKED_INCREMENT( x)		InterlockedIncrement( x)
#	define CASTOR_INTERLOCKED_DECREMENT( x)		InterlockedDecrement( x)
#else
#	define CASTOR_INTERLOCKED_INCREMENT( x)		( * x)++
#	define CASTOR_INTERLOCKED_DECREMENT( x)		( * x)--
#endif

namespace Castor
{	namespace Templates
{
	class SharedCount
	{
	private:
		long * m_pCount;

	public:
		SharedCount()throw()
			:	m_pCount( NULL)
		{
		}

		template <typename T>
		explicit SharedCount( T * p_pPointer)
			:	m_pCount( NULL)
		{
			m_pCount = new CountedImpl<Y>( p_pPointer);

			if (m_pCount == NULL)
			{
				delete p_pPointer;
				throw std::bad_alloc();
			}
		}

		SharedCount( const SharedCount & p_sharedCount)throw()
			:	m_pCount( p_sharedCount.m_pCount)
		{
		}
		~SharedCount()throw()
		{
			unref();
		}
		void ref()throw()
		{
			if (m_pCount == NULL)
			{
				try
				{
					m_pCount = new long( 1);
				}
				catch (std::bad_alloc)
				{
					m_pCount = NULL;
				}
			}
			else
			{
				CASTOR_INTERLOCKED_INCREMENT( m_pCount);
			}
		}
		void unref()throw()
		{
			if (m_pCount != NULL && (* m_pCount) > 0)
			{
				CASTOR_INTERLOCKED_DECREMENT( m_pCount);
			}

			_erase();
		}
		long count()const throw()
		{
			long l_iReturn = 0;

			if (m_pCount != NULL)
			{
				l_iReturn = * m_pCount;
			}

			return l_iReturn;
		}
		void reset()throw()
		{
			m_pCount = NULL;
		}
		SharedCount & operator =( const SharedCount & p_sharedCount)throw()
		{
			m_pCount = p_sharedCount.m_pCount;
			return *this;
		}
		void swap( SharedCount & p_sharedCount)throw()
		{
			std::swap( m_pCount, p_sharedCount.m_pCount);
		}

	private:
		void _erase()
		{
			if (m_pCount != NULL && (* m_pCount) == 0)
			{
				delete m_pCount;
				m_pCount = NULL;
			}
		}
	};
}
}

#endif