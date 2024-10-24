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
		template< PixelFormat PiF >
		friend struct ConstPixelIterator;

		using value_type = Pixel< PF >;
		using difference_type = std::ptrdiff_t;
		using pointer = Pixel< PF > *;
		using reference = Pixel< PF > &;

		using pixel_type = Pixel< PF >;
		using array_type = PxBufferBase::PxArray;
		using internal_type = array_type::iterator;
		using const_internal_type = array_type::const_iterator;

		using iterator_category = internal_type::iterator_category;

		~PixelIterator()noexcept = default;

		PixelIterator( array_type const & array, internal_type const & iter )
			: m_current{ iter }
			, m_end{ array.end() }
		{
			doLink();
		}

		PixelIterator( PixelIterator const & iter )
			: m_current{ iter.m_current }
			, m_end{ iter.m_end }
		{
			doLink();
		}
			
		PixelIterator( PixelIterator && iter )noexcept
			: m_current{ castor::move( iter.m_current ) }
			, m_end{ castor::move( iter.m_end ) }
		{
			doLink();
			iter.m_pixel.unlink();
		}

		PixelIterator & operator=( PixelIterator const & it )
		{
			m_current = it.m_current;
			m_end = it.m_end;
			doLink();
			return *this;
		}

		PixelIterator & operator=( PixelIterator && it )noexcept
		{
			m_current = castor::move( it.m_current );
			m_end = castor::move( it.m_end );
			doLink();
			it.m_pixel.unlink();
			return *this;
		}

		PixelIterator & operator+=( size_t offset )
		{
			m_current += ptrdiff_t( offset * PixelIterator::size );
			doLink();
			return *this;
		}

		PixelIterator & operator-=( size_t offset )
		{
			m_current -= ptrdiff_t( offset * PixelIterator::size );
			doLink();
			return *this;
		}

		PixelIterator & operator++()
		{
			operator+=( 1u );
			return *this;
		}

		PixelIterator operator++( int )
		{
			PixelIterator temp = *this;
			++( *this );
			return temp;
		}

		PixelIterator & operator--()
		{
			operator-=( 1u );
			return *this;
		}

		PixelIterator operator--( int )
		{
			PixelIterator temp = *this;
			++( *this );
			return temp;
		}

		pixel_type const & operator*()const
		{
			CU_Require( m_current != m_end );
			return m_pixel;
		}

		pixel_type & operator*()
		{
			CU_Require( m_current != m_end );
			return m_pixel;
		}

		pixel_type * operator->()
		{
			CU_Require( m_current != m_end );
			return &m_pixel;
		}

		bool operator==( PixelIterator const & it )const
		{
			return m_current == it.m_current;
		}

		bool operator!=( PixelIterator const & it )const
		{
			return !( *this == it );
		}

		static difference_type diffIt( PixelIterator const & lhs
			, PixelIterator const & rhs )
		{
			assert( lhs.m_end == rhs.m_end );
			return difference_type( ( lhs.m_current - rhs.m_current ) / size );
		}

	private:
		void doLink()noexcept
		{
			if ( m_current != m_end )
			{
				m_pixel.link( &( *m_current ) );
			}
		}

	private:
		static uint8_t const size = PixelDefinitionsT< PF >::Size;
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
