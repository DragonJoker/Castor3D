#include "CastorUtilsThreadPoolTest.hpp"

#include <Multithreading/ThreadPool.hpp>

#include <atomic>

using namespace Castor;

namespace Testing
{
	CastorUtilsThreadPoolTest::CastorUtilsThreadPoolTest()
		: TestCase( "CastorUtilsThreadPoolTest" )
	{
	}

	CastorUtilsThreadPoolTest::~CastorUtilsThreadPoolTest()
	{
	}

	void CastorUtilsThreadPoolTest::DoRegisterTests()
	{
		DoRegisterTest( "CastorUtilsWorkerThreadTest::Underload", std::bind( &CastorUtilsThreadPoolTest::Underload, this ) );
		DoRegisterTest( "CastorUtilsWorkerThreadTest::Exactload", std::bind( &CastorUtilsThreadPoolTest::Exactload, this ) );
		DoRegisterTest( "CastorUtilsWorkerThreadTest::Overload", std::bind( &CastorUtilsThreadPoolTest::Overload, this ) );
	}

	void CastorUtilsThreadPoolTest::Underload()
	{
		constexpr size_t count = 1000000u;
		ThreadPool pool( 5u );
		std::vector< size_t > data;
		pool.PushJob( [&data, count]()
		{
			while ( data.size() < count )
			{
				data.push_back( data.size() );
			}
		} );

		CT_CHECK( !pool.WaitAll( std::chrono::milliseconds( 1 ) ) );
		CT_CHECK( !pool.IsEmpty() );
		CT_CHECK( pool.WaitAll( std::chrono::milliseconds( 0xFFFFFFFF ) ) );
		CT_CHECK( data.size() == count );
	}

	void CastorUtilsThreadPoolTest::Exactload()
	{
		constexpr size_t count = 1000000u;
		ThreadPool pool( 5u );
		std::atomic_int value{ 0 };

		auto job = [&value, count]()
		{
			size_t i = 0;

			while ( i++ < count )
			{
				value++;
			}
		};

		pool.PushJob( job );
		pool.PushJob( job );
		pool.PushJob( job );
		pool.PushJob( job );
		pool.PushJob( job );

		CT_CHECK( !pool.WaitAll( std::chrono::milliseconds( 1 ) ) );
		CT_CHECK( pool.IsEmpty() );
		CT_CHECK( pool.WaitAll( std::chrono::milliseconds( 0xFFFFFFFF ) ) );
		CT_CHECK( !pool.IsEmpty() );
		CT_CHECK( value == count * 5u );
	}

	void CastorUtilsThreadPoolTest::Overload()
	{
		constexpr size_t count = 1000000u;
		ThreadPool pool( 5u );
		std::atomic_int value{ 0 };

		auto job = [&value, count]()
		{
			size_t i = 0;

			while ( i++ < count )
			{
				value++;
			}
		};

		pool.PushJob( job );
		pool.PushJob( job );
		pool.PushJob( job );
		pool.PushJob( job );
		pool.PushJob( job );
		CT_CHECK( pool.IsEmpty() );
		CT_CHECK( !pool.WaitAll( std::chrono::milliseconds( 1 ) ) );

		pool.PushJob( job );
		//CT_CHECK( pool.IsEmpty() );
		CT_CHECK( !pool.WaitAll( std::chrono::milliseconds( 1 ) ) );

		CT_CHECK( pool.WaitAll( std::chrono::milliseconds( 0xFFFFFFFF ) ) );
		CT_CHECK( !pool.IsEmpty() );
		CT_CHECK( value == count * 6u );
	}
}
