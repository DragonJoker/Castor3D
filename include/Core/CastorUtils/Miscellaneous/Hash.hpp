/*
See LICENSE file in root folder
*/
#ifndef ___CU_Hash_HPP___
#define ___CU_Hash_HPP___

#include "CastorUtils/Miscellaneous/MiscellaneousModule.hpp"

#include <ctime>
#include <type_traits>

namespace castor
{
	template< typename T >
	inline size_t hashCombine( size_t & hash
		, T const & rhs )
	{
		const uint64_t kMul = 0x9ddfea08eb382d69ULL;
		auto seed = hash;

		std::hash< T > hasher;
		uint64_t a = ( hasher( rhs ) ^ seed ) * kMul;
		a ^= ( a >> 47 );

		uint64_t b = ( seed ^ a ) * kMul;
		b ^= ( b >> 47 );

		if constexpr ( sizeof( std::size_t ) == sizeof( uint32_t ) )
		{
			hash = std::size_t( b * kMul );
		}
		else
		{
			hash = b * kMul;
		}

		return hash;
	}

	template< typename T >
	inline size_t hashCombinePtr( size_t & hash
		, T const & rhs )
	{
		const uint64_t kMul = 0x9ddfea08eb382d69ULL;
		auto seed = hash;

		std::hash< T const * > hasher;
		uint64_t a = ( hasher( &rhs ) ^ seed ) * kMul;
		a ^= ( a >> 47 );

		uint64_t b = ( seed ^ a ) * kMul;
		b ^= ( b >> 47 );

		if constexpr ( sizeof( std::size_t ) == sizeof( uint32_t ) )
		{
			hash = std::size_t( b * kMul );
		}
		else
		{
			hash = b * kMul;
		}

		return hash;
	}
}

#endif
