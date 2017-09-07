/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___CU_GroupChangeTracked_H___
#define ___CU_GroupChangeTracked_H___

#include "CastorUtilsPrerequisites.hpp"

#include <vector>

namespace castor
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Templated class that provide std::array style buffer view.
	\~french
	\brief		Classe template qui fournit une vue sur un tampon, à la manière d'un std::array.
	*/
	template< typename T >
	class GroupChangeTracked
	{
	public:
		GroupChangeTracked( bool & dirty )noexcept
			: m_value{}
			, m_dirty{ dirty }
		{
		}

		GroupChangeTracked( bool & dirty, T const & rhs )noexcept
			: m_value{ rhs }
			, m_dirty{ dirty }
		{
		}

		GroupChangeTracked & operator=( T const & rhs )noexcept
		{
			m_dirty |= m_value != rhs;
			m_value = rhs;
			return *this;
		}

		void reset()noexcept
		{
			m_dirty = false;
		}

		T const & value()const noexcept
		{
			return m_value;
		}

		bool isDirty()const noexcept
		{
			return m_dirty;
		}

		operator T()const noexcept
		{
			return m_value;
		}

		T const * operator->()const noexcept
		{
			return &m_value;
		}

	private:
		T m_value;
		bool & m_dirty;
	};

	template< typename T >
	bool operator==( GroupChangeTracked< T > const & lhs
		, T const & rhs )
	{
		return lhs.value() == rhs;
	}

	template< typename T >
	bool operator==( T const & lhs
		, GroupChangeTracked< T > const & rhs )
	{
		return lhs == rhs.value();
	}

	template< typename T >
	bool operator==( GroupChangeTracked< T > const & lhs
		, GroupChangeTracked< T > const & rhs )
	{
		return lhs.value() == rhs.value();
	}

	template< typename T >
	bool operator!=( GroupChangeTracked< T > const & lhs
		, T const & rhs )
	{
		return !operator==( lhs, rhs );
	}

	template< typename T >
	bool operator!=( T const & lhs
		, GroupChangeTracked< T > const & rhs )
	{
		return !operator==( lhs, rhs );
	}

	template< typename T >
	bool operator!=( GroupChangeTracked< T > const & lhs
		, GroupChangeTracked< T > const & rhs )
	{
		return !operator==( lhs, rhs );
	}
}

#endif
