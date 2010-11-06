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
#ifndef ___Castor_ScopedPtr___
#define ___Castor_ScopedPtr___

#include "Module_Utils.h"

namespace Castor
{	namespace Templates
{
	/*!
	 ScopedPtr template class stores a pointer to a dynamically allocated object
	 Only holds the pointer and doesn't share it (and so can't be put in std containers)
	 */
	template <typename T>
	class ScopedPtr
	{
	protected:
		T * m_pPointer;

	private:
		// The copy constructor and the two assignment operators are private to forbid their use
		ScopedPtr( const ScopedPtr & p_scopedPtr){}
		ScopedPtr & operator =( const ScopedPtr & p_scopedPtr){}
		ScopedPtr & operator =( T * p_pPointer){}

	public:
		ScopedPtr( T * p_pPointer=NULL)throw()
			:	m_pPointer( p_pPointer)
		{
		}
		~ScopedPtr()throw()
		{
			_release();
		}

		T & operator *()throw()
		{
			return * m_pPointer;
		}
		T * operator ->()throw()
		{
			return m_pPointer;
		}
		const T & operator *()const throw()
		{
			return * m_pPointer;
		}
		const T * operator ->()const throw()
		{
			return m_pPointer;
		}
		T * get()const throw()
		{
			return m_pPointer;
		}

		void reset( T * pPointer=NULL)throw()
		{
			_release();
			m_pPointer = pPointer;
		}
		void swap( ScopedPtr & p_scopedPtr)throw()
		{
			std::swap( m_pPointer, p_scopedPtr.m_pPointer);
		}

	private:
		void _release()throw()
		{
			delete m_pPointer;
			m_pPointer = NULL;
		}
	};

	template <class T>
	void swap( ScopedPtr<T> & a, ScopedPtr<T> & b)throw()
	{
		a.swap( b);
	}
}
}

#endif