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
		constexpr size_t l_count = 1000000u;
		ThreadPool l_pool( 5u );
		std::vector< size_t > l_data;
		l_pool.PushJob( [&l_data, l_count]()
		{
			while ( l_data.size() < l_count )
			{
				l_data.push_back( l_data.size() );
			}
		} );

		CT_CHECK( !l_pool.WaitAll( std::chrono::milliseconds( 1 ) ) );
		CT_CHECK( !l_pool.IsEmpty() );
		CT_CHECK( l_pool.WaitAll( std::chrono::milliseconds( 0xFFFFFFFF ) ) );
		CT_CHECK( l_data.size() == l_count );
	}

	void CastorUtilsThreadPoolTest::Exactload()
	{
		constexpr size_t l_count = 1000000u;
		ThreadPool l_pool( 5u );
		std::atomic_int l_value{ 0 };

		auto l_job = [&l_value, l_count]()
		{
			size_t i = 0;

			while ( i++ < l_count )
			{
				l_value++;
			}
		};

		l_pool.PushJob( l_job );
		l_pool.PushJob( l_job );
		l_pool.PushJob( l_job );
		l_pool.PushJob( l_job );
		l_pool.PushJob( l_job );

		CT_CHECK( !l_pool.WaitAll( std::chrono::milliseconds( 1 ) ) );
		CT_CHECK( l_pool.IsEmpty() );
		CT_CHECK( l_pool.WaitAll( std::chrono::milliseconds( 0xFFFFFFFF ) ) );
		CT_CHECK( !l_pool.IsEmpty() );
		CT_CHECK( l_value == l_count * 5u );
	}

	void CastorUtilsThreadPoolTest::Overload()
	{
		constexpr size_t l_count = 1000000u;
		ThreadPool l_pool( 5u );
		std::atomic_int l_value{ 0 };

		auto l_job = [&l_value, l_count]()
		{
			size_t i = 0;

			while ( i++ < l_count )
			{
				l_value++;
			}
		};

		l_pool.PushJob( l_job );
		l_pool.PushJob( l_job );
		l_pool.PushJob( l_job );
		l_pool.PushJob( l_job );
		l_pool.PushJob( l_job );
		CT_CHECK( l_pool.IsEmpty() );
		CT_CHECK( !l_pool.WaitAll( std::chrono::milliseconds( 1 ) ) );

		l_pool.PushJob( l_job );
		CT_CHECK( l_pool.IsEmpty() );
		CT_CHECK( !l_pool.WaitAll( std::chrono::milliseconds( 1 ) ) );

		CT_CHECK( l_pool.WaitAll( std::chrono::milliseconds( 0xFFFFFFFF ) ) );
		CT_CHECK( !l_pool.IsEmpty() );
		CT_CHECK( l_value == l_count * 6u );
	}
}
