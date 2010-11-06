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
#ifndef ___Castor_SharedArray___
#define ___Castor_SharedArray___

#include "SmartPtrCommon.h"

namespace Castor
{	namespace Templates
{
	template <typename T> class SharedArray;

	/*!
	 WeakArray template class stores a pointer from a SharedArray.
	 Holds the pointer and can tell about it's existence but does'nt own it.
	 It can also lock it from destruction by creating a SharedPtr from it.
	*/
	template <typename T>
	class WeakArray
	{
	private:
		template <class Y> friend class WeakArray;
		template <class Y> friend class SharedArray;

	private:
		SharedCount m_count;
		T * m_pArray;
		DeleterBase * m_pDelete;

	public:
		/**
		 * Constructors
		 */
		WeakArray()throw()
			:	m_pPointer( NULL),
				m_count(),
				m_pDelete( NULL)
		{
		}
		WeakArray( const WeakArray & p_weakArray)throw()
			:	m_count( p_weakArray.m_count),
				m_pArray( p_weakArray.m_pArray),
				m_pDelete( p_weakArray.m_pDelete)
		{
		}
		template <class Y>
		WeakArray( const WeakArray<Y> & p_weakArray)throw()		// p_weakArray.get() must be convertible to T *
			:	m_count( p_weakArray.m_count),
				m_pArray( dynamic_cast <T *>( p_weakArray.m_pArray)),
				m_pDelete( p_weakArray.m_pDelete)
		{
		}
		WeakArray( const SharedArray<T> & p_sharedArray)throw()
			:	m_count( p_sharedArray.m_count),
				m_pArray( p_sharedArray.m_pArray),
				m_pDelete( p_sharedArray.m_pDelete)
		{
		}
		template <class Y>
		WeakArray( const SharedArray<Y> & p_sharedArray)throw()	// p_sharedArray.get() must be convertible to T *
			:	m_count( p_sharedArray.m_count),
				m_pArray( dynamic_cast <T *>( p_sharedArray.m_pArray)),
				m_pDelete( p_sharedArray.m_pDelete)
		{
		}
		/**
		 * Destructor
		 */
		virtual ~WeakArray()throw()
		{
			m_count.reset();
			m_pArray = NULL;
		}

		/**
		 * Assignment operators
		 */
		WeakArray & operator =( const WeakArray & p_weakPtr)throw()
		{
			WeakArray( p_weakPtr).swap( * this);
		}
		template <class Y>
		WeakArray & operator =( WeakArray <Y> const & p_weakPtr)throw()
		{
			WeakArray( p_weakPtr).swap( * this);
		}
		template <class Y>
		WeakArray & operator =( SharedArray <Y> const & p_sharedArray)throw()
		{
			WeakArray( p_sharedArray).swap( * this);
		}

		/**
		 * Locking function, used to prevent the destruction of the pointer by creating a SharedPtr wrapping it
		 */
		SharedArray<T> lock()const throw()
		{
			SharedArray<T> l_return( * this);
			return l_return;
		}
		/**
		 * 
		 */
		bool expired()const throw()
		{
			return m_count.count() == 0;
		}

		/**
		 * reset function to reinitialise this
		 */
		void reset()throw()
		{
			WeakArray().swap( * this);
		}

		/**
		 * swap function
		 */
		void swap( WeakArray & p_weakArray)throw()
		{
			WeakArray<T> l_ptr( * this);
			this->operator =( p_weakArray);
			p_weakArray = l_ptr;
		}
	};

	/*!
	 SharedArray template class stores a pointer to a dynamically allocated array
	 Holds the pointer and can share it thanks to copy constructor and assignment operator
	 (and so can be put in std containers)
	 */
	template <typename T>
	class SharedArray
	{
	private:
		template <class Y> friend class WeakArray;
		template <class Y> friend class SharedArray;

	private:
		SharedCount m_count;
		T * m_pArray;
		DeleterBase * m_pDelete;

	public:
		/**
		 * Constructors
		 */
		SharedArray()throw()
			:	m_pArray( NULL),
				m_count(),
				m_pDelete( NULL)
		{
		}
		SharedArray( T * p_pArray)throw()
			:	m_pArray( p_pArray),
				m_count()
		{
			m_pDelete = new DeleterArray<T>( m_pArray);
			_ref();
		}
		template <class Y>
		explicit SharedArray( Y * p_pArray)throw()	// p_pPointer must be convertible to T *
			:	m_pArray( dynamic_cast <T *>( p_pArray)),
				m_count()
		{
			m_pDelete = new DeleterArray<T>( m_pPointer);
			_ref();
		}
		SharedArray( const SharedArray & p_sharedArray)throw()
			:	m_pArray( p_sharedArray.m_pArray),
				m_count( p_sharedArray.m_count),
				m_pDelete( p_sharedArray.m_pDelete)
		{
			_ref();
		}
		SharedArray( const WeakArray<T> & p_weakArray)throw()
			:	m_pArray( p_weakArray.m_pArray),
				m_count( p_weakArray.m_count),
				m_pDelete( p_weakArray.m_pDelete)
		{
			_ref();
		}
		template <class Y>
		SharedArray( const WeakArray<Y> & p_weakArray)throw()	// p_weakPtr.get() must be convertible to T *
			:	m_pArray( dynamic_cast <T *>( p_weakArray.m_pArray)),
				m_count( p_weakArray.m_count),
				m_pDelete( p_weakArray.m_pDelete)
		{
			_ref();
		}
		template <class Y>
		SharedArray( const SharedArray<Y> & p_sharedArray)throw()	// p_sharedPtr.get() must be convertible to T *
			:	m_pArray( dynamic_cast <T *>( p_sharedArray.m_pArray)),
				m_count( p_sharedArray.m_count),
				m_pDelete( p_sharedArray.m_pDelete)
		{
			_ref();
		}
		template <class Y>
		SharedArray( const SharedArray<Y> & p_sharedArray, static_cast_tag)throw()
			:	m_pArray( static_cast <T *>( p_sharedArray.m_pArray)),
				m_count( p_sharedArray.m_count),
				m_pDelete( p_sharedArray.m_pDelete)
		{
			_ref();
		}
		template <class Y>
		SharedArray( const SharedArray<Y> & p_sharedArray, const_cast_tag)throw()
			:	m_pArray( const_cast <T *>( p_sharedArray.m_pArray)),
				m_count( p_sharedArray.m_count),
				m_pDelete( p_sharedArray.m_pDelete)
		{
			_ref();
		}
		template <class Y>
		SharedArray( const SharedArray<Y> & p_sharedArray, dynamic_cast_tag)throw()
			:	m_pArray( dynamic_cast <T *>( p_sharedArray.m_pArray)),
				m_count(),
		{
			if (m_pArray != NULL)
			{
				m_pDelete = p_sharedArray.m_pDelete;
				m_count = p_sharedArray.m_count;
			}

			_ref();
		}
		template <class Y>
		SharedArray( const SharedArray<Y> & p_sharedArray, polymorphic_cast_tag)
			:	m_pArray( dynamic_cast <T *>( p_sharedArray.m_pArray)),
				m_count()
		{
			if (m_pArray == NULL)
			{
				throw std::bad_cast;
			}
			else
			{
				m_count =  p_sharedArray.m_count;
				m_pDelete = p_sharedArray.m_pDelete;
			}

			_ref();
		}
		template <class Y>
		SharedArray( const SharedArray<Y> & p_sharedArray, T * p_pArray)throw()
			:	m_pArray( p_pArray),
				m_count( p_sharedArray.m_count),
				m_pDelete( p_sharedArray.m_pDelete)
		{
			_ref();
		}
		/**
		 * Destructor
		 */
		virtual ~SharedArray()throw()
		{
			m_count.unref();
			if (null())
			{
				if (m_pArray != NULL)
				{
					m_pDelete->Delete();
					delete m_pDelete;
				}
			}

			m_count.reset();
			m_pArray = NULL;
		}

		/**
		 * Accessors
		 */
		T & operator *()throw()
		{
			return (* m_pArray);
		}
		T & operator []( size_t p_uiIndex)throw()
		{
			return m_pArray[p_uiIndex];
		}
		const T & operator *()const throw()
		{
			return (* m_pArray);
		}
		const T & operator []( size_t p_uiIndex)const throw()
		{
			return m_pArray[p_uiIndex];
		}
		T * get()const throw()
		{
			return m_pArray;
		}
		long count()const throw()
		{
			return m_count.count();
		}
		bool unique()const throw()
		{
			return m_count.count() == 1;
		}
		bool null()const throw()
		{
			return m_count.count() == 0;
		}

		/**
		 * Assignment operators
		 */
		SharedArray & operator =( const SharedArray & p_sharedArray)throw()
		{
			SharedArray( p_sharedArray).swap( * this);
			return * this;
		}
		template <typename Y>
		SharedArray & operator =( const SharedArray <Y> & p_sharedArray)throw()	// p_sharedPtr.m_pPointer must be convertible to T *
		{
			SharedArray( p_sharedArray).swap( * this);
			return * this;
		}

		/**
		 * reset functions, used to reset the value of the current SharedPtr to another value (pointer, SharedPtr, ...)
		 */
		void reset()throw()
		{
			SharedArray().swap( * this);
		}
		template <typename Y>
		void reset( Y * p_pArray)throw()	// p_pPointer must be convertible to T *
		{
			SharedArray( p_pArray).swap( * this);
		}
		template <typename Y>
		void reset( SharedArray<Y> p_sharedArray)throw()	// p_pPointer must be convertible to T *
		{
			SharedArray( p_sharedArray).swap( * this);
		}
		template <typename Y>
		void reset( SharedArray<Y> p_sharedArray, T * p_pArray)throw()	// p_pPointer must be convertible to T *
		{
			SharedArray( p_sharedArray, p_pArray).swap( * this);
		}

		/**
		 * swap function
		 */
		void swap( SharedArray & p_sharedArray)throw()
		{
			m_count.swap( p_sharedArray.m_count);
			std::swap( m_pArray, p_sharedArray.m_pArray);
			std::swap( m_pDelete, p_sharedArray.m_pDelete);
		}

	private:
		void _ref()
		{
			if (m_pArray != NULL)
			{
				m_count.ref();
			}
		}
	};

	template <class T>
	void swap( WeakArray<T> & a, WeakArray<T> & b)throw()
	{
		a.swap( b);
	}

	template <class T>
	void swap( SharedArray<T> & a, SharedArray<T> & b)
	{
		a.swap( b);
	}

	template <class T, class U>
	SharedArray<T> static_pointer_cast( const SharedArray<U> & p_sharedArray)throw()
	{
		return SharedArray<T> l_return( p_sharedArray, static_cast_tag());
	}

	template <class T, class U>
	SharedArray<T> const_pointer_cast( const SharedArray<U> & p_sharedArray)throw()
	{
		return SharedArray<T> l_return( p_sharedArray, const_cast_tag());
	}

	template <class T, class U>
	SharedArray<T> dynamic_pointer_cast( const SharedArray<U> & p_sharedArray)throw()
	{
		return SharedArray<T> l_return( p_sharedArray, dynamic_cast_tag());
	}

	template <class T>
	bool operator ==( const SharedArray<T> & a, const SharedArray<T> & b)throw()
	{
		return a.get() == b.get();
	}

	template <class T>
	bool operator !=( const SharedArray<T> & a, const SharedArray<T> & b)throw()
	{
		return a.get() != b.get();
	}

	template <class T>
	bool operator <( const SharedArray<T> & a, const SharedArray<T> & b)throw()
	{
		return std::less( a.get(), b.get());
	}

	template <class T>
	bool operator ==( const T * a, const SharedArray<T> & b)throw()
	{
		return a == b.get();
	}

	template <class T>
	bool operator !=( const T * a, const SharedArray<T> & b)throw()
	{
		return a != b.get();
	}

	template <class T>
	bool operator <( const T * a, const SharedArray<T> & b)throw()
	{
		return std::less( a, b.get());
	}

	template <class T>
	bool operator ==( const SharedArray<T> & a, const T * b)throw()
	{
		return a.get() == b;
	}

	template <class T>
	bool operator !=( const SharedArray<T> & a, const T * b)throw()
	{
		return a.get() != b;
	}

	template <class T>
	bool operator <( const SharedArray<T> & a, const T * b)throw()
	{
		return std::less( a.get(), b);
	}
}
}

#endif