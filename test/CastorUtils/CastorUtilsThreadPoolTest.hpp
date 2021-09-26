/* See LICENSE file in root folder */
#ifndef ___CUT_ThreadPoolTest_H___
#define ___CUT_ThreadPoolTest_H___

#include "CastorUtilsTestPrerequisites.hpp"

namespace Testing
{
	class CastorUtilsThreadPoolTest
		: public TestCase
	{
	public:
		CastorUtilsThreadPoolTest();

	private:
		void doRegisterTests() override;

	private:
		void Underload();
		void Exactload();
		void Overload();
	};
}

#endif
