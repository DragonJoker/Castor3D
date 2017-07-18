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
		constexpr size_t count = 1000000u;
		WorkerThread worker;
		CT_CHECK( worker.IsEnded() );
		std::vector< size_t > data;

		worker.Feed( [&data, count]()
		{
			while ( data.size() < count )
			{
				data.push_back( data.size() );
			}
		} );

		CT_CHECK( !worker.Wait( std::chrono::milliseconds( 1 ) ) );
		CT_CHECK( !worker.IsEnded() );
		CT_CHECK( worker.Wait( std::chrono::milliseconds( 0xFFFFFFFF ) ) );
		CT_CHECK( worker.IsEnded() );
		CT_CHECK( data.size() == count );
	}

	void CastorUtilsWorkerThreadTest::ProducerConsumer()
	{
		constexpr size_t count = 1000000u;
		WorkerThread producer;
		WorkerThread consumer;
		std::atomic_int value{ 0 };

		producer.Feed( [&value, &count]()
		{
			size_t i = 0;

			while ( i++ < count )
			{
				value++;
			}
		} );

		consumer.Feed( [&value, &count]()
		{
			size_t i = 0;

			while ( i++ < count )
			{
				value--;
			}
		} );

		CT_CHECK( !producer.IsEnded() );
		CT_CHECK( !consumer.IsEnded() );
		CT_CHECK( producer.Wait( std::chrono::milliseconds( 0xFFFFFFFF ) ) );
		CT_CHECK( consumer.Wait( std::chrono::milliseconds( 0xFFFFFFFF ) ) );
		CT_CHECK( producer.IsEnded() );
		CT_CHECK( consumer.IsEnded() );
		CT_CHECK( value == 0 );
	}

	void CastorUtilsWorkerThreadTest::MultipleSameTask()
	{
		constexpr size_t count = 1000000u;
		WorkerThread worker1;
		WorkerThread worker2;
		std::atomic_int value{ 0 };

		auto job = [&value, &count]()
		{
			size_t i = 0;

			while ( i++ < count )
			{
				value++;
			}
		};

		worker1.Feed( job );
		worker2.Feed( job );

		CT_CHECK( !worker1.IsEnded() );
		CT_CHECK( !worker2.IsEnded() );
		CT_CHECK( worker1.Wait( std::chrono::milliseconds( 0xFFFFFFFF ) ) );
		CT_CHECK( worker2.Wait( std::chrono::milliseconds( 0xFFFFFFFF ) ) );
		CT_CHECK( worker1.IsEnded() );
		CT_CHECK( worker2.IsEnded() );
		CT_CHECK( value == count * 2 );
	}
}
