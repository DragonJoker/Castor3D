#include "BuddyAllocator.hpp"

#include "Exception/Assertion.hpp"
#include "Log/Logger.hpp"

namespace castor
{
	namespace
	{
		bool doIs2Pow( size_t value )
		{
			return value && !( value & ( value - 1 ) );
		}

		uint32_t doGetNext2Pow( size_t value )
		{
			uint32_t result = 0;

			if ( value > 1 )
			{
				if ( doIs2Pow( value ) )
				{
					result--;
				}

				while ( value >= 1 )
				{
					++result;
					value >>= 1;
				}
			}
			else
			{
				result = 1;
			}

			return result;
		}
	}

	BuddyAllocator::BuddyAllocator( uint32_t numLevels
		, uint32_t minBlockSize )
		: m_numLevels{ numLevels }
		, m_minBlockSize{ minBlockSize }
	{
		REQUIRE( numLevels < 32 );
		m_freeLists.resize( m_numLevels + 1 );
		m_memory.resize( m_minBlockSize * ( 1u << m_numLevels ) );
		m_freeLists[0u].push_back( Block{ m_memory.data() } );
	}

	BuddyAllocator::~BuddyAllocator()
	{
	}

	uint8_t * BuddyAllocator::allocate( size_t size )
	{
		uint8_t * result;

		if ( size <= m_memory.size() )
		{
			auto level = doGetLevel( size );
			result = doAllocate( level ).data;
			m_allocated.emplace_back( size_t( result - m_memory.data() ), level );
		}

		return result;
	}

	void BuddyAllocator::deallocate( void * pointer )
	{
		auto diff = size_t( reinterpret_cast< uint8_t * >( pointer ) - m_memory.data() );
		auto it = std::find_if( m_allocated.begin()
			, m_allocated.end()
			, [diff]( PointerLevel const & ptrLevel )
			{
				return ptrLevel.first == diff;
			} );
		REQUIRE( it != m_allocated.end() );

		if ( it != m_allocated.end() )
		{
			auto level = it->second;
			auto index = diff / doGetLevelSize( level );
			Block block{ reinterpret_cast< uint8_t * >( pointer ) };
			m_freeLists[level].push_back( block );
			doMergeLevel( level );
		}
	}

	uint32_t BuddyAllocator::doGetLevel( size_t size )
	{
		auto levelSize = m_memory.size();
		uint32_t level = 0;

		while ( size < levelSize )
		{
			levelSize >>= 1;
			++level;
		}

		if ( size > levelSize )
		{
			--level;
		}

		return level;
	}

	size_t BuddyAllocator::doGetLevelSize( uint32_t level )
	{
		return m_memory.size() / size_t( 1ull << level );
	}

	BuddyAllocator::Block BuddyAllocator::doAllocate( uint32_t level )
	{
		if ( level >= m_freeLists.size() )
		{
			return Block{ nullptr };
		}

		auto & freeList = m_freeLists[level];

		if ( freeList.empty() )
		{
			if ( level == 0 )
			{
				return Block{ nullptr };
			}

			auto buddy = doAllocate( level - 1 );
			freeList.push_back( Block{ buddy.data } );
			freeList.push_back( Block{ buddy.data + doGetLevelSize( level ) } );
		}

		auto buddy = freeList.front();
		freeList.erase( freeList.begin() );
		return buddy;
	}

	void BuddyAllocator::doMergeLevel( uint32_t level )
	{
	}
}
