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
		void size();
		void initValue();
		void leftShift();
		void rightShift();
		void or();
		void and();
		void xor();
		void set();
	};
}

#endif
