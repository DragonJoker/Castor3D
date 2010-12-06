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
	class WeakPtr : public std::tr1::weak_ptr<T>
	{
	private:
		typedef std::tr1::weak_ptr<T> my_type;
		template <class Y> friend class WeakPtr;
		template <class Y> friend class SharedPtr;

	public:
		/**
		 * Constructors
		 */
		WeakPtr()throw()
			:	my_type()
		{
		}
		WeakPtr( const WeakPtr & p_weakPtr)throw()
			:	my_type( p_weakPtr)
		{
		}
		template <class Y>
		WeakPtr( const WeakPtr<Y> & p_weakPtr)throw()		// p_weakPtr.get() must be convertible to T *
			:	my_type( p_weakPtr)
		{
		}
		WeakPtr( const SharedPtr<T> & p_sharedPtr)throw()
			:	my_type( p_sharedPtr)
		{
		}
		template <class Y>
		WeakPtr( const SharedPtr<Y> & p_sharedPtr)throw()	// p_sharedPtr.get() must be convertible to T *
			:	my_type( p_sharedPtr)
		{
		}
		/**
		 * Destructor
		 */
		virtual ~WeakPtr()throw()
		{
		}

		/**
		 * Assignment operators
		 */
		WeakPtr & operator =( const WeakPtr & p_weakPtr)throw()
		{
			my_type::operator =( p_weakPtr);
			return * this;
		}
		template <class Y>
		WeakPtr & operator =( WeakPtr <Y> const & p_weakPtr)throw()
		{
			my_type::operator =( p_weakPtr);
			return * this;
		}
		template <class Y>
		WeakPtr & operator =( SharedPtr <Y> const & p_sharedPtr)throw()
		{
			my_type::operator =( p_sharedPtr);
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
			return my_type::expired();
		}

		/**
		 * reset function to reinitialise this
		 */
		void reset()throw()
		{
			my_type::reset();
		}

		/**
		 * swap function
		 */
		void swap( WeakPtr & p_weakPtr)throw()
		{
			my_type::swap( p_weakPtr);
		}
	};

	/*!
	 SharedPtr template class stores a pointer to a dynamically allocated object
	 Holds the pointer and can share it thanks to copy constructor and assignment operator
	 (and so can't be put in std containers)
	 */
	template <class T>
	class SharedPtr : public std::tr1::shared_ptr<T>
	{
	public:
		typedef const SharedPtr<T> & ParamType;

	private:
		typedef std::tr1::shared_ptr<T> my_type;
		template <class Y> friend class WeakPtr;
		template <class Y> friend class SharedPtr;

	public:
		/**
		 * Constructors
		 */
		SharedPtr()
			:	my_type()
		{
		}
		template <class Y>
		explicit SharedPtr( Y * p_pPointer)
			:	my_type( p_pPointer)
		{
		}
		SharedPtr( const SharedPtr & p_sharedPtr)
			:	my_type( p_sharedPtr)
		{
		}
		template <class Y>
		SharedPtr( const SharedPtr<Y> & p_sharedPtr)
			:	my_type( p_sharedPtr)
		{
		}
		template <class Y>
		SharedPtr( const WeakPtr<Y> & p_weakPtr)
			:	my_type( p_weakPtr)
		{
		}
		template <class Y>
		SharedPtr( const SharedPtr<Y> & p_sharedPtr, const static_cast_tag & p_tag)
			:	my_type( p_sharedPtr, std::tr1::_Static_tag())
		{
		}
		template <class Y>
		SharedPtr( const SharedPtr<Y> & p_sharedPtr, const const_cast_tag & p_tag)
			:	my_type( p_sharedPtr, std::tr1::_Const_tag())
		{
		}
		template <class Y>
		SharedPtr( const SharedPtr<Y> & p_sharedPtr,const  dynamic_cast_tag & p_tag)
			:	my_type( p_sharedPtr, std::tr1::_Dynamic_tag())
		{
		}
		template <class Y>
		SharedPtr( const SharedPtr<Y> & p_sharedPtr, T * p_pPointer)
			:	my_type( p_sharedPtr, p_pPointer)
		{
		}
		/**
		 * Destructor
		 */
		virtual ~SharedPtr()
		{
		}

		/**
		 * Accessors
		 */
		T * get()const
		{
			return my_type::get();
		}
		T & operator *()
		{
			return my_type::operator *();
		}
		T * operator ->()
		{
			return my_type::operator ->();
		}
		const T & operator *()const
		{
			return my_type::operator *();
		}
		const T * operator ->()const
		{
			return my_type::operator ->();
		}
		long count()const
		{
			return my_type::use_count();
		}
		bool unique()const
		{
			return my_type::unique();
		}
		bool null()const
		{
			return count() == 0;
		}

		/**
		 * Assignment operators
		 */
		SharedPtr & operator =( const SharedPtr & p_sharedPtr)
		{
			my_type::operator =( p_sharedPtr);
			return * this;
		}
		template <typename Y>
		SharedPtr & operator =( const SharedPtr<Y> & p_sharedPtr)	// p_sharedPtr.m_pPointer must be convertible to T *
		{
			my_type::operator =( p_sharedPtr);
			return * this;
		}

		/**
		 * reset functions, used to reset the value of the current SharedPtr to another value (pointer, SharedPtr, ...)
		 */
		void reset()
		{
			my_type::reset();
		}
		template <typename Y>
		void reset( Y * p_pPointer)	// p_pPointer must be convertible to T *
		{
			my_type::reset( p_pPointer);
		}

		/**
		 * swap function
		 */
		void swap( SharedPtr & p_sharedPtr)
		{
			my_type::swap( p_sharedPtr);
		}
	};

	template <class T>
	void swap( WeakPtr<T> & a, WeakPtr<T> & b)
	{
		a.swap( b);
	}

	template <class T>
	void swap( SharedPtr<T> & a, SharedPtr<T> & b)
	{
		a.swap( b);
	}

	template <class T, class U>
	SharedPtr<T> static_pointer_cast( const SharedPtr<U> & p_sharedPtr)
	{
		return SharedPtr<T>( p_sharedPtr, static_cast_tag());
	}

	template <class T, class U>
	SharedPtr<T> const_pointer_cast( const SharedPtr<U> & p_sharedPtr)
	{
		return SharedPtr<T>( p_sharedPtr, const_cast_tag());
	}

	template <class T, class U>
	SharedPtr<T> dynamic_pointer_cast( const SharedPtr<U> & p_sharedPtr)
	{
		return SharedPtr<T>( p_sharedPtr, dynamic_cast_tag());
	}

	template <class T>
	bool operator ==( const SharedPtr<T> & a, const SharedPtr<T> & b)
	{
		return a.get() == b.get();
	}

	template <class T>
	bool operator !=( const SharedPtr<T> & a, const SharedPtr<T> & b)
	{
		return a.get() != b.get();
	}

	template <class T>
	bool operator <( const SharedPtr<T> & a, const SharedPtr<T> & b)
	{
		return a.get() < b.get();
	}

	template <class T>
	bool operator ==( const T * a, const SharedPtr<T> & b)
	{
		return a == b.get();
	}

	template <class T>
	bool operator !=( const T * a, const SharedPtr<T> & b)
	{
		return a != b.get();
	}

	template <class T>
	bool operator <( const T * a, const SharedPtr<T> & b)
	{
		return a < b.get();
	}

	template <class T>
	bool operator ==( const SharedPtr<T> & a, const T * b)
	{
		return a.get() == b;
	}

	template <class T>
	bool operator !=( const SharedPtr<T> & a, const T * b)
	{
		return a.get() != b;
	}

	template <class T>
	bool operator <( const SharedPtr<T> & a, const T * b)
	{
		return a.get() < b;
	}
}
}

#endif