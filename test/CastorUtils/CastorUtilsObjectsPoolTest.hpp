/* See LICENSE file in root folder */
#ifndef ___CUT_CastorUtilsObjectsPoolTest___
#define ___CUT_CastorUtilsObjectsPoolTest___

#include "CastorUtilsTestPrerequisites.hpp"

namespace Testing
{
	class CastorUtilsObjectsPoolTest
		: public TestCase
	{
	public:
		CastorUtilsObjectsPoolTest();

	private:
		void doRegisterTests() override;

	private:
		void ObjectPoolTest();
		void AlignedObjectPoolTest();
		void FixedSizePerformanceTest();
		void AllDeallPerformanceTest();
		void ScatteredMemoryPerformanceTest();
		void VariableSizePerformanceTest();
		void UniquePoolTest();
	};
}

#endif
