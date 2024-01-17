/*
See LICENSE file in root folder
*/
#ifndef ___CU_ConstPixelIterator_H___
#define ___CU_ConstPixelIterator_H___

#include "CastorUtils/Graphics/PixelIterator.hpp"

#include <cstddef>
#include <iterator>

namespace castor
{
	template< PixelFormat PF >
	struct ConstPixelIterator
	{
		template< PixelFormat PiF >
		friend struct PixelIterator;

		using value_type = Pixel< PF >;
		using difference_type = std::ptrdiff_t;
		using pointer = Pixel< PF > const *;
		using reference = Pixel< PF > const &;

		using pixel_type = Pixel< PF >;
		using array_type = PxBufferBase::PxArray;
		using internal_type = array_type::const_iterator;

		using iterator_category = internal_type::iterator_category;
		
		ConstPixelIterator( array_type const & array, internal_type const & iter )
			: m_current{ iter }
			, m_end{ array.end() }
		{
			doLink();
		}

		ConstPixelIterator( ConstPixelIterator const & iter )
			: m_current{ iter.m_current }
			, m_end{ iter.m_end }
		{
			doLink();
		}
			
		ConstPixelIterator( ConstPixelIterator && iter )noexcept
			: m_current{ std::move( iter.m_current ) }
			, m_end{ std::move( iter.m_end ) }
		{
			doLink();
			iter.m_pixel.unlink();
		}

		ConstPixelIterator & operator=( ConstPixelIterator const & it )
		{
			m_current = it.m_current;
			m_end = it.m_end;
			doLink();
			return *this;
		}

		ConstPixelIterator & operator=( ConstPixelIterator && it )noexcept
		{
			m_current = std::move( it.m_current );
			m_end = std::move( it.m_end );
			doLink();
			it.m_pixel.unlink();
			return *this;
		}

		ConstPixelIterator & operator+=( size_t offset )
		{
			m_current += ptrdiff_t( offset * ConstPixelIterator::size );
			doLink();
			return *this;
		}

		ConstPixelIterator & operator-=( size_t offset )
		{
			m_current -= ptrdiff_t( offset * ConstPixelIterator::size );
			doLink();
			return *this;
		}

		ConstPixelIterator & operator++()
		{
			operator+=( 1u );
			return *this;
		}

		ConstPixelIterator operator++( int )
		{
			ConstPixelIterator temp = *this;
			++( *this );
			return temp;
		}

		ConstPixelIterator & operator--()
		{
			operator-=( 1u );
			return *this;
		}

		ConstPixelIterator operator--( int )
		{
			ConstPixelIterator temp = *this;
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

		bool operator==( ConstPixelIterator const & it )const
		{
			return m_current == it.m_current;
		}

		bool operator!=( ConstPixelIterator const & it )const
		{
			return !( *this == it );
		}

		static difference_type diffIt( ConstPixelIterator const & lhs
			, ConstPixelIterator const & rhs )
		{
			assert( lhs.m_end == rhs.m_end );
			return difference_type( ( lhs.m_current - rhs.m_current ) / size );
		}

		static difference_type diffIt( ConstPixelIterator const & lhs
			, PixelIterator< PF > const & rhs )
		{
			assert( lhs.m_end == rhs.m_end );
			return difference_type( ( lhs.m_current - rhs.m_current ) / size );
		}

		static difference_type diffIt( PixelIterator< PF > const & lhs
			, ConstPixelIterator const & rhs )
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
		internal_type m_end;
		pixel_type m_pixel;
	};

	template< PixelFormat PF >
	inline ConstPixelIterator< PF > operator+( ConstPixelIterator< PF > const & it
		, size_t offset )
	{
		ConstPixelIterator< PF > result{ it };
		result += offset;
		return result;
	}

	template< PixelFormat PF >
	inline ConstPixelIterator< PF > operator-( ConstPixelIterator< PF > const & it
		, size_t offset )
	{
		ConstPixelIterator< PF > result{ it };
		result -= offset;
		return result;
	}

	template< PixelFormat PF >
	inline typename ConstPixelIterator< PF >::difference_type operator-( ConstPixelIterator< PF > const & lhs
		, ConstPixelIterator< PF > const & rhs )
	{
		return ConstPixelIterator< PF >::diffIt( lhs, rhs );
	}

	template< PixelFormat PF >
	inline typename ConstPixelIterator< PF >::difference_type operator-( PixelIterator< PF > const & lhs
		, ConstPixelIterator< PF > const & rhs )
	{
		return ConstPixelIterator< PF >::diffIt( lhs, rhs );
	}

	template< PixelFormat PF >
	inline typename ConstPixelIterator< PF >::difference_type operator-( ConstPixelIterator< PF > const & lhs
		, PixelIterator< PF > const & rhs )
	{
		return ConstPixelIterator< PF >::diffIt( lhs, rhs );
	}
}

#endif
