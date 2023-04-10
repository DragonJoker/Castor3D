/* See LICENSE file in root folder */
#ifndef ___CUT_CastorUtilsSignalTest___
#define ___CUT_CastorUtilsSignalTest___

#include "CastorUtilsTestPrerequisites.hpp"

namespace Testing
{
	class CastorUtilsSignalTest
		: public TestCase
	{
	public:
		CastorUtilsSignalTest();

	private:
		void doRegisterTests() override;

	private:
		void Creation();
		void Assignment();
		void MultipleSignalConnectionAssignment();
	};
}

#endif
