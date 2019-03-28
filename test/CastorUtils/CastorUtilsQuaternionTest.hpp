/* See LICENSE file in root folder */
#ifndef ___CUT_CastorUtilsQuaternionTest___
#define ___CUT_CastorUtilsQuaternionTest___

#include "CastorUtilsTestPrerequisites.hpp"

#include <CastorUtils/Math/Quaternion.hpp>
#if defined( CASTOR_USE_GLM )
#	include <glm/glm.hpp>
#endif

namespace Testing
{
	class CastorUtilsQuaternionTest
		: public TestCase
	{
	public:
		CastorUtilsQuaternionTest();
		virtual ~CastorUtilsQuaternionTest();

	private:
		void doRegisterTests() override;

#if defined( CASTOR_USE_GLM )

		bool compare( castor::Matrix4x4f const & lhs, glm::mat4x4 const & rhs );
		bool compare( castor::Matrix4x4d const & lhs, glm::mat4x4 const & rhs );

#endif

	private:

#if defined( CASTOR_USE_GLM )

		void TransformationMatrixComparison();

#endif

	private:
		template< typename T, typename U >
		inline bool compare( T const & a, U const & b )
		{
			return TestCase::compare( a, b );
		}

		inline bool compare( castor::SquareMatrix< float, 4 > const & a, castor::SquareMatrix< float, 4 > const & b )
		{
			float epsilon = 0.0001f;
			return std::abs( a[0][0] - b[0][0] ) < epsilon
				&& std::abs( a[0][1] - b[0][1] ) < epsilon
				&& std::abs( a[0][2] - b[0][2] ) < epsilon
				&& std::abs( a[0][3] - b[0][3] ) < epsilon
				&& std::abs( a[1][0] - b[1][0] ) < epsilon
				&& std::abs( a[1][1] - b[1][1] ) < epsilon
				&& std::abs( a[1][2] - b[1][2] ) < epsilon
				&& std::abs( a[1][3] - b[1][3] ) < epsilon
				&& std::abs( a[2][0] - b[2][0] ) < epsilon
				&& std::abs( a[2][1] - b[2][1] ) < epsilon
				&& std::abs( a[2][2] - b[2][2] ) < epsilon
				&& std::abs( a[2][3] - b[2][3] ) < epsilon
				&& std::abs( a[3][0] - b[3][0] ) < epsilon
				&& std::abs( a[3][1] - b[3][1] ) < epsilon
				&& std::abs( a[3][2] - b[3][2] ) < epsilon
				&& std::abs( a[3][3] - b[3][3] ) < epsilon;
		}

		inline bool compare( castor::SquareMatrix< double, 4 > const & a, castor::SquareMatrix< double, 4 > const & b )
		{
			double epsilon = 0.0001;
			return std::abs( a[0][0] - b[0][0] ) < epsilon
				&& std::abs( a[0][1] - b[0][1] ) < epsilon
				&& std::abs( a[0][2] - b[0][2] ) < epsilon
				&& std::abs( a[0][3] - b[0][3] ) < epsilon
				&& std::abs( a[1][0] - b[1][0] ) < epsilon
				&& std::abs( a[1][1] - b[1][1] ) < epsilon
				&& std::abs( a[1][2] - b[1][2] ) < epsilon
				&& std::abs( a[1][3] - b[1][3] ) < epsilon
				&& std::abs( a[2][0] - b[2][0] ) < epsilon
				&& std::abs( a[2][1] - b[2][1] ) < epsilon
				&& std::abs( a[2][2] - b[2][2] ) < epsilon
				&& std::abs( a[2][3] - b[2][3] ) < epsilon
				&& std::abs( a[3][0] - b[3][0] ) < epsilon
				&& std::abs( a[3][1] - b[3][1] ) < epsilon
				&& std::abs( a[3][2] - b[3][2] ) < epsilon
				&& std::abs( a[3][3] - b[3][3] ) < epsilon;
		}

	};
}

#endif
