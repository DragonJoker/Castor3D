/* See LICENSE file in root folder */
#ifndef ___CUT_WorkerThreadTest_H___
#define ___CUT_WorkerThreadTest_H___

#include "CastorUtilsTestPrerequisites.hpp"

namespace Testing
{
	class CastorUtilsWorkerThreadTest
		: public TestCase
	{
	public:
		CastorUtilsWorkerThreadTest();

	private:
		void doRegisterTests() override;

	private:
		void SingleThread();
		void ProducerConsumer();
		void MultipleSameTask();
	};
}

#endif
