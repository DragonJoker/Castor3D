/*
See LICENSE file in root folder
*/
#ifndef ___CastorUtils_RangeSequence_HPP___
#define ___CastorUtils_RangeSequence_HPP___
#pragma once

#include "CastorUtils/Math/Range.hpp"

namespace castor
{
	template< typename T >
	class IndexedRangeT
		: public Range< T >
	{
	public:
		inline IndexedRangeT( uint32_t index
			, T const & min
			, T const & max )
			: Range< T >{ min, max }
			, m_index{ index }
		{
		}

		inline uint32_t getIndex()const
		{
			return m_index;
		}

	private:
		uint32_t m_index;
	};

	template< typename T >
	class RangeSequenceT
		: public Range< T >
	{
	public:
		using RangeType = IndexedRangeT< T >;
		using SequenceType = std::vector< RangeType >;

	public:
		inline RangeSequenceT( std::vector< T > const & sequence )
			: Range{ sequence.front(), sequence.back() }
			, m_sequence{ convert( sequence ) }
		{
		}

		inline RangeType const & getRange( T const & value )const
		{
			auto it = std::find_if( m_sequence.begin()
				, m_sequence.end()
				, [&value]( RangeType const & range )
				{
					return range.has( value );
				} );

			if ( it != m_sequence.end() )
			{
				return *it;
			}

			return m_sequence.back();
		}

	private:
		inline SequenceType convert( std::vector< T > const & sequence )
		{
			assert( sequence.size() >= 2u );
			SequenceType result;
			uint32_t index = 0u;
			auto it = sequence.begin();
			auto end = sequence.end();
			auto prv = *it;
			++it;

			while ( it != end )
			{
				auto cur = *it;
				result.emplace_back( index, prv, cur );
				prv = cur;
				++it;
				++index;
			}

			return result;
		}

	private:
		SequenceType m_sequence;
	};
}

#endif
