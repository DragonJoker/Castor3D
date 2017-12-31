/* See LICENSE file in root folder */
#ifndef ___CUT_ZIP_TEST___
#define ___CUT_ZIP_TEST___

#include "CastorUtilsTestPrerequisites.hpp"

namespace Testing
{
	class CastorUtilsZipTest
		:	public TestCase
	{
	public:
		CastorUtilsZipTest();
		virtual ~CastorUtilsZipTest();

	private:
		void doRegisterTests() override;

	private:
		void ZipFile();
	};
}

#endif
