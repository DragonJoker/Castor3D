#include "CastorUtilsThreadPoolTest.hpp"

#include <CastorUtils/Multithreading/ThreadPool.hpp>

#include <atomic>

namespace Testing
{
	CastorUtilsThreadPoolTest::CastorUtilsThreadPoolTest()
		: TestCase( "CastorUtilsThreadPoolTest" )
	{
	}

	void CastorUtilsThreadPoolTest::doRegisterTests()
	{
		doRegisterTest( "Underload", [this](){ Underload(); } );
		doRegisterTest( "Exactload", [this](){ Exactload(); } );
		doRegisterTest( "Overload", [this](){ Overload(); } );
	}

	void CastorUtilsThreadPoolTest::Underload()
	{
		static constexpr size_t count = 1000000u;
		c3d::ThreadPool pool( 5u );
		c3d::Vector< size_t > data;
		pool.pushJob( [&data]()
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
		static constexpr size_t count = 1000000u;
		c3d::ThreadPool pool( 5u );
		std::atomic_int value{ 0 };

		auto job = [&value]()
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
		static constexpr size_t count = 1000000u;
		c3d::ThreadPool pool( 5u );
		std::atomic_int value{ 0 };

		auto job = [&value]()
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
		CT_CHECK( !pool.waitAll( std::chrono::milliseconds( 1 ) ) );

		CT_CHECK( pool.waitAll( std::chrono::milliseconds( 0xFFFFFFFF ) ) );
		CT_CHECK( !pool.isEmpty() );
		CT_CHECK( value == count * 6u );
	}
}
