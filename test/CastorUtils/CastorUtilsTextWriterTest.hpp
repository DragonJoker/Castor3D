/* See LICENSE file in root folder */
#ifndef ___CastorUtilsTextWriterTest_H___
#define ___CastorUtilsTextWriterTest_H___

#include "CastorUtilsTestPrerequisites.hpp"

namespace Testing
{
	class CastorUtilsTextWriterTest
		: public TestCase
	{
	public:
		CastorUtilsTextWriterTest();

	private:
		void doRegisterTests()override;

	private:
		void BaseTypes();
		void Point();
		void Quaternion();
		void RgbColour();
		void RgbaColour();
		void UnnamedBlock();
		void NamedBlock();
		void TypedBlock();
	};
}

#endif
