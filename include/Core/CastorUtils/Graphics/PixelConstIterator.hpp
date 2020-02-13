/*
See LICENSE file in root folder
*/
#ifndef ___CU_ConstPixelIterator_H___
#define ___CU_ConstPixelIterator_H___

#include "CastorUtils/Graphics/Pixel.hpp"

#include <cstddef>
#include <iterator>

namespace castor
{
	template< PixelFormat PF >
	struct ConstPixelIterator
		: public std::iterator< std::random_access_iterator_tag
			, Pixel< PF >
			, std::ptrdiff_t
			, Pixel< PF > const *
			, Pixel< PF > const & >
	{
		using pixel_type = Pixel< PF >;
		using array_type = PxBufferBase::px_array;
		using internal_type = array_type::const_iterator;
		
		inline ConstPixelIterator( array_type const & array, internal_type const & iter )
			: m_current{ iter }
			, m_end{ array.end() }
		{
			doLink();
		}

		inline ConstPixelIterator( ConstPixelIterator const & iter )
			: m_current{ iter.m_current }
			, m_end{ iter.m_end }
		{
			doLink();
		}
			
		inline ConstPixelIterator( ConstPixelIterator && iter )
			: m_current{ std::move( iter.m_current ) }
			, m_end{ std::move( iter.m_end ) }
		{
			doLink();
			iter.m_pixel.unlink();
		}

		inline ConstPixelIterator & operator=( ConstPixelIterator const & it )
		{
			m_current = it.m_current;
			m_end = it.m_end;
			doLink();
			return *this;
		}

		inline ConstPixelIterator & operator=( ConstPixelIterator && it )
		{
			m_current = std::move( it.m_current );
			m_end = std::move( it.m_end );
			doLink();
			it.m_pixel.unlink();
			return *this;
		}

		inline ConstPixelIterator & operator+=( size_t offset )
		{
			m_current += offset * ConstPixelIterator::size;
			doLink();
			return *this;
		}

		inline ConstPixelIterator & operator-=( size_t offset )
		{
			m_current -= offset * ConstPixelIterator::size;
			doLink();
			return *this;
		}

		inline ConstPixelIterator & operator++()
		{
			operator+=( 1u );
			return *this;
		}

		inline ConstPixelIterator operator++( int )
		{
			ConstPixelIterator temp = *this;
			++( *this );
			return temp;
		}

		inline ConstPixelIterator & operator--()
		{
			operator-=( 1u );
			return *this;
		}

		inline ConstPixelIterator operator--( int )
		{
			ConstPixelIterator temp = *this;
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

		inline bool operator==( ConstPixelIterator const & it )const
		{
			return m_current == it.m_current;
		}

		inline bool operator!=( ConstPixelIterator const & it )const
		{
			return !( *this == it );
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
		internal_type m_end;
		pixel_type m_pixel;
	};

	template< PixelFormat PF >
	inline ConstPixelIterator< PF > operator+( ConstPixelIterator< PF > it, size_t offset )
	{
		ConstPixelIterator< PF > result{ it };
		result += offset;
		return result;
	}

	template< PixelFormat PF >
	inline ConstPixelIterator< PF > operator-( ConstPixelIterator< PF > it, size_t offset )
	{
		ConstPixelIterator< PF > result{ it };
		result -= offset;
		return result;
	}
}

#endif
