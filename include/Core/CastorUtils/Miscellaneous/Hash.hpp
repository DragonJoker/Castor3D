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
	inline uint32_t hashCombine32( uint32_t & hash
		, T const & rhs )
	{
		constexpr uint32_t kMul = 0x9e3779b9u;
		std::hash< T > hasher;
		hash ^= hasher( rhs ) + kMul + ( hash << 6 ) + ( hash >> 2 );
		return hash;
	}

	template< typename T >
	inline uint32_t hashCombinePtr32( uint32_t & hash
		, T const & rhs )
	{
		constexpr uint32_t kMul = 0x9e3779b9u;
		std::hash< T const * > hasher;
		hash ^= hasher( &rhs ) + kMul + ( hash << 6 ) + ( hash >> 2 );
		return hash;
	}

	template< typename T >
	inline uint64_t hashCombine64( uint64_t & hash
		, T const & rhs )
	{
		constexpr uint64_t kMul = 0x9ddfea08eb382d69ULL;
		auto seed = hash;

		std::hash< T > hasher;
		uint64_t a = ( uint64_t( hasher( rhs ) ) ^ seed ) * kMul;
		a ^= ( a >> 47 );

		uint64_t b = ( seed ^ a ) * kMul;
		b ^= ( b >> 47 );

		hash = b * kMul;
		return hash;
	}

	template< typename T >
	inline uint64_t hashCombinePtr64( uint64_t & hash
		, T const & rhs )
	{
		const uint64_t kMul = 0x9ddfea08eb382d69ULL;
		auto seed = hash;

		std::hash< T const * > hasher;
		uint64_t a = ( uint64_t( hasher( &rhs ) ) ^ seed ) * kMul;
		a ^= ( a >> 47 );

		uint64_t b = ( seed ^ a ) * kMul;
		b ^= ( b >> 47 );

		hash = b * kMul;
		return hash;
	}

	namespace hashcomb
	{
		template< typename HashT, typename EnableT = void >
		struct HashCombinerT;

		template< typename HashT >
		struct HashCombinerT< HashT, std::enable_if_t< std::is_same_v< HashT, uint32_t > > >
		{
			template< typename T >
			static HashT combine( HashT & hash, T const & rhs )
			{
				return hashCombine32( hash, rhs );
			}

			template< typename T >
			static HashT combinePtr( HashT & hash, T const & rhs )
			{
				return hashCombinePtr32( hash, rhs );
			}
		};

		template< typename HashT >
		struct HashCombinerT< HashT, std::enable_if_t< !std::is_same_v< HashT, uint32_t > && sizeof( HashT ) == 4u > >
		{
			template< typename T >
			static HashT combine( HashT & hash, T const & rhs )
			{
				auto hashed = uint32_t( hash );
				hash = HashT( hashCombine32( hashed, rhs ) );
				return hash;
			}

			template< typename T >
			static HashT combinePtr( HashT & hash, T const & rhs )
			{
				auto hashed = uint32_t( hash );
				hash = HashT( hashCombinePtr32( hashed, rhs ) );
				return hash;
			}
		};

		template< typename HashT >
		struct HashCombinerT< HashT, std::enable_if_t< std::is_same_v< HashT, uint64_t > > >
		{
			template< typename T >
			static HashT combine( HashT & hash, T const & rhs )
			{
				return hashCombine64( hash, rhs );
			}

			template< typename T >
			static HashT combinePtr( HashT & hash, T const & rhs )
			{
				return hashCombinePtr64( hash, rhs );
			}
		};

		template< typename HashT >
		struct HashCombinerT< HashT, std::enable_if_t< !std::is_same_v< HashT, uint64_t > && sizeof( HashT ) == 8u > >
		{
			template< typename T >
			static HashT combine( HashT & hash, T const & rhs )
			{
				auto hashed = uint64_t( hash );
				hash = HashT( hashCombine64( hashed, rhs ) );
				return hash;
			}

			template< typename T >
			static HashT combinePtr( HashT & hash, T const & rhs )
			{
				auto hashed = uint64_t( hash );
				hash = HashT( hashCombinePtr64( hashed, rhs ) );
				return hash;
			}
		};
	}

	template< typename T >
	inline size_t hashCombine( size_t & hash
		, T const & rhs )
	{
		return hashcomb::HashCombinerT< size_t >::combine( hash, rhs );
	}

	template< typename T >
	inline size_t hashCombinePtr( size_t & hash
		, T const & rhs )
	{
		return hashcomb::HashCombinerT< size_t >::combinePtr( hash, rhs );
	}
}

#endif
