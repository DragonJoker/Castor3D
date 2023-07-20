/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_ARRAY_VIEW_H___
#define ___CASTOR_ARRAY_VIEW_H___

#include "CastorUtils/Design/DesignModule.hpp"

#include <iterator>

namespace castor
{
	template< typename ValueT, typename IteratorTraitsT >
	class ArrayView
	{
		using my_traits = IteratorTraitsT;

	public:
		using value_type = typename my_traits::value_type;
		using reference = typename my_traits::reference;
		using pointer = value_type *;
		using const_pointer = value_type const *;
		using iterator = typename my_traits::pointer;
		using const_iterator = const iterator;
		using reverse_iterator = std::reverse_iterator< iterator >;
		using const_reverse_iterator = std::reverse_iterator< const_iterator >;

	public:
		ArrayView()noexcept
			: m_begin{ nullptr }
			, m_end{ nullptr }
		{
		}

		ArrayView( iterator begin, iterator end )noexcept
			: m_begin( begin )
			, m_end( end )
		{
		}

		reference operator[]( size_t index )noexcept
		{
			CU_Require( index < size() );
			return *( m_begin + index );
		}

		const reference operator[]( size_t index )const noexcept
		{
			CU_Require( index < size() );
			return *( m_begin + index );
		}

		bool empty()const noexcept
		{
			return m_end == m_begin;
		}

		size_t size()const noexcept
		{
			return size_t( m_end - m_begin );
		}

		pointer data()noexcept
		{
			return &( *m_begin );
		}

		const_pointer data()const noexcept
		{
			return &( *m_begin );
		}

		reference front()noexcept
		{
			CU_Require( !empty() );
			return *m_begin;
		}

		const reference front()const noexcept
		{
			CU_Require( !empty() );
			return *m_begin;
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
		iterator m_begin;
		iterator m_end;
	};

	template< typename IterT >
	bool operator==( ArrayView< IterT > const & lhs
		, ArrayView< IterT > const & rhs )
	{
		auto result = lhs.size() == rhs.size();
		auto itLhs = lhs.begin();
		auto itRhs = rhs.begin();

		while ( result && itLhs != lhs.end() )
		{
			result = ( *itLhs == *itRhs );
			++itLhs;
			++itRhs;
		}

		return result;
	}

	template< typename IterT >
	bool operator!=( ArrayView< IterT > const & lhs
		, ArrayView< IterT > const & rhs )
	{
		auto result = lhs.size() != rhs.size();
		auto itLhs = lhs.begin();
		auto itRhs = rhs.begin();

		while ( !result && itLhs != lhs.end() )
		{
			result = ( *itLhs != *itRhs );
			++itLhs;
			++itRhs;
		}

		return result;
	}

	template< typename IterT, typename ValueT = typename IteratorTraits< IterT >::value_type >
	ArrayView< ValueT > makeArrayView( IterT begin, IterT end )
	{
		return ( ( begin != end )
			? ArrayView< ValueT >{ &( *begin )
				, &( *begin ) + std::distance( begin, end ) }
			: ArrayView< ValueT >{} );
	}

	template< typename ValueT >
	ArrayView< ValueT > makeArrayView( ValueT * begin, ValueT * end )
	{
		return ArrayView< ValueT >{ begin, end };
	}

	template< typename IterT >
	auto makeArrayView( IterT begin, uint32_t size )
	{
		return makeArrayView( begin, begin + size );
	}

	template< typename IterT >
	auto makeArrayView( IterT begin, uint64_t size )
	{
		return makeArrayView( begin, begin + size );
	}

	template< typename ValueT, size_t N >
	auto makeArrayView( ValueT( &buffer )[N] )
	{
		return makeArrayView( buffer, buffer + N );
	}

	CU_DeclareArrayView( byte, Byte );
	CU_DeclareArrayView( s32, Int32 );
	CU_DeclareArrayView( u32, UInt32 );
	CU_DeclareArrayView( byte const, ConstByte );
	CU_DeclareArrayView( s32 const, ConstInt32 );
	CU_DeclareArrayView( u32 const, ConstUInt32 );
}

#endif
