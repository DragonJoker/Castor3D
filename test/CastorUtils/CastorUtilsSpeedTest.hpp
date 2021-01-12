/* See LICENSE file in root folder */
#ifndef ___CUT_CastorUtilsSpeedTest_H___
#define ___CUT_CastorUtilsSpeedTest_H___

#include "CastorUtilsTestPrerequisites.hpp"

namespace Testing
{
	class CastorUtilsSpeedTest
		: public TestCase
	{
	public:
		CastorUtilsSpeedTest();
		virtual ~CastorUtilsSpeedTest();

	private:
		void doRegisterTests()override;

	private:
		void BasicTest();
		void ConversionTest();
		void AngleTest();
	};
}

#endif
