#include "CastorUtilsBuddyAllocatorTest.hpp"

#include <Pool/BuddyAllocator.hpp>

using namespace castor;

namespace Testing
{
	CastorUtilsBuddyAllocatorTest::CastorUtilsBuddyAllocatorTest()
		: TestCase{ "CastorUtilsBuddyAllocatorTest" }
	{
	}

	CastorUtilsBuddyAllocatorTest::~CastorUtilsBuddyAllocatorTest()
	{
	}

	void CastorUtilsBuddyAllocatorTest::doRegisterTests()
	{
		doRegisterTest( "SizeTest", std::bind( &CastorUtilsBuddyAllocatorTest::SizeTest, this ) );
		doRegisterTest( "AllocationTest", std::bind( &CastorUtilsBuddyAllocatorTest::AllocationTest, this ) );
		doRegisterTest( "DeallocationTest", std::bind( &CastorUtilsBuddyAllocatorTest::DeallocationTest, this ) );
	}

	void CastorUtilsBuddyAllocatorTest::SizeTest()
	{
		{
			BuddyAllocator allocator{ 4, 1 };
			CT_EQUAL( allocator.getSize(), 16 );
			for ( auto i = 1u; i < allocator.getSize(); ++i )
			{
				CT_CHECK( allocator.hasAvailable( i ) );
			}
		}
		{
			BuddyAllocator allocator{ 4, 2 };
			CT_EQUAL( allocator.getSize(), 32 );
			for ( auto i = 1u; i < allocator.getSize(); ++i )
			{
				CT_CHECK( allocator.hasAvailable( i ) );
			}
		}
		{
			BuddyAllocator allocator{ 5, 2 };
			CT_EQUAL( allocator.getSize(), 64 );
			for ( auto i = 1u; i < allocator.getSize(); ++i )
			{
				CT_CHECK( allocator.hasAvailable( i ) );
			}
		}
		{
			BuddyAllocator allocator{ 5, 3 };
			CT_EQUAL( allocator.getSize(), 96 );
			for ( auto i = 1u; i < allocator.getSize(); ++i )
			{
				CT_CHECK( allocator.hasAvailable( i ) );
			}
		}
		{
			BuddyAllocator allocator{ 31, 1 };
			CT_EQUAL( allocator.getSize(), 1u << 31u );
		}
		{
			BuddyAllocator allocator{ 16, 8 };
			CT_EQUAL( allocator.getSize(), 524288u );
		}
		{
			BuddyAllocator allocator{ 16, 128 };
			CT_EQUAL( allocator.getSize(), 8388608u );
		}
		{
			BuddyAllocator allocator{ 20, 96 };
			CT_EQUAL( allocator.getSize(), 100663296u );
		}
	}

	void CastorUtilsBuddyAllocatorTest::AllocationTest()
	{
		{
			BuddyAllocator allocator{ 4, 1 };
			auto buf1 = allocator.allocate( 16 );
			CT_NEQUAL( buf1, nullptr );
			CT_CHECK( !allocator.hasAvailable( 1 ) );
			CT_EQUAL( allocator.allocate( 1 ), nullptr );
		}
		{
			BuddyAllocator allocator{ 4, 1 };
			auto buf1 = allocator.allocate( 8 );
			CT_NEQUAL( buf1, nullptr );
			CT_CHECK( allocator.hasAvailable( 4 ) );
			auto buf2 = allocator.allocate( 4 );
			CT_NEQUAL( buf2, nullptr );
			CT_CHECK( allocator.hasAvailable( 4 ) );
			auto buf3 = allocator.allocate( 4 );
			CT_NEQUAL( buf3, nullptr );
			CT_CHECK( !allocator.hasAvailable( 2 ) );
			CT_EQUAL( allocator.allocate( 2 ), nullptr );
		}
		{
			BuddyAllocator allocator{ 4, 1 };
			auto buf1 = allocator.allocate( 1 );
			CT_NEQUAL( buf1, nullptr );
			auto buf2 = allocator.allocate( 1 );
			CT_NEQUAL( buf2, nullptr );
			auto buf3 = allocator.allocate( 1 );
			CT_NEQUAL( buf3, nullptr );
			auto buf4 = allocator.allocate( 1 );
			CT_NEQUAL( buf4, nullptr );
			auto buf5 = allocator.allocate( 1 );
			CT_NEQUAL( buf5, nullptr );
			auto buf6 = allocator.allocate( 1 );
			CT_NEQUAL( buf6, nullptr );
			auto buf7 = allocator.allocate( 1 );
			CT_NEQUAL( buf7, nullptr );
			auto buf8 = allocator.allocate( 1 );
			CT_NEQUAL( buf8, nullptr );
			auto buf9 = allocator.allocate( 1 );
			CT_NEQUAL( buf9, nullptr );
			auto buf10 = allocator.allocate( 1 );
			CT_NEQUAL( buf10, nullptr );
			auto buf11 = allocator.allocate( 1 );
			CT_NEQUAL( buf11, nullptr );
			auto buf12 = allocator.allocate( 1 );
			CT_NEQUAL( buf12, nullptr );
			auto buf13 = allocator.allocate( 1 );
			CT_NEQUAL( buf13, nullptr );
			auto buf14 = allocator.allocate( 1 );
			CT_NEQUAL( buf14, nullptr );
			auto buf15 = allocator.allocate( 1 );
			CT_NEQUAL( buf15, nullptr );
			auto buf16 = allocator.allocate( 1 );
			CT_NEQUAL( buf16, nullptr );
			CT_EQUAL( allocator.allocate( 1 ), nullptr );
		}
		{
			BuddyAllocator allocator{ 4, 1 };
			CT_CHECK( allocator.hasAvailable( 8 ) );
			auto buf1 = allocator.allocate( 8 );
			CT_NEQUAL( buf1, nullptr );
			CT_CHECK( allocator.hasAvailable( 5 ) );
			auto buf2 = allocator.allocate( 5 );
			CT_NEQUAL( buf2, nullptr );
			CT_CHECK( !allocator.hasAvailable( 4 ) );
			CT_EQUAL( allocator.allocate( 4 ), nullptr );
			CT_CHECK( !allocator.hasAvailable( 2 ) );
			CT_EQUAL( allocator.allocate( 2 ), nullptr );
		}
	}

	void CastorUtilsBuddyAllocatorTest::DeallocationTest()
	{
		{
			BuddyAllocator allocator{ 4, 1 };
			auto buf1 = allocator.allocate( 16 );
			CT_NEQUAL( buf1, nullptr );
			allocator.deallocate( buf1 );
			CT_NEQUAL( buf1 = allocator.allocate( 1 ), nullptr );
		}
		{
			BuddyAllocator allocator{ 4, 1 };
			auto buf1 = allocator.allocate( 8 );
			CT_NEQUAL( buf1, nullptr );
			auto buf2 = allocator.allocate( 4 );
			CT_NEQUAL( buf2, nullptr );
			auto buf3 = allocator.allocate( 4 );
			CT_NEQUAL( buf3, nullptr );
			allocator.deallocate( buf3 );
			allocator.deallocate( buf2 );
			allocator.deallocate( buf1 );
		}
	}
}
