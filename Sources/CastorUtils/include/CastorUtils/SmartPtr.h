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
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___CU_SmartPtr___
#define ___CU_SmartPtr___

namespace Castor
{	namespace Templates
{
	template <class T>
	class SmartPtr
	{
	protected:
		T * m_pPointer;
		size_t * m_pUseCount;

	public:
		SmartPtr()
			:	m_pPointer( NULL),
				m_pUseCount( NULL)
		{
		}
		template <class Y>
		explicit SmartPtr( Y * p_pPointer) 
			:	m_pPointer( p_pPointer),
				m_pUseCount( p_pPointer != NULL ? new size_t( 1) : NULL)
		{
		}
		SmartPtr( const SmartPtr & p_ref)
			:	m_pPointer( NULL),
				m_pUseCount( NULL)
		{
			m_pPointer = p_ref.m_pPointer;
			m_pUseCount = p_ref.m_pUseCount;

			if (m_pUseCount)
			{
				++(* m_pUseCount); 
			}
		}
		SmartPtr & operator =( const SmartPtr & p_ref)
		{
			if (m_pPointer == p_ref.m_pPointer)
			{
				return * this;
			}

			SmartPtr<T> l_tmp( p_ref);
			_swap( l_tmp);
			return * this;
		}

		template <class U>
		SmartPtr( const SmartPtr <U> & p_ref)
			:	m_pPointer( NULL),
				m_pUseCount( NULL)
		{
			m_pPointer = p_ref.ptr();
			m_pUseCount = p_ref.count_ptr();

			if (m_pUseCount)
			{
				++( * m_pUseCount);
			}
		}
		template< class U>
		SmartPtr & operator =( const SmartPtr<U> & p_ref)
		{
			if (m_pPointer == p_ref.m_pPointer)
			{
				return * this;
			}

			SmartPtr<T> l_tmp( p_ref);
			_swap( l_tmp);
			return * this;
		}
		virtual ~SmartPtr()
		{
			_release();
		}
		inline T & operator *()const
		{
			assert( m_pPointer != NULL);
			return * m_pPointer;
		}
		inline T * operator ->()const
		{
			assert( m_pPointer != NULL);
			return m_pPointer;
		}
		void Bind( T * p_pPointer)
		{
			assert( m_pPointer == NULL && m_pUseCount == NULL);
			m_pUseCount = new size_t( 1);
			m_pPointer = p_pPointer;
		}
		inline bool Unique()const
		{
			assert( m_pUseCount != NULL);
			return * m_pUseCount == 1;
		}
		inline unsigned int Count()const
		{
			assert( m_pUseCount);
			return * m_pUseCount;
		}
		inline unsigned int * CountPtr()const
		{
			return m_pUseCount;
		}
		inline T * Ptr()const
		{
			return m_pPointer;
		}
		inline bool IsNull()const
		{
			return m_pPointer == 0;
		}
		inline void SetNull(void)
		{ 
			if (m_pPointer != NULL)
			{
				_release();
				m_pPointer = NULL;
				m_pUseCount = NULL;
			}
		}

    protected:
		inline void _release()
		{
			bool l_bDestroyThis = false;

			if (m_pUseCount != NULL)
			{
				if (--( * pUseCount) == 0) 
				{
					l_bDestroyThis = true;
				}
			}

			if (l_bDestroyThis)
			{
				destroy();
			}
		}
		virtual void _destroy()
		{
			delete m_pPointer;
			delete m_pUseCount;
		}
		virtual void _swap( SmartPtr<T> & p_other)
		{
			std::swap( m_pPointer, p_other.m_pPointer);
			std::swap( m_pUseCount, p_other.m_pUseCount);
		}
	};

	template <class T, class U>
	inline bool operator ==( const SmartPtr <T> & a, const SmartPtr <U> & b)
	{
		return a.Ptr() == b.Ptr();
	}

	template <class T, class U>
	inline bool operator !=( const SmartPtr <T> & a, const SmartPtr <U> & b)
	{
		return a.Ptr() != b.Ptr();
	}

	template <class T, class U>
	inline bool operator <( const SmartPtr <T> & a, const SmartPtr <U> & b)
	{
		return std::less <const void *>()( a.Ptr(), b.Ptr());
	}
}

#endif