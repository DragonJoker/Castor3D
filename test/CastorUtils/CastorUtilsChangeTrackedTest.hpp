/* See LICENSE file in root folder */
#ifndef ___CUT_ChangeTrackedTest_H___
#define ___CUT_ChangeTrackedTest_H___

#include "CastorUtilsTestPrerequisites.hpp"

namespace Testing
{
	class CastorUtilsChangeTrackedTest
		: public TestCase
	{
	public:
		CastorUtilsChangeTrackedTest();

	private:
		void doRegisterTests()override;

	private:
		void BasicTest();
	};
}

#endif
