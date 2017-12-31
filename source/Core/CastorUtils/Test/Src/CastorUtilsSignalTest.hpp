/* See LICENSE file in root folder */
#ifndef ___CUT_CastorUtilsSignalTest___
#define ___CUT_CastorUtilsSignalTest___

#include "CastorUtilsTestPrerequisites.hpp"

#include <Math/SquareMatrix.hpp>
#if defined( CASTOR_USE_GLM )
#	include <glm/glm.hpp>
#endif

namespace Testing
{
	class CastorUtilsSignalTest
		: public TestCase
	{
	public:
		CastorUtilsSignalTest();
		virtual ~CastorUtilsSignalTest();

	private:
		void doRegisterTests() override;

	private:
		void Creation();
		void Assignment();
		void MultipleSignalConnectionAssignment();
	};
}

#endif
