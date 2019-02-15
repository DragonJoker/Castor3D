/* See LICENSE file in root folder */
#ifndef ___CUT_DynamicBitsetTest_H___
#define ___CUT_DynamicBitsetTest_H___

#include "CastorUtilsTestPrerequisites.hpp"

namespace Testing
{
	class CastorUtilsDynamicBitsetTest
		: public TestCase
	{
	public:
		CastorUtilsDynamicBitsetTest();
		~CastorUtilsDynamicBitsetTest();

	private:
		void doRegisterTests()override;

	private:
		void sizeTest();
		void initValueTest();
		void leftShiftTest();
		void rightShiftTest();
		void orTest();
		void andTest();
		void xorTest();
		void setTest();
	};
}

#endif
