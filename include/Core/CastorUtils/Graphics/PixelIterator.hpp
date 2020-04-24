/*
See LICENSE file in root folder
*/
#ifndef ___CU_PixelIterator_H___
#define ___CU_PixelIterator_H___

#include "CastorUtils/Graphics/Pixel.hpp"

#include <cstddef>
#include <iterator>

namespace castor
{
	template< PixelFormat PF >
	struct PixelIterator
	{
		template< PixelFormat PF >
		friend struct ConstPixelIterator;

		using iterator_category = typename std::random_access_iterator_tag;
		using value_type = Pixel< PF >;
		using difference_type = std::ptrdiff_t;
		using pointer = Pixel< PF > *;
		using reference = Pixel< PF > &;

		using pixel_type = Pixel< PF >;
		using array_type = PxBufferBase::px_array;
		using internal_type = array_type::iterator;
		using const_internal_type = array_type::const_iterator;

		inline PixelIterator( array_type const & array, internal_type const & iter )
			: m_current{ iter }
			, m_end{ array.end() }
		{
			doLink();
		}

		inline PixelIterator( PixelIterator const & iter )
			: m_current{ iter.m_current }
			, m_end{ iter.m_end }
		{
			doLink();
		}
			
		inline PixelIterator( PixelIterator && iter )
			: m_current{ std::move( iter.m_current ) }
			, m_end{ std::move( iter.m_end ) }
		{
			doLink();
			iter.m_pixel.unlink();
		}

		inline PixelIterator & operator=( PixelIterator const & it )
		{
			m_current = it.m_current;
			m_end = it.m_end;
			doLink();
			return *this;
		}

		inline PixelIterator & operator=( PixelIterator && it )
		{
			m_current = std::move( it.m_current );
			m_end = std::move( it.m_end );
			doLink();
			it.m_pixel.unlink();
			return *this;
		}

		inline PixelIterator & operator+=( size_t offset )
		{
			m_current += offset * PixelIterator::size;
			doLink();
			return *this;
		}

		inline PixelIterator & operator-=( size_t offset )
		{
			m_current -= offset * PixelIterator::size;
			doLink();
			return *this;
		}

		inline PixelIterator & operator++()
		{
			operator+=( 1u );
			return *this;
		}

		inline PixelIterator operator++( int )
		{
			PixelIterator temp = *this;
			++( *this );
			return temp;
		}

		inline PixelIterator & operator--()
		{
			operator-=( 1u );
			return *this;
		}

		inline PixelIterator operator--( int )
		{
			PixelIterator temp = *this;
			++( *this );
			return temp;
		}

		inline pixel_type const & operator*()const
		{
			CU_Require( m_current != m_end );
			return m_pixel;
		}

		inline pixel_type & operator*()
		{
			CU_Require( m_current != m_end );
			return m_pixel;
		}

		inline pixel_type * operator->()
		{
			CU_Require( m_current != m_end );
			return &m_pixel;
		}

		inline bool operator==( PixelIterator const & it )const
		{
			return m_current == it.m_current;
		}

		inline bool operator!=( PixelIterator const & it )const
		{
			return !( *this == it );
		}

		static inline difference_type diffIt( PixelIterator const & lhs
			, PixelIterator const & rhs )
		{
			assert( lhs.m_end == rhs.m_end );
			return difference_type( ( lhs.m_current - rhs.m_current ) / size );
		}

	private:
		void doLink()
		{
			if ( m_current != m_end )
			{
				m_pixel.link( &( *m_current ) );
			}
		}

	private:
		static uint8_t const size = PixelDefinitions< PF >::Size;
		internal_type m_current;
		const_internal_type m_end;
		pixel_type m_pixel;
	};

	template< PixelFormat PF >
	inline PixelIterator< PF > operator+( PixelIterator< PF > it
		, size_t offset )
	{
		PixelIterator< PF > result{ it };
		result += offset;
		return result;
	}

	template< PixelFormat PF >
	inline PixelIterator< PF > operator-( PixelIterator< PF > it
		, size_t offset )
	{
		PixelIterator< PF > result{ it };
		result -= offset;
		return result;
	}

	template< PixelFormat PF >
	inline typename PixelIterator< PF >::difference_type operator-( PixelIterator< PF > const & lhs
		, PixelIterator< PF > const & rhs )
	{
		return PixelIterator< PF >::diffIt( lhs, rhs );
	}
}

#endif
