#include "CastorUtils/Exception/Assertion.hpp"

namespace castor
{
	template< typename Traits >
	inline BuddyAllocatorT< Traits >::BuddyAllocatorT( uint32_t numLevels
		, uint32_t minBlockSize )
		: Traits{ minBlockSize * ( 1u << numLevels ) }
		, m_numLevels{ numLevels }
		, m_minBlockSize{ minBlockSize }
	{
		CU_Require( numLevels < 32 );
		m_freeLists.resize( m_numLevels + 1 );
		m_freeLists[0u].push_back( Block{ this->getPointer( 0u ) } );
	}

	template< typename Traits >
	inline BuddyAllocatorT< Traits >::~BuddyAllocatorT()
	{
	}

	template< typename Traits >
	inline bool BuddyAllocatorT< Traits >::hasAvailable( size_t size )const
	{
		auto level = doGetLevel( size );
		bool result = !size;

		if ( !result && size <= this->getSize() )
		{
			auto it = m_freeLists.rend() - ( level + 1 );

			while ( it != m_freeLists.rend() && it->empty() )
			{
				++it;
			}

			result = it != m_freeLists.rend();
		}

		return result;
	}

	template< typename Traits >
	inline typename BuddyAllocatorT< Traits >::PointerType BuddyAllocatorT< Traits >::allocate( size_t size )
	{
		typename BuddyAllocatorT< Traits >::PointerType result{};

		if ( size <= this->getSize() )
		{
			auto level = doGetLevel( size );
			result = doAllocate( level ).data;
			m_allocated.emplace_back( this->getOffset( result ), level );
		}

		return result;
	}

	template< typename Traits >
	inline void BuddyAllocatorT< Traits >::deallocate( typename BuddyAllocatorT< Traits >::PointerType pointer )
	{
		auto offset = this->getOffset( pointer );
		auto it = std::find_if( m_allocated.begin()
			, m_allocated.end()
			, [offset]( PointerLevel const & ptrLevel )
			{
				return ptrLevel.first == offset;
			} );
		CU_Require( it != m_allocated.end() );

		if ( it != m_allocated.end() )
		{
			auto level = it->second;
			auto index = uint32_t( offset / doGetLevelSize( level ) );
			Block block{ pointer };
			m_allocated.erase( it );
			doMergeLevel( block, index, level );
		}
	}

	template< typename Traits >
	size_t BuddyAllocatorT< Traits >::getTotalSize()const
	{
		return Traits::getSize();
	}

	template< typename Traits >
	inline uint32_t BuddyAllocatorT< Traits >::doGetLevel( size_t size )const
	{
		auto levelSize = this->getSize();
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

		return std::min( level, m_numLevels );
	}

	template< typename Traits >
	inline size_t BuddyAllocatorT< Traits >::doGetLevelSize( uint32_t level )const
	{
		return this->getSize() / size_t( 1ull << level );
	}

	template< typename Traits >
	inline typename BuddyAllocatorT< Traits >::Block BuddyAllocatorT< Traits >::doAllocate( uint32_t level )
	{
		auto & freeList = m_freeLists[level];

		if ( freeList.empty() )
		{
			if ( level == 0 )
			{
				return this->getNull();
			}

			auto buddy = doAllocate( level - 1 );

			if ( this->isNull( buddy.data ) )
			{
				return buddy;
			}

			freeList.push_back( Block{ PointerType( buddy.data ) } );
			freeList.push_back( Block{ PointerType( buddy.data + doGetLevelSize( level ) ) } );
		}

		auto buddy = freeList.front();
		freeList.erase( freeList.begin() );
		return buddy;
	}

	template< typename Traits >
	inline void BuddyAllocatorT< Traits >::doMergeLevel( typename BuddyAllocatorT< Traits >::Block const & block
		, uint32_t index
		, uint32_t level )
	{
		auto & freeList = m_freeLists[level];
		Block lhs;
		Block rhs;
		typename FreeList::iterator it;

		if ( index % 2u )
		{
			// RHS block => find LHS in free blocks of same level.
			lhs.data = PointerType( block.data - doGetLevelSize( level ) );
			it = std::find_if( freeList.begin()
				, freeList.end()
				, [lhs]( Block const & lookUp )
			{
				return lookUp.data == lhs.data;
			} );
			rhs = block;
		}
		else
		{
			// LHS block => find RHS in free blocks of same level.
			lhs = block;
			rhs.data = PointerType( block.data + doGetLevelSize( level ) );
			it = std::find_if( freeList.begin()
				, freeList.end()
				, [rhs]( Block const & lookUp )
			{
				return lookUp.data == rhs.data;
			} );
		}

		if ( it != freeList.end() )
		{
			// remove the other block from the list.
			freeList.erase( it );
			// add lhs to lower level list.
			auto offset = this->getOffset( lhs.data );
			auto newIndex = uint32_t( offset / doGetLevelSize( level - 1u ) );
			// merge lower level.
			doMergeLevel( lhs, newIndex, level - 1u );
		}
		else
		{
			freeList.push_back( block );
		}
	}
}
