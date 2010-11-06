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
#ifndef ___Castor_ScopedArray___
#define ___Castor_ScopedArray___

#include "Module_Utils.h"

namespace Castor
{	namespace Templates
{
	/*!
	 ScopedArray template class stores a pointer to a dynamically allocated object array
	 Only holds the pointer and doesn't share it (and so can't be put in std containers)
	 */
	template <typename T>
	class ScopedArray
	{
	protected:
		T * m_pArray;

	private:
		// The copy constructor and the two assignment operators are private to forbid their use
		ScopedArray( const ScopedArray & p_scopedArray){}
		ScopedArray & operator =( const ScopedArray & p_scopedArray){}
		ScopedArray & operator =( T * p_pArray){}

	public:
		ScopedArray( T * p_pArray = NULL)throw()
			:	m_pArray( p_pArray)
		{
		}
		~ScopedArray()throw()
		{
			_release();
		}

		T & operator *()throw()
		{
			return * m_pArray;
		}
		T & operator []( size_t p_iIndex)
		{
			return m_pArray[p_iIndex];
		}
		const T & operator *()const throw()
		{
			return * m_pArray;
		}
		const T & operator []( size_t p_iIndex)const
		{
			return m_pArray[p_iIndex];
		}
		void reset( T * p_pArray=NULL)throw()
		{
			_release();
			m_pArray = p_pArray;
		}
		T * get()const throw()
		{
			return m_pArray;
		}
		void swap( ScopedArray & p_scopedArray)throw()
		{
			std::swap( m_pArray, p_scopedArray.m_pArray);
		}

	private:
		void _release()throw()
		{
			if (m_pArray != NULL)
			{
				delete [] m_pArray;
			}

			m_pArray = NULL;
		}
	};

	template <class T>
	void swap( ScopedArray<T> & a, ScopedArray<T> & b)
	{
		a.swap( b);
	}
}
}

#endif