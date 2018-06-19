#include "DynamicBitset.hpp"

#include <algorithm>
#include <cstring>

#pragma warning( push )
#pragma warning( disable: 4804 )

namespace castor
{
	//*************************************************************************

	namespace details
	{
		template< typename BlockType >
		BlockType makeBitMask( size_t index )
		{
			BlockType result{ 0x01 };
			result <<= index;
			return result;
		}

		template< typename BlockType >
		size_t getBlockIndex( size_t index )
		{
			return index / DynamicBitset< BlockType >::bitsPerBlock;
		}

		template< typename BlockType >
		size_t getBitIndex( size_t index )
		{
			static constexpr size_t bitMask = DynamicBitset< BlockType >::bitsPerBlock - 1u;
			return index & bitMask;
		}

		template< typename BlockType >
		size_t getBlockCount( size_t index )
		{
			return getBlockIndex< BlockType >( index ) + ( getBitIndex< BlockType >( index ) ? 1u : 0u );
		}
	}

	//*************************************************************************

	template< typename BlockType >
	inline DynamicBitset< BlockType >::DynamicBitset()
		: m_bitCount{ 0u }
	{
	}

	template< typename BlockType >
	inline DynamicBitset< BlockType >::DynamicBitset( size_t size, bool value )
	{
		resize( size, value );
	}

	template< typename BlockType >
	inline DynamicBitset< BlockType >::DynamicBitset( String const & bits )
		: DynamicBitset{ bits.data(), bits.size() }
	{
	}

	template< typename BlockType >
	inline DynamicBitset< BlockType >::DynamicBitset( char const * bits )
		: DynamicBitset{ bits, std::strlen( bits ) }
	{
	}

	template< typename BlockType >
	inline DynamicBitset< BlockType >::DynamicBitset( char const * bits
		, size_t size )
		: m_blocks( details::getBlockCount< BlockType >( size ), BlockType{} )
		, m_bitCount{ size }
	{
		for ( auto i = 0u; i < size; ++i )
		{
			REQUIRE( *bits == '0' || *bits == '1' );
			set( m_bitCount - i - 1u, ( *bits++ ) == '1' );
		}
	}

	template< typename BlockType >
	inline void DynamicBitset< BlockType >::set( size_t bit, bool value )
	{
		REQUIRE( bit < m_bitCount );

		BlockType & block = m_blocks[details::getBlockIndex< BlockType >( bit )];
		BlockType mask = BlockType{ 1u } << details::getBitIndex< BlockType >( bit );

		// Setting of the bit without branching
		// https://graphics.stanford.edu/~seander/bithacks.html#ConditionalSetOrClearBitsWithoutBranching
		block = ( block & ~mask ) | ( -value & mask );
	}

	template< typename BlockType >
	inline bool DynamicBitset< BlockType >::get( size_t bit )const
	{
		REQUIRE( bit < m_bitCount );
		return ( m_blocks[details::getBlockIndex< BlockType >( bit )] & details::makeBitMask< BlockType >( details::getBitIndex< BlockType >( bit ) ) )
			? true
			: false;
	}

	template< typename BlockType >
	inline BlockType DynamicBitset< BlockType >::getBlock( size_t index )const
	{
		REQUIRE( index < m_blocks.size() );
		return m_blocks[index];
	}

	template< typename BlockType >
	inline void DynamicBitset< BlockType >::reset()
	{
		static constexpr BlockType zero{};
		std::fill( m_blocks.begin()
			, m_blocks.end()
			, zero );
	}

	template< typename BlockType >
	inline void DynamicBitset< BlockType >::resize( size_t size, bool value )
	{
		size_t lastBlockIndex = m_blocks.size() - 1u;
		m_blocks.resize( details::getBlockCount< BlockType >( size ), ( value ? fullBitMask : BlockType{} ) );
		size_t remainingBits = details::getBitIndex< BlockType >( m_bitCount );

		if ( size > m_bitCount && remainingBits > 0u && value )
		{
			m_blocks[lastBlockIndex] |= fullBitMask << remainingBits;
		}

		m_bitCount = size;
		doResetExtraBits();
	}

	template< typename BlockType >
	inline size_t DynamicBitset< BlockType >::getSize()const
	{
		return m_bitCount;
	}

	template< typename BlockType >
	inline size_t DynamicBitset< BlockType >::getBlockCount()const
	{
		return m_blocks.size();
	}

	template< typename BlockType >
	inline bool DynamicBitset< BlockType >::none()const
	{
		return !any();
	}

	template< typename BlockType >
	inline bool DynamicBitset< BlockType >::any()const
	{
		return m_blocks.end() != std::find_if( m_blocks.begin()
			, m_blocks.end()
			, []( BlockType const lookup )
			{
				return lookup != 0;
			} );
	}

	template< typename BlockType >
	inline bool DynamicBitset< BlockType >::all()const
	{
		bool result = false;

		if ( !m_blocks.empty() )
		{
			auto end = m_blocks.begin() + m_blocks.size() - 1u;
			result = end == std::find_if( m_blocks.begin()
				, end
				, []( BlockType const lookup )
				{
					return lookup != fullBitMask;
				} );
			// last block check
			result &= ( *end ) == ( fullBitMask & doGetLastBlockMask() );
		}

		return result;
	}

	template< typename BlockType >
	inline typename DynamicBitset< BlockType >::Bit DynamicBitset< BlockType >::operator[]( size_t index )
	{
		return Bit{ m_blocks[details::getBlockIndex< BlockType >( index )]
			, details::makeBitMask< BlockType >( details::getBitIndex< BlockType >( index ) ) };
	}

	template< typename BlockType >
	inline bool DynamicBitset< BlockType >::operator[]( size_t index )const
	{
		return get( index );
	}

	template< typename BlockType >
	inline DynamicBitset< BlockType > & DynamicBitset< BlockType >::operator<<=( int value )
	{
		if ( value )
		{
			if ( value >= m_bitCount )
			{
				reset();
			}
			else
			{
				size_t blockShift = value / bitsPerBlock;
				size_t remainder = value % bitsPerBlock;

				if ( remainder )
				{
					size_t lastIndex = m_blocks.size() - 1u;
					size_t remaining = bitsPerBlock - remainder;

					for ( size_t i = blockShift; i < lastIndex; ++i )
					{
						m_blocks[i - blockShift] = ( m_blocks[i] >> remainder ) | ( m_blocks[i + 1u] << remaining );
					}

					m_blocks[lastIndex - blockShift] = m_blocks[lastIndex] >> remainder;
					std::fill_n( m_blocks.begin() + ( m_blocks.size() - blockShift )
						, blockShift
						, BlockType{} );
				}
				else
				{
					for ( auto it = m_blocks.begin(); it != m_blocks.end(); ++it )
					{
						if ( size_t( std::distance( m_blocks.begin(), it ) + blockShift ) < m_blocks.size() )
						{
							auto shiftedIt = it;
							std::advance( shiftedIt, blockShift );
							*it = *shiftedIt;
						}
						else
						{
							*it = 0u;
						}
					}
				}

				doResetExtraBits();
			}
		}

		return *this;
	}

	template< typename BlockType >
	inline DynamicBitset< BlockType > & DynamicBitset< BlockType >::operator>>=( int value )
	{
		if ( value )
		{
			if ( value >= m_bitCount )
			{
				reset();
			}
			else
			{
				size_t blockShift = value / bitsPerBlock;
				size_t remainder = value % bitsPerBlock;

				if ( remainder )
				{
					size_t lastIndex = m_blocks.size() - 1u;
					size_t remaining = bitsPerBlock - remainder;

					for ( auto i = lastIndex - blockShift; i > 0u; --i )
					{
						m_blocks[i + blockShift] = ( m_blocks[i] << remainder ) | ( m_blocks[i - 1] >> remaining );
					}

					m_blocks[blockShift] = m_blocks[0] << remainder;
					std::fill_n( m_blocks.begin()
						, blockShift
						, BlockType{} );
				}
				else
				{
					for ( auto it = m_blocks.rbegin(); it != m_blocks.rend(); ++it )
					{
						if ( size_t( std::distance( m_blocks.rbegin(), it ) + blockShift ) < m_blocks.size() )
						{
							auto shiftedIt = it;
							std::advance( shiftedIt, blockShift );
							*it = *shiftedIt;
						}
						else
						{
							*it = 0u;
						}
					}
				}

				doResetExtraBits();
			}
		}

		return *this;
	}

	template< typename BlockType >
	inline DynamicBitset< BlockType > & DynamicBitset< BlockType >::operator&=( DynamicBitset const & value )
	{
		std::pair< size_t, size_t > minmax = std::minmax( getBlockCount(), value.getBlockCount() );
		m_blocks.resize( minmax.second );
		m_bitCount = std::max( getSize(), value.getSize() );

		for ( size_t i = 0u; i < minmax.first; ++i )
		{
			m_blocks[i] = m_blocks[i] & value.m_blocks[i];
		}

		for ( size_t i = minmax.first; i < minmax.second; ++i )
		{
			m_blocks[i] = 0u;
		}

		doResetExtraBits();
		return *this;
	}

	template< typename BlockType >
	inline DynamicBitset< BlockType > & DynamicBitset< BlockType >::operator|=( DynamicBitset const & value )
	{
		DynamicBitset const & greater = ( getSize() > value.getSize() ) ? *this : value;
		DynamicBitset const & lesser = ( getSize() > value.getSize() ) ? value : *this;

		size_t maxBlockCount = greater.getBlockCount();
		size_t minBlockCount = lesser.getBlockCount();
		m_blocks.resize( maxBlockCount );
		m_bitCount = greater.getSize();

		for ( size_t i = 0; i < minBlockCount; ++i )
		{
			m_blocks[i] = m_blocks[i] | value.m_blocks[i];
		}

		for ( size_t i = minBlockCount; i < maxBlockCount; ++i )
		{
			m_blocks[i] = greater.m_blocks[i];
		}

		doResetExtraBits();
		return *this;
	}

	template< typename BlockType >
	inline DynamicBitset< BlockType > & DynamicBitset< BlockType >::operator^=( DynamicBitset const & value )
	{
		DynamicBitset const & greater = ( getSize() > value.getSize() ) ? *this : value;
		DynamicBitset const & lesser = ( getSize() > value.getSize() ) ? value : *this;

		size_t maxBlockCount = greater.getBlockCount();
		size_t minBlockCount = lesser.getBlockCount();
		m_blocks.resize( maxBlockCount );
		m_bitCount = greater.getSize();

		for ( size_t i = 0; i < minBlockCount; ++i )
		{
			m_blocks[i] = m_blocks[i] ^ value.m_blocks[i];
		}

		for ( size_t i = minBlockCount; i < maxBlockCount; ++i )
		{
			m_blocks[i] = greater.m_blocks[i];
		}

		doResetExtraBits();
		return *this;
	}

	template< typename BlockType >
	inline DynamicBitset< BlockType > DynamicBitset< BlockType >::operator~()const
	{
		DynamicBitset result;

		for ( auto & block : result.m_blocks )
		{
			block = ~block;
		}

		result.doResetExtraBits();
		return result;
	}

	template< typename BlockType >
	inline String DynamicBitset< BlockType >::toString()const
	{
		String result( m_bitCount, '0' );

		for ( auto i = 0u; i < m_bitCount; ++i )
		{
			result[i] = get( i ) ? '1' : '0';
		}

		return result;
	}

	template< typename BlockType >
	void DynamicBitset< BlockType >::doResetExtraBits()
	{
		auto mask = doGetLastBlockMask();

		if ( mask )
		{
			m_blocks.back() &= mask;
		}
	}

	template< typename BlockType >
	BlockType DynamicBitset< BlockType >::doGetLastBlockMask() const
	{
		return ( BlockType{ 1u } << details::getBitIndex< BlockType >( m_bitCount ) ) - 1u;
	}

	//*************************************************************************

	template< typename BlockType >
	inline bool operator==( DynamicBitset< BlockType > const & lhs
		, DynamicBitset< BlockType > const & rhs )
	{
		DynamicBitset< BlockType > const & greater = ( lhs.getSize() > rhs.getSize() ) ? lhs : rhs;
		DynamicBitset< BlockType > const & lesser = ( lhs.getSize() > rhs.getSize() ) ? rhs : lhs;
		size_t maxBlockCount = greater.getBlockCount();
		size_t minBlockCount = lesser.getBlockCount();
		bool result = true;

		for ( size_t i = 0; result && i < minBlockCount; ++i )
		{
			result = ( lhs.getBlock( i ) == rhs.getBlock( i ) );
		}

		for ( size_t i = minBlockCount; result && i < maxBlockCount; ++i )
		{
			result = greater.getBlock( i ) == 0;
		}

		return result;
	}

	template< typename BlockType >
	inline bool operator!=( DynamicBitset< BlockType > const & lhs
		, DynamicBitset< BlockType > const & rhs )
	{
		return !( lhs == rhs );
	}

	template< typename BlockType >
	inline DynamicBitset< BlockType > operator<<( DynamicBitset< BlockType > const & lhs
		, int rhs )
	{
		DynamicBitset< BlockType > result{ lhs };
		result <<= rhs;
		return result;
	}

	template< typename BlockType >
	inline DynamicBitset< BlockType > operator>>( DynamicBitset< BlockType > const & lhs
		, int rhs )
	{
		DynamicBitset< BlockType > result{ lhs };
		result >>= rhs;
		return result;
	}

	template< typename BlockType >
	inline DynamicBitset< BlockType > operator&( DynamicBitset< BlockType > const & lhs
		, DynamicBitset< BlockType > const & rhs )
	{
		DynamicBitset< BlockType > result{ lhs };
		result &= rhs;
		return result;
	}

	template< typename BlockType >
	inline DynamicBitset< BlockType > operator|( DynamicBitset< BlockType > const & lhs
		, DynamicBitset< BlockType > const & rhs )
	{
		DynamicBitset< BlockType > result{ lhs };
		result |= rhs;
		return result;
	}

	template< typename BlockType >
	inline DynamicBitset< BlockType > operator^( DynamicBitset< BlockType > const & lhs
		, DynamicBitset< BlockType > const & rhs )
	{
		DynamicBitset< BlockType > result{ lhs };
		result ^= rhs;
		return result;
	}

	//*************************************************************************

	template< typename BlockType >
	inline typename DynamicBitset< BlockType >::Bit & DynamicBitset< BlockType >::Bit::operator=( bool val )
	{
		m_block = val
			? m_block | m_mask
			: m_block & ~m_mask;
		return *this;
	}

	template< typename BlockType >
	inline typename DynamicBitset< BlockType >::Bit & DynamicBitset< BlockType >::Bit::operator=( DynamicBitset< BlockType >::Bit const & bit )
	{
		set( bit );
		return *this;
	}

	template< typename BlockType >
	inline void DynamicBitset< BlockType >::Bit::set( bool value )
	{
		// https://graphics.stanford.edu/~seander/bithacks.html#ConditionalSetOrClearBitsWithoutBranching
		m_block = ( m_block & ~m_mask ) | ( -value & m_mask );
	}

	template< typename BlockType >
	template< bool BadCall >
	inline void * DynamicBitset< BlockType >::Bit::operator&()const
	{
		static_assert( !BadCall, "Taking the address of a bit in a bitset is impossible." );
		return nullptr;
	}

	template< typename BlockType >
	DynamicBitset< BlockType >::Bit::operator bool()const
	{
		return ( m_block & m_mask )
			? true
			: false;
	}

	template< typename BlockType >
	inline typename DynamicBitset< BlockType >::Bit & DynamicBitset< BlockType >::Bit::operator|=( bool value )
	{
		set( ( value ) ? true : bool( *this ) );
		return *this;
	}

	template< typename BlockType >
	inline typename DynamicBitset< BlockType >::Bit & DynamicBitset< BlockType >::Bit::operator&=( bool value )
	{
		set( ( value ) ? bool( *this ) : false );
		return *this;
	}

	template< typename BlockType >
	inline typename DynamicBitset< BlockType >::Bit & DynamicBitset< BlockType >::Bit::operator^=( bool value )
	{
		set( ( value ) ? !bool( *this ) : bool( *this ) );
		return *this;
	}

	//*************************************************************************

	template< typename BlockType >
	inline bool operator==( typename DynamicBitset< BlockType >::Bit const & lhs
		, typename DynamicBitset< BlockType >::Bit const & rhs )
	{
		return bool( lhs ) == bool( rhs );
	}

	template< typename BlockType >
	inline bool operator!=( typename DynamicBitset< BlockType >::Bit const & lhs
		, typename DynamicBitset< BlockType >::Bit const & rhs )
	{
		return bool( lhs ) != bool( rhs );
	}

	template< typename BlockType >
	typename DynamicBitset< BlockType >::Bit operator|( typename DynamicBitset< BlockType >::Bit const & lhs
		, bool rhs )
	{
		typename DynamicBitset< BlockType >::Bit result{ lhs };
		result |= rhs;
		return result;
	}

	template< typename BlockType >
	typename DynamicBitset< BlockType >::Bit operator&( typename DynamicBitset< BlockType >::Bit const & lhs
		, bool rhs )
	{
		typename DynamicBitset< BlockType >::Bit result{ lhs };
		result &= rhs;
		return result;
	}

	template< typename BlockType >
	typename DynamicBitset< BlockType >::Bit operator^( typename DynamicBitset< BlockType >::Bit const & lhs
		, bool rhs )
	{
		typename DynamicBitset< BlockType >::Bit result{ lhs };
		result ^= rhs;
		return result;
	}

	//*************************************************************************
}

#pragma warning( pop )
