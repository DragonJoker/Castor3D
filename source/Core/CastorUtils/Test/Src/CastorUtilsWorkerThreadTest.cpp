#include "CastorUtilsWorkerThreadTest.hpp"

#include <Multithreading/WorkerThread.hpp>

#include <atomic>

using namespace Castor;

namespace Testing
{
	CastorUtilsWorkerThreadTest::CastorUtilsWorkerThreadTest()
		: TestCase( "CastorUtilsWorkerThreadTest" )
	{
	}

	CastorUtilsWorkerThreadTest::~CastorUtilsWorkerThreadTest()
	{
	}

	void CastorUtilsWorkerThreadTest::DoRegisterTests()
	{
		DoRegisterTest( "CastorUtilsWorkerThreadTest::SingleThread", std::bind( &CastorUtilsWorkerThreadTest::SingleThread, this ) );
		DoRegisterTest( "CastorUtilsWorkerThreadTest::ProducerConsumer", std::bind( &CastorUtilsWorkerThreadTest::ProducerConsumer, this ) );
		DoRegisterTest( "CastorUtilsWorkerThreadTest::MultipleSameTask", std::bind( &CastorUtilsWorkerThreadTest::MultipleSameTask, this ) );
	}

	void CastorUtilsWorkerThreadTest::SingleThread()
	{
		constexpr size_t l_count = 1000000u;
		WorkerThread l_worker;
		CT_CHECK( l_worker.IsEnded() );
		std::vector< size_t > l_data;

		l_worker.Feed( [&l_data, l_count]()
		{
			while ( l_data.size() < l_count )
			{
				l_data.push_back( l_data.size() );
			}
		} );

		CT_CHECK( !l_worker.Wait( std::chrono::milliseconds( 1 ) ) );
		CT_CHECK( !l_worker.IsEnded() );
		CT_CHECK( l_worker.Wait( std::chrono::milliseconds( 0xFFFFFFFF ) ) );
		CT_CHECK( l_worker.IsEnded() );
		CT_CHECK( l_data.size() == l_count );
	}

	void CastorUtilsWorkerThreadTest::ProducerConsumer()
	{
		constexpr size_t l_count = 1000000u;
		WorkerThread l_producer;
		WorkerThread l_consumer;
		std::atomic_int l_value{ 0 };

		l_producer.Feed( [&l_value, &l_count]()
		{
			size_t i = 0;

			while ( i++ < l_count )
			{
				l_value++;
			}
		} );

		l_consumer.Feed( [&l_value, &l_count]()
		{
			size_t i = 0;

			while ( i++ < l_count )
			{
				l_value--;
			}
		} );

		CT_CHECK( !l_producer.IsEnded() );
		CT_CHECK( !l_consumer.IsEnded() );
		CT_CHECK( l_producer.Wait( std::chrono::milliseconds( 0xFFFFFFFF ) ) );
		CT_CHECK( l_consumer.Wait( std::chrono::milliseconds( 0xFFFFFFFF ) ) );
		CT_CHECK( l_producer.IsEnded() );
		CT_CHECK( l_consumer.IsEnded() );
		CT_CHECK( l_value == 0 );
	}

	void CastorUtilsWorkerThreadTest::MultipleSameTask()
	{
		constexpr size_t l_count = 1000000u;
		WorkerThread l_worker1;
		WorkerThread l_worker2;
		std::atomic_int l_value{ 0 };

		auto l_job = [&l_value, &l_count]()
		{
			size_t i = 0;

			while ( i++ < l_count )
			{
				l_value++;
			}
		};

		l_worker1.Feed( l_job );
		l_worker2.Feed( l_job );

		CT_CHECK( !l_worker1.IsEnded() );
		CT_CHECK( !l_worker2.IsEnded() );
		CT_CHECK( l_worker1.Wait( std::chrono::milliseconds( 0xFFFFFFFF ) ) );
		CT_CHECK( l_worker2.Wait( std::chrono::milliseconds( 0xFFFFFFFF ) ) );
		CT_CHECK( l_worker1.IsEnded() );
		CT_CHECK( l_worker2.IsEnded() );
		CT_CHECK( l_value == l_count * 2 );
	}
}
