/*
See LICENSE file in root folder
*/
#ifndef ___CU_Hash_HPP___
#define ___CU_Hash_HPP___

#include "CastorUtils/Miscellaneous/MiscellaneousModule.hpp"

#include <ctime>

namespace castor
{
	template< typename T >
	inline size_t hashCombine( size_t & hash
		, T const & rhs )
	{
		const uint64_t kMul = 0x9ddfea08eb382d69ULL;
		auto seed = hash;

		std::hash< T const * > hasher;
		uint64_t a = ( hasher( &rhs ) ^ seed ) * kMul;
		a ^= ( a >> 47 );

		uint64_t b = ( seed ^ a ) * kMul;
		b ^= ( b >> 47 );

		hash = static_cast< std::size_t >( b * kMul );
		return hash;
	}
}

#endif
