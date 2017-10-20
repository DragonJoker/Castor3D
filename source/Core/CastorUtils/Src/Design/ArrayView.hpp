/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_ARRAY_VIEW_H___
#define ___CASTOR_ARRAY_VIEW_H___

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
		ArrayView( pointer p_begin, pointer p_end )noexcept
			: m_begin( p_begin )
			, m_end( p_end )
		{
		}

		template< size_t N >
		ArrayView( T( & p_buffer )[N] )noexcept
			: ArrayView( p_buffer, p_buffer + N )
		{
		}

		ArrayView( T * p_buffer, size_t p_count )noexcept
			: ArrayView( p_buffer, p_buffer + p_count )
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

		pointer data()noexcept
		{
			return m_begin;
		}

		pointer const data()const noexcept
		{
			return m_begin;
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

	template< typename T >
	ArrayView< T > makeArrayView( T * p_begin, size_t p_size )
	{
		return ArrayView< T >( p_begin, p_begin + p_size );
	}

	template< typename T >
	ArrayView< T > makeArrayView( T * p_begin, T * p_end )
	{
		return ArrayView< T >( p_begin, p_end );
	}

	template< typename T, size_t N >
	ArrayView< T > makeArrayView( T ( & p_buffer )[N] )
	{
		return ArrayView< T >( p_buffer );
	}
}

#endif
