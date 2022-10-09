#include "CastorUtils/Design/DynamicBitset.hpp"

#include <algorithm>
#include <cstring>

#pragma warning( push )
#pragma warning( disable: 4804 )

namespace castor
{
	//*************************************************************************

	namespace details
	{
		template< typename BlockTypeT >
		BlockTypeT makeBitMask( size_t index )
		{
			BlockTypeT result{ 0x01 };
			result <<= index;
			return result;
		}

		template< typename BlockTypeT >
		size_t getBlockIndex( size_t index )
		{
			return index / DynamicBitsetT< BlockTypeT >::bitsPerBlock;
		}

		template< typename BlockTypeT >
		size_t getBitIndex( size_t index )
		{
			static constexpr size_t bitMask = DynamicBitsetT< BlockTypeT >::bitsPerBlock - 1u;
			return index & bitMask;
		}

		template< typename BlockTypeT >
		size_t getBlockCount( size_t index )
		{
			return getBlockIndex< BlockTypeT >( index ) + ( getBitIndex< BlockTypeT >( index ) ? 1u : 0u );
		}
	}

	//*************************************************************************

	template< typename BlockTypeT >
	inline DynamicBitsetT< BlockTypeT >::DynamicBitsetT()
		: m_bitCount{ 0u }
	{
	}

	template< typename BlockTypeT >
	inline DynamicBitsetT< BlockTypeT >::DynamicBitsetT( size_t size, bool value )
	{
		resize( size, value );
	}

	template< typename BlockTypeT >
	inline DynamicBitsetT< BlockTypeT >::DynamicBitsetT( String const & bits )
		: DynamicBitsetT{ bits.data(), bits.size() }
	{
	}

	template< typename BlockTypeT >
	inline DynamicBitsetT< BlockTypeT >::DynamicBitsetT( char const * bits )
		: DynamicBitsetT{ bits, std::strlen( bits ) }
	{
	}

	template< typename BlockTypeT >
	inline DynamicBitsetT< BlockTypeT >::DynamicBitsetT( char const * bits
		, size_t size )
		: m_blocks( details::getBlockCount< BlockTypeT >( size ), BlockTypeT{} )
		, m_bitCount{ size }
	{
		for ( auto i = 0u; i < size; ++i )
		{
			CU_Require( *bits == '0' || *bits == '1' );
			set( m_bitCount - i - 1u, ( *bits++ ) == '1' );
		}
	}

	template< typename BlockTypeT >
	inline void DynamicBitsetT< BlockTypeT >::set( size_t bit, bool value )
	{
		CU_Require( bit < m_bitCount );

		BlockTypeT & block = m_blocks[details::getBlockIndex< BlockTypeT >( bit )];
		BlockTypeT mask = BlockTypeT{ 1u } << details::getBitIndex< BlockTypeT >( bit );

		// Setting of the bit without branching
		// https://graphics.stanford.edu/~seander/bithacks.html#ConditionalSetOrClearBitsWithoutBranching
		block = ( block & ~mask ) | ( -value & mask );
	}

	template< typename BlockTypeT >
	inline bool DynamicBitsetT< BlockTypeT >::get( size_t bit )const
	{
		CU_Require( bit < m_bitCount );
		return ( m_blocks[details::getBlockIndex< BlockTypeT >( bit )] & details::makeBitMask< BlockTypeT >( details::getBitIndex< BlockTypeT >( bit ) ) )
			? true
			: false;
	}

	template< typename BlockTypeT >
	inline BlockTypeT DynamicBitsetT< BlockTypeT >::getBlock( size_t index )const
	{
		CU_Require( index < m_blocks.size() );
		return m_blocks[index];
	}

	template< typename BlockTypeT >
	inline void DynamicBitsetT< BlockTypeT >::reset()
	{
		static constexpr BlockTypeT zero{};
		std::fill( m_blocks.begin()
			, m_blocks.end()
			, zero );
	}

	template< typename BlockTypeT >
	inline void DynamicBitsetT< BlockTypeT >::resize( size_t size, bool value )
	{
		size_t lastBlockIndex = m_blocks.size() - 1u;
		m_blocks.resize( details::getBlockCount< BlockTypeT >( size ), ( value ? fullBitMask : BlockTypeT{} ) );
		size_t remainingBits = details::getBitIndex< BlockTypeT >( m_bitCount );

		if ( size > m_bitCount && remainingBits > 0u && value )
		{
			m_blocks[lastBlockIndex] |= BlockTypeT( fullBitMask << remainingBits );
		}

		m_bitCount = size;
		doResetExtraBits();
	}

	template< typename BlockTypeT >
	inline size_t DynamicBitsetT< BlockTypeT >::getSize()const
	{
		return m_bitCount;
	}

	template< typename BlockTypeT >
	inline size_t DynamicBitsetT< BlockTypeT >::getBlockCount()const
	{
		return m_blocks.size();
	}

	template< typename BlockTypeT >
	inline bool DynamicBitsetT< BlockTypeT >::none()const
	{
		return !any();
	}

	template< typename BlockTypeT >
	inline bool DynamicBitsetT< BlockTypeT >::any()const
	{
		return m_blocks.end() != std::find_if( m_blocks.begin()
			, m_blocks.end()
			, []( BlockTypeT const lookup )
			{
				return lookup != 0;
			} );
	}

	template< typename BlockTypeT >
	inline bool DynamicBitsetT< BlockTypeT >::all()const
	{
		bool result = false;

		if ( !m_blocks.empty() )
		{
			auto end = std::next( m_blocks.begin(), ptrdiff_t( m_blocks.size() - 1u ) );
			result = end == std::find_if( m_blocks.begin()
				, end
				, []( BlockTypeT const lookup )
				{
					return lookup != fullBitMask;
				} );
			// last block check
			result &= ( *end ) == ( fullBitMask & doGetLastBlockMask() );
		}

		return result;
	}

	template< typename BlockTypeT >
	inline typename DynamicBitsetT< BlockTypeT >::Bit DynamicBitsetT< BlockTypeT >::operator[]( size_t index )
	{
		return Bit{ m_blocks[details::getBlockIndex< BlockTypeT >( index )]
			, details::makeBitMask< BlockTypeT >( details::getBitIndex< BlockTypeT >( index ) ) };
	}

	template< typename BlockTypeT >
	inline bool DynamicBitsetT< BlockTypeT >::operator[]( size_t index )const
	{
		return get( index );
	}

	template< typename BlockTypeT >
	inline DynamicBitsetT< BlockTypeT > & DynamicBitsetT< BlockTypeT >::operator<<=( int value )
	{
		if ( value )
		{
			if ( size_t( value ) >= m_bitCount )
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
					std::fill_n( std::next( m_blocks.begin(), ptrdiff_t( m_blocks.size() - blockShift ) )
						, blockShift
						, BlockTypeT{} );
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

	template< typename BlockTypeT >
	inline DynamicBitsetT< BlockTypeT > & DynamicBitsetT< BlockTypeT >::operator>>=( int value )
	{
		if ( value )
		{
			if ( size_t( value ) >= m_bitCount )
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
						, BlockTypeT{} );
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

	template< typename BlockTypeT >
	inline DynamicBitsetT< BlockTypeT > & DynamicBitsetT< BlockTypeT >::operator&=( DynamicBitsetT const & value )
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

	template< typename BlockTypeT >
	inline DynamicBitsetT< BlockTypeT > & DynamicBitsetT< BlockTypeT >::operator|=( DynamicBitsetT< BlockTypeT > const & value )
	{
		DynamicBitsetT const & greater = ( getSize() > value.getSize() ) ? *this : value;
		DynamicBitsetT const & lesser = ( getSize() > value.getSize() ) ? value : *this;

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

	template< typename BlockTypeT >
	inline DynamicBitsetT< BlockTypeT > & DynamicBitsetT< BlockTypeT >::operator^=( DynamicBitsetT< BlockTypeT > const & value )
	{
		DynamicBitsetT const & greater = ( getSize() > value.getSize() ) ? *this : value;
		DynamicBitsetT const & lesser = ( getSize() > value.getSize() ) ? value : *this;

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

	template< typename BlockTypeT >
	inline DynamicBitsetT< BlockTypeT > DynamicBitsetT< BlockTypeT >::operator~()const
	{
		DynamicBitsetT result;

		for ( auto & block : result.m_blocks )
		{
			block = ~block;
		}

		result.doResetExtraBits();
		return result;
	}

	template< typename BlockTypeT >
	inline String DynamicBitsetT< BlockTypeT >::toString()const
	{
		String result( m_bitCount, '0' );

		for ( auto i = 0u; i < m_bitCount; ++i )
		{
			result[i] = get( i ) ? '1' : '0';
		}

		return result;
	}

	template< typename BlockTypeT >
	void DynamicBitsetT< BlockTypeT >::doResetExtraBits()
	{
		auto mask = doGetLastBlockMask();

		if ( mask )
		{
			m_blocks.back() &= mask;
		}
	}

	template< typename BlockTypeT >
	BlockTypeT DynamicBitsetT< BlockTypeT >::doGetLastBlockMask() const
	{
		return BlockTypeT( ( BlockTypeT{ 1u } << details::getBitIndex< BlockTypeT >( m_bitCount ) ) - 1u );
	}

	//*************************************************************************

	template< typename BlockTypeT >
	inline bool operator==( DynamicBitsetT< BlockTypeT > const & lhs
		, DynamicBitsetT< BlockTypeT > const & rhs )
	{
		DynamicBitsetT< BlockTypeT > const & greater = ( lhs.getSize() > rhs.getSize() ) ? lhs : rhs;
		DynamicBitsetT< BlockTypeT > const & lesser = ( lhs.getSize() > rhs.getSize() ) ? rhs : lhs;
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

	template< typename BlockTypeT >
	inline bool operator!=( DynamicBitsetT< BlockTypeT > const & lhs
		, DynamicBitsetT< BlockTypeT > const & rhs )
	{
		return !( lhs == rhs );
	}

	template< typename BlockTypeT >
	inline DynamicBitsetT< BlockTypeT > operator<<( DynamicBitsetT< BlockTypeT > const & lhs
		, int rhs )
	{
		DynamicBitsetT< BlockTypeT > result{ lhs };
		result <<= rhs;
		return result;
	}

	template< typename BlockTypeT >
	inline DynamicBitsetT< BlockTypeT > operator>>( DynamicBitsetT< BlockTypeT > const & lhs
		, int rhs )
	{
		DynamicBitsetT< BlockTypeT > result{ lhs };
		result >>= rhs;
		return result;
	}

	template< typename BlockTypeT >
	inline DynamicBitsetT< BlockTypeT > operator&( DynamicBitsetT< BlockTypeT > const & lhs
		, DynamicBitsetT< BlockTypeT > const & rhs )
	{
		DynamicBitsetT< BlockTypeT > result{ lhs };
		result &= rhs;
		return result;
	}

	template< typename BlockTypeT >
	inline DynamicBitsetT< BlockTypeT > operator|( DynamicBitsetT< BlockTypeT > const & lhs
		, DynamicBitsetT< BlockTypeT > const & rhs )
	{
		DynamicBitsetT< BlockTypeT > result{ lhs };
		result |= rhs;
		return result;
	}

	template< typename BlockTypeT >
	inline DynamicBitsetT< BlockTypeT > operator^( DynamicBitsetT< BlockTypeT > const & lhs
		, DynamicBitsetT< BlockTypeT > const & rhs )
	{
		DynamicBitsetT< BlockTypeT > result{ lhs };
		result ^= rhs;
		return result;
	}

	//*************************************************************************

	template< typename BlockTypeT >
	inline typename DynamicBitsetT< BlockTypeT >::Bit & DynamicBitsetT< BlockTypeT >::Bit::operator=( bool val )
	{
		m_block = BlockTypeT( val
			? m_block | m_mask
			: m_block & ~m_mask );
		return *this;
	}

	template< typename BlockTypeT >
	inline typename DynamicBitsetT< BlockTypeT >::Bit & DynamicBitsetT< BlockTypeT >::Bit::operator=( DynamicBitsetT< BlockTypeT >::Bit const & bit )
	{
		set( bit );
		return *this;
	}

	template< typename BlockTypeT >
	inline void DynamicBitsetT< BlockTypeT >::Bit::set( bool value )
	{
		// https://graphics.stanford.edu/~seander/bithacks.html#ConditionalSetOrClearBitsWithoutBranching
		m_block = ( m_block & ~m_mask ) | ( -value & m_mask );
	}

	template< typename BlockTypeT >
	template< bool BadCall >
	inline void * DynamicBitsetT< BlockTypeT >::Bit::operator&()const
	{
		static_assert( !BadCall, "Taking the address of a bit in a bitset is impossible." );
		return nullptr;
	}

	template< typename BlockTypeT >
	DynamicBitsetT< BlockTypeT >::Bit::operator bool()const
	{
		return ( m_block & m_mask )
			? true
			: false;
	}

	template< typename BlockTypeT >
	inline typename DynamicBitsetT< BlockTypeT >::Bit & DynamicBitsetT< BlockTypeT >::Bit::operator|=( bool value )
	{
		set( ( value ) ? true : bool( *this ) );
		return *this;
	}

	template< typename BlockTypeT >
	inline typename DynamicBitsetT< BlockTypeT >::Bit & DynamicBitsetT< BlockTypeT >::Bit::operator&=( bool value )
	{
		set( ( value ) ? bool( *this ) : false );
		return *this;
	}

	template< typename BlockTypeT >
	inline typename DynamicBitsetT< BlockTypeT >::Bit & DynamicBitsetT< BlockTypeT >::Bit::operator^=( bool value )
	{
		set( ( value ) ? !bool( *this ) : bool( *this ) );
		return *this;
	}

	//*************************************************************************

	template< typename BlockTypeT >
	inline bool operator==( typename DynamicBitsetT< BlockTypeT >::Bit const & lhs
		, typename DynamicBitsetT< BlockTypeT >::Bit const & rhs )
	{
		return bool( lhs ) == bool( rhs );
	}

	template< typename BlockTypeT >
	inline bool operator!=( typename DynamicBitsetT< BlockTypeT >::Bit const & lhs
		, typename DynamicBitsetT< BlockTypeT >::Bit const & rhs )
	{
		return bool( lhs ) != bool( rhs );
	}

	template< typename BlockTypeT >
	typename DynamicBitsetT< BlockTypeT >::Bit operator|( typename DynamicBitsetT< BlockTypeT >::Bit const & lhs
		, bool rhs )
	{
		typename DynamicBitsetT< BlockTypeT >::Bit result{ lhs };
		result |= rhs;
		return result;
	}

	template< typename BlockTypeT >
	typename DynamicBitsetT< BlockTypeT >::Bit operator&( typename DynamicBitsetT< BlockTypeT >::Bit const & lhs
		, bool rhs )
	{
		typename DynamicBitsetT< BlockTypeT >::Bit result{ lhs };
		result &= rhs;
		return result;
	}

	template< typename BlockTypeT >
	typename DynamicBitsetT< BlockTypeT >::Bit operator^( typename DynamicBitsetT< BlockTypeT >::Bit const & lhs
		, bool rhs )
	{
		typename DynamicBitsetT< BlockTypeT >::Bit result{ lhs };
		result ^= rhs;
		return result;
	}

	//*************************************************************************
}

#pragma warning( pop )
