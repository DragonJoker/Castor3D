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
		virtual ~CastorUtilsUniqueTest();

	private:
		void doRegisterTests() override;

	private:
		void NoInstanceTest();
		void OneInstanceTest();
		void MultipleInstanceTest();
		void MemoryTest();
	};
}

#endif
