/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

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
#ifndef ___CASTOR_ARRAY_VIEW_H___
#define ___CASTOR_ARRAY_VIEW_H___

#include "CastorUtilsPrerequisites.hpp"

#include <vector>

namespace Castor
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Templated class that provide std::array style buffer view.
	\~french
	\brief		Classe template qui fournit une vue sur un tampon, � la mani�re d'un std::array.
	*/
	template< typename T >
	class ArrayView
	{
	public:
		using pointer = T *;
		using reference = T &;
		using const_reference = T const &;
		using iterator = T *;
		using const_iterator = T const *;
		using reverse_iterator = std::reverse_iterator< iterator >;
		using const_reverse_iterator = std::reverse_iterator< const_iterator >;

	public:
		ArrayView( T * p_begin, T * p_end )noexcept
			: m_begin( p_begin )
			, m_end( p_end )
		{
		}

		template< size_t N >
		ArrayView( T ( & p_buffer )[N] )noexcept
			: ArrayView ( p_buffer, p_buffer + N )
		{
		}

		ArrayView( T * p_buffer, size_t p_count )noexcept
			: ArrayView ( p_buffer, p_buffer + p_count )
		{
		}

		reference operator[]( size_t p_index )noexcept
		{
			return m_begin[p_index];
		}

		const_reference operator[]( size_t p_index )const noexcept
		{
			return m_begin[p_index];
		}

		bool empty()const noexcept
		{
			return m_end == m_begin;
		}

		size_t size()const noexcept
		{
			return m_end - m_begin;
		}

		iterator begin()noexcept
		{
			return m_begin;
		}

		reverse_iterator rbegin()noexcept
		{
			return reverse_iterator{ end() };
		}

		const_iterator begin()const noexcept
		{
			return m_begin;
		}

		const_reverse_iterator rbegin()const noexcept
		{
			return reverse_iterator{ end() };
		}

		const_iterator cbegin()const noexcept
		{
			return m_begin;
		}

		const_reverse_iterator crbegin()const noexcept
		{
			return reverse_iterator{ cend() };
		}

		iterator end()noexcept
		{
			return m_end;
		}

		reverse_iterator rend()noexcept
		{
			return reverse_iterator{ begin() };
		}

		const_iterator end()const noexcept
		{
			return m_end;
		}

		const_reverse_iterator rend()const noexcept
		{
			return reverse_iterator{ begin() };
		}

		const_iterator cend()const noexcept
		{
			return m_end;
		}

		const_reverse_iterator crend()const noexcept
		{
			return reverse_iterator{ cbegin() };
		}

	private:
		pointer m_begin;
		pointer m_end;
	};
}

#endif
