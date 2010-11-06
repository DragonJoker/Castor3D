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
#ifndef ___Castor_SharedPtr___
#define ___Castor_SharedPtr___

#include "SmartPtrCommon.h"

namespace Castor
{	namespace Templates
{
	template <class T> class SharedPtr;

	/*!
	 WeakPtr template class stores a pointer from a SharedPtr.
	 Holds the pointer and can tell about it's existence but does'nt own it.
	 It can also lock it from destruction by creating a SharedPtr from it.
	*/
	template <class T>
	class WeakPtr
	{
	private:
		template <class Y> friend class WeakPtr;
		template <class Y> friend class SharedPtr;

	private:
		SharedCount m_count;
		T * m_pPointer;
		DeleterBase * m_pDelete;

	public:
		/**
		 * Constructors
		 */
		WeakPtr()throw()
			:	m_pPointer( NULL),
				m_count(),
				m_pDelete( NULL)
		{
		}
		WeakPtr( const WeakPtr & p_weakPtr)throw()
			:	m_count( p_weakPtr.m_count),
				m_pPointer( p_weakPtr.m_pPointer),
				m_pDelete( p_weakPtr.m_pDelete)
		{
		}
		template <class Y>
		WeakPtr( const WeakPtr<Y> & p_weakPtr)throw()		// p_weakPtr.get() must be convertible to T *
			:	m_count( p_weakPtr.m_count),
				m_pPointer( dynamic_cast <T *>( p_weakPtr.m_pPointer)),
				m_pDelete( p_weakPtr.m_pDelete)
		{
		}
		WeakPtr( const SharedPtr<T> & p_sharedPtr)throw()
			:	m_count( p_sharedPtr.m_count),
				m_pPointer( p_sharedPtr.m_pPointer),
				m_pDelete( p_sharedPtr.m_pDelete)
		{
		}
		template <class Y>
		WeakPtr( const SharedPtr<Y> & p_sharedPtr)throw()	// p_sharedPtr.get() must be convertible to T *
			:	m_count( p_sharedPtr.m_count),
				m_pPointer( dynamic_cast <T *>( p_sharedPtr.m_pPointer)),
				m_pDelete( p_sharedPtr.m_pDelete)
		{
		}
		/**
		 * Destructor
		 */
		virtual ~WeakPtr()throw()
		{
			m_count.reset();
			m_pPointer = NULL;
		}

		/**
		 * Assignment operators
		 */
		WeakPtr & operator =( const WeakPtr & p_weakPtr)throw()
		{
			WeakPtr( p_weakPtr).swap( * this);
			return * this;
		}
		template <class Y>
		WeakPtr & operator =( WeakPtr <Y> const & p_weakPtr)throw()
		{
			WeakPtr( p_weakPtr).swap( * this);
			return * this;
		}
		template <class Y>
		WeakPtr & operator =( SharedPtr <Y> const & p_sharedPtr)throw()
		{
			WeakPtr( p_sharedPtr).swap( * this);
			return * this;
		}

		/**
		 * Locking function, used to prevent the destruction of the pointer by creating a SharedPtr wrapping it
		 */
		SharedPtr<T> lock()const throw()
		{
			SharedPtr<T> l_return( * this);
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
			WeakPtr().swap( * this);
		}

		/**
		 * swap function
		 */
		void swap( WeakPtr & p_weakPtr)throw()
		{
			m_count.swap( p_weakPtr.m_count);
			std::swap( m_pPointer, p_weakPtr.m_pPointer);
			std::swap( m_pDelete, p_weakPtr.m_pDelete);
		}
	};

	/*!
	 SharedPtr template class stores a pointer to a dynamically allocated object
	 Holds the pointer and can share it thanks to copy constructor and assignment operator
	 (and so can't be put in std containers)
	 */
	template <class T>
	class SharedPtr
	{
	private:
		template <class Y> friend class WeakPtr;
		template <class Y> friend class SharedPtr;

	private:
		SharedCount m_count;
		T * m_pPointer;
		DeleterBase * m_pDelete;

	public:
		/**
		 * Constructors
		 */
		SharedPtr()throw()
			:	m_pPointer( NULL),
				m_count(),
				m_pDelete( NULL)
		{
		}
		SharedPtr( T * p_pPointer)throw()
			:	m_pPointer( p_pPointer),
				m_count()
		{
			m_pDelete = new DeleterPtr<T>( m_pPointer);
			_ref();
		}
		template <class Y>
		explicit SharedPtr( Y * p_pPointer)throw()	// p_pPointer must be convertible to T *
			:	m_pPointer( dynamic_cast <T *>( p_pPointer)),
				m_count()
		{
			m_pDelete = new DeleterPtr<T>( m_pPointer);
			_ref();
		}
		SharedPtr( const SharedPtr & p_sharedPtr)throw()
			:	m_pPointer( p_sharedPtr.m_pPointer),
				m_count( p_sharedPtr.m_count),
				m_pDelete( p_sharedPtr.m_pDelete)
		{
			_ref();
		}
		SharedPtr( const WeakPtr<T> & p_weakPtr)throw()
			:	m_pPointer( p_weakPtr.m_pPointer),
				m_count( p_weakPtr.m_count),
				m_pDelete( p_weakPtr.m_pDelete)
		{
			_ref();
		}
		template <class Y>
		SharedPtr( const WeakPtr<Y> & p_weakPtr)throw()	// p_weakPtr.get() must be convertible to T *
			:	m_pPointer( dynamic_cast <T *>( p_weakPtr.m_pPointer)),
				m_count( p_weakPtr.m_count),
				m_pDelete( p_sharedPtr.m_pDelete)
		{
			_ref();
		}
		template <class Y>
		SharedPtr( const SharedPtr<Y> & p_sharedPtr)throw()	// p_sharedPtr.get() must be convertible to T *
			:	m_pPointer( dynamic_cast <T *>( p_sharedPtr.m_pPointer)),
				m_count( p_sharedPtr.m_count),
				m_pDelete( p_sharedPtr.m_pDelete)
		{
			_ref();
		}
		template <class Y>
		SharedPtr( const SharedPtr<Y> & p_sharedPtr, static_cast_tag)throw()
			:	m_pPointer( static_cast <T *>( p_sharedPtr.m_pPointer)),
				m_count( p_sharedPtr.m_count),
				m_pDelete( p_sharedPtr.m_pDelete)
		{
			_ref();
		}
		template <class Y>
		SharedPtr( const SharedPtr<Y> & p_sharedPtr, const_cast_tag)throw()
			:	m_pPointer( const_cast <T *>( p_sharedPtr.m_pPointer)),
				m_count( p_sharedPtr.m_count),
				m_pDelete( p_sharedPtr.m_pDelete)
		{
			_ref();
		}
		template <class Y>
		SharedPtr( const SharedPtr<Y> & p_sharedPtr, dynamic_cast_tag)throw()
			:	m_pPointer( dynamic_cast <T *>( p_sharedPtr.m_pPointer)),
				m_count(),
		{
			if (m_pPointer != NULL)
			{
				m_pDelete = p_sharedPtr.m_pDelete;
				m_count = p_sharedPtr.m_count;
			}

			_ref();
		}
		template <class Y>
		SharedPtr( const SharedPtr<Y> & p_sharedPtr, polymorphic_cast_tag)
			:	m_pPointer( dynamic_cast <T *>( p_sharedPtr.m_pPointer)),
				m_count()
		{
			if (m_pPointer == NULL)
			{
				throw std::bad_cast;
			}
			else
			{
				m_count =  p_sharedPtr.m_count;
				m_pDelete = p_sharedPtr.m_pDelete;
			}

			_ref();
		}
		template <class Y>
		SharedPtr( const SharedPtr<Y> & p_sharedPtr, T * p_pPointer)throw()
			:	m_pPointer( p_pPointer),
				m_count( p_sharedPtr.m_count),
				m_pDelete( p_sharedPtr.m_pDelete)
		{
			_ref();
		}
		/**
		 * Destructor
		 */
		virtual ~SharedPtr()throw()
		{
			m_count.unref();
			if (null())
			{
				if (m_pPointer != NULL)
				{
					m_pDelete->Delete();
					delete m_pDelete;
				}
			}
			m_count.reset();
			m_pPointer = NULL;
		}

		/**
		 * Accessors
		 */
		T & operator *()throw()
		{
			return (* m_pPointer);
		}
		T * operator ->()throw()
		{
			return m_pPointer;
		}
		const T & operator *()const throw()
		{
			return (* m_pPointer);
		}
		const T * operator ->()const throw()
		{
			return m_pPointer;
		}
		T * get()const throw()
		{
			return m_pPointer;
		}
		bool operator ()()
		{
			return m_count.count() != 0;
		}
		bool operator ! ()
		{
			return m_count.count() == 0;
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
		SharedPtr & operator =( const SharedPtr & p_sharedPtr)throw()
		{
			SharedPtr( p_sharedPtr).swap( * this);
			return * this;
		}
		template <typename Y>
		SharedPtr & operator =( const SharedPtr <Y> & p_sharedPtr)throw()	// p_sharedPtr.m_pPointer must be convertible to T *
		{
			SharedPtr( p_sharedPtr).swap( * this);
			return * this;
		}

		/**
		 * reset functions, used to reset the value of the current SharedPtr to another value (pointer, SharedPtr, ...)
		 */
		void reset()throw()
		{
			SharedPtr().swap( * this);
		}
		template <typename Y>
		void reset( Y * p_pPointer)throw()	// p_pPointer must be convertible to T *
		{
			SharedPtr( p_pPointer).swap( * this);
		}
		template <typename Y>
		void reset( SharedPtr<Y> p_sharedPtr)throw()	// p_pPointer must be convertible to T *
		{
			SharedPtr( p_sharedPtr).swap( * this);
		}
		template <typename Y>
		void reset( SharedPtr<Y> p_sharedPtr, T * p_pPointer)throw()	// p_pPointer must be convertible to T *
		{
			SharedPtr( p_sharedPtr, p_pPointer).swap( * this);
		}

		/**
		 * swap function
		 */
		void swap( SharedPtr & p_sharedPtr)throw()
		{
			m_count.swap( p_sharedPtr.m_count);
			std::swap( m_pPointer, p_sharedPtr.m_pPointer);
			std::swap( m_pDelete, p_sharedPtr.m_pDelete);
		}

	private:
		void _ref()
		{
			if (m_pPointer != NULL)
			{
				m_count.ref();
			}
		}
	};

	template <class T>
	void swap( WeakPtr<T> & a, WeakPtr<T> & b)throw()
	{
		a.swap( b);
	}

	template <class T>
	void swap( SharedPtr<T> & a, SharedPtr<T> & b)throw()
	{
		a.swap( b);
	}

	template <class T, class U>
	SharedPtr<T> static_pointer_cast( const SharedPtr<U> & p_sharedPtr)throw()
	{
		return SharedPtr<T> l_return( p_sharedPtr, static_cast_tag());
	}

	template <class T, class U>
	SharedPtr<T> const_pointer_cast( const SharedPtr<U> & p_sharedPtr)throw()
	{
		return SharedPtr<T> l_return( p_sharedPtr, const_cast_tag());
	}

	template <class T, class U>
	SharedPtr<T> dynamic_pointer_cast( const SharedPtr<U> & p_sharedPtr)throw()
	{
		return SharedPtr<T> l_return( p_sharedPtr, dynamic_cast_tag());
	}

	template <class T>
	bool operator ==( const SharedPtr<T> & a, const SharedPtr<T> & b)throw()
	{
		return a.get() == b.get();
	}

	template <class T>
	bool operator !=( const SharedPtr<T> & a, const SharedPtr<T> & b)throw()
	{
		return a.get() != b.get();
	}

	template <class T>
	bool operator <( const SharedPtr<T> & a, const SharedPtr<T> & b)throw()
	{
		return a.get() < b.get();
	}

	template <class T>
	bool operator ==( const T * a, const SharedPtr<T> & b)throw()
	{
		return a == b.get();
	}

	template <class T>
	bool operator !=( const T * a, const SharedPtr<T> & b)throw()
	{
		return a != b.get();
	}

	template <class T>
	bool operator <( const T * a, const SharedPtr<T> & b)throw()
	{
		return a < b.get();
	}

	template <class T>
	bool operator ==( const SharedPtr<T> & a, const T * b)throw()
	{
		return a.get() == b;
	}

	template <class T>
	bool operator !=( const SharedPtr<T> & a, const T * b)throw()
	{
		return a.get() != b;
	}

	template <class T>
	bool operator <( const SharedPtr<T> & a, const T * b)throw()
	{
		return a.get() < b;
	}
}
}

#endif