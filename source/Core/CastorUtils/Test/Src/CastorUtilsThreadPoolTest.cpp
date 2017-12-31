#include "CastorUtilsThreadPoolTest.hpp"

#include <Multithreading/ThreadPool.hpp>

#include <atomic>

using namespace castor;

namespace Testing
{
	CastorUtilsThreadPoolTest::CastorUtilsThreadPoolTest()
		: TestCase( "CastorUtilsThreadPoolTest" )
	{
	}

	CastorUtilsThreadPoolTest::~CastorUtilsThreadPoolTest()
	{
	}

	void CastorUtilsThreadPoolTest::doRegisterTests()
	{
		doRegisterTest( "CastorUtilsWorkerThreadTest::Underload", std::bind( &CastorUtilsThreadPoolTest::Underload, this ) );
		doRegisterTest( "CastorUtilsWorkerThreadTest::Exactload", std::bind( &CastorUtilsThreadPoolTest::Exactload, this ) );
		doRegisterTest( "CastorUtilsWorkerThreadTest::Overload", std::bind( &CastorUtilsThreadPoolTest::Overload, this ) );
	}

	void CastorUtilsThreadPoolTest::Underload()
	{
		constexpr size_t count = 1000000u;
		ThreadPool pool( 5u );
		std::vector< size_t > data;
		pool.pushJob( [&data, count]()
		{
			while ( data.size() < count )
			{
				data.push_back( data.size() );
			}
		} );

		CT_CHECK( !pool.waitAll( std::chrono::milliseconds( 1 ) ) );
		CT_CHECK( !pool.isEmpty() );
		CT_CHECK( pool.waitAll( std::chrono::milliseconds( 0xFFFFFFFF ) ) );
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

		pool.pushJob( job );
		pool.pushJob( job );
		pool.pushJob( job );
		pool.pushJob( job );
		pool.pushJob( job );

		CT_CHECK( !pool.waitAll( std::chrono::milliseconds( 1 ) ) );
		CT_CHECK( pool.isEmpty() );
		CT_CHECK( pool.waitAll( std::chrono::milliseconds( 0xFFFFFFFF ) ) );
		CT_CHECK( !pool.isEmpty() );
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

		pool.pushJob( job );
		pool.pushJob( job );
		pool.pushJob( job );
		pool.pushJob( job );
		pool.pushJob( job );
		CT_CHECK( pool.isEmpty() );
		CT_CHECK( !pool.waitAll( std::chrono::milliseconds( 1 ) ) );

		pool.pushJob( job );
		//CT_CHECK( pool.isEmpty() );
		CT_CHECK( !pool.waitAll( std::chrono::milliseconds( 1 ) ) );

		CT_CHECK( pool.waitAll( std::chrono::milliseconds( 0xFFFFFFFF ) ) );
		CT_CHECK( !pool.isEmpty() );
		CT_CHECK( value == count * 6u );
	}
}
