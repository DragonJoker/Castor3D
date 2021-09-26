/* See LICENSE file in root folder */
#ifndef ___CUT_STRING_TEST_H___
#define ___CUT_STRING_TEST_H___

#include "CastorUtilsTestPrerequisites.hpp"

namespace Testing
{
	class CastorUtilsStringTest
		: public TestCase
	{
	public:
		CastorUtilsStringTest();

	private:
		void doRegisterTests() override;

	private:
		void StringConversions();
	};
}

#endif
