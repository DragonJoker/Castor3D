#include "CastorUtilsWorkerThreadTest.hpp"

#include <Multithreading/WorkerThread.hpp>

#include <atomic>

using namespace castor;

namespace Testing
{
	CastorUtilsWorkerThreadTest::CastorUtilsWorkerThreadTest()
		: TestCase( "CastorUtilsWorkerThreadTest" )
	{
	}

	CastorUtilsWorkerThreadTest::~CastorUtilsWorkerThreadTest()
	{
	}

	void CastorUtilsWorkerThreadTest::doRegisterTests()
	{
		doRegisterTest( "CastorUtilsWorkerThreadTest::SingleThread", std::bind( &CastorUtilsWorkerThreadTest::SingleThread, this ) );
		doRegisterTest( "CastorUtilsWorkerThreadTest::ProducerConsumer", std::bind( &CastorUtilsWorkerThreadTest::ProducerConsumer, this ) );
		doRegisterTest( "CastorUtilsWorkerThreadTest::MultipleSameTask", std::bind( &CastorUtilsWorkerThreadTest::MultipleSameTask, this ) );
	}

	void CastorUtilsWorkerThreadTest::SingleThread()
	{
		constexpr size_t count = 1000000u;
		WorkerThread worker;
		CT_CHECK( worker.isEnded() );
		std::vector< size_t > data;

		worker.feed( [&data, count]()
		{
			while ( data.size() < count )
			{
				data.push_back( data.size() );
			}
		} );

		CT_CHECK( !worker.wait( std::chrono::milliseconds( 1 ) ) );
		CT_CHECK( !worker.isEnded() );
		CT_CHECK( worker.wait( std::chrono::milliseconds( 0xFFFFFFFF ) ) );
		CT_CHECK( worker.isEnded() );
		CT_CHECK( data.size() == count );
	}

	void CastorUtilsWorkerThreadTest::ProducerConsumer()
	{
		constexpr size_t count = 1000000u;
		WorkerThread producer;
		WorkerThread consumer;
		std::atomic_int value{ 0 };

		producer.feed( [&value, &count]()
		{
			size_t i = 0;

			while ( i++ < count )
			{
				value++;
			}
		} );

		consumer.feed( [&value, &count]()
		{
			size_t i = 0;

			while ( i++ < count )
			{
				value--;
			}
		} );

		CT_CHECK( !producer.isEnded() );
		CT_CHECK( !consumer.isEnded() );
		CT_CHECK( producer.wait( std::chrono::milliseconds( 0xFFFFFFFF ) ) );
		CT_CHECK( consumer.wait( std::chrono::milliseconds( 0xFFFFFFFF ) ) );
		CT_CHECK( producer.isEnded() );
		CT_CHECK( consumer.isEnded() );
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

		worker1.feed( job );
		worker2.feed( job );

		CT_CHECK( !worker1.isEnded() );
		CT_CHECK( !worker2.isEnded() );
		CT_CHECK( worker1.wait( std::chrono::milliseconds( 0xFFFFFFFF ) ) );
		CT_CHECK( worker2.wait( std::chrono::milliseconds( 0xFFFFFFFF ) ) );
		CT_CHECK( worker1.isEnded() );
		CT_CHECK( worker2.isEnded() );
		CT_CHECK( value == count * 2 );
	}
}
