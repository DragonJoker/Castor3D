/* See LICENSE file in root folder */
#ifndef ___CUT_PIXEL_FORMAT_TEST_H___
#define ___CUT_PIXEL_FORMAT_TEST_H___

#include "CastorUtilsTestPrerequisites.hpp"

namespace Testing
{
	class CastorUtilsPixelFormatTest
		:	public TestCase
	{
	public:
		CastorUtilsPixelFormatTest();
		virtual ~CastorUtilsPixelFormatTest();

	private:
		void doRegisterTests() override;

	private:
		void TestPixelConversions();
		void TestBufferConversions();
	};
}

#endif
