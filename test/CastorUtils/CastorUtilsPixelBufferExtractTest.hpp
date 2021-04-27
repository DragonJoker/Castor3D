/* See LICENSE file in root folder */
#ifndef ___CUT_CastorUtilsPixelBufferExtractTest_H___
#define ___CUT_CastorUtilsPixelBufferExtractTest_H___

#include "CastorUtilsTestPrerequisites.hpp"

namespace Testing
{
	class CastorUtilsPixelBufferExtractTest
		:	public TestCase
	{
	public:
		CastorUtilsPixelBufferExtractTest();
		virtual ~CastorUtilsPixelBufferExtractTest();

	private:
		void doRegisterTests() override;

	private:
		void TestExtract1Component();
		void TestExtract2ContigComponents();
		void TestExtract3ContigComponents();
		void TestExtract2DisjointComponents();
		void TestExtract3DisjointComponents();
	};
}

#endif
