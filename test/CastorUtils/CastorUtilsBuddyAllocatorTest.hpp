/* See LICENSE file in root folder */
#ifndef ___CUT_CastorUtilsBuddyAllocatorTest_H___
#define ___CUT_CastorUtilsBuddyAllocatorTest_H___

#include "CastorUtilsTestPrerequisites.hpp"

namespace Testing
{
	class CastorUtilsBuddyAllocatorTest
		: public TestCase
	{
	public:
		CastorUtilsBuddyAllocatorTest();
		virtual ~CastorUtilsBuddyAllocatorTest();

	private:
		void doRegisterTests()override;

	private:
		void SizeTest();
		void AllocationTest();
		void DeallocationTest();
	};
}

#endif
