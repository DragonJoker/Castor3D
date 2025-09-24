/* See LICENSE file in root folder */
#ifndef ___CUT_UNIQUE_TEST_H___
#define ___CUT_UNIQUE_TEST_H___

#include "CastorUtilsTestPrerequisites.hpp"

namespace Testing
{
	class CastorUtilsUniqueTest
		: public TestCase
	{
	public:
		CastorUtilsUniqueTest();

	private:
		void doRegisterTests() override;

	private:
		void MultipleInstanceTest();
		void MemoryTest();
	};
}

#endif
