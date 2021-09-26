/* See LICENSE file in root folder */
#ifndef ___CUT_ARRAY_VIEW_TEST_H___
#define ___CUT_ARRAY_VIEW_TEST_H___

#include "CastorUtilsTestPrerequisites.hpp"

namespace Testing
{
	class CastorUtilsArrayViewTest
		: public TestCase
	{
	public:
		CastorUtilsArrayViewTest();

	private:
		void doRegisterTests()override;

	private:
		void BasicTest();
	};
}

#endif
