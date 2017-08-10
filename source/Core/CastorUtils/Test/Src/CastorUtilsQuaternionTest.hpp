/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___CUT_CastorUtilsQuaternionTest___
#define ___CUT_CastorUtilsQuaternionTest___

#include "CastorUtilsTestPrerequisites.hpp"

#include <Math/Quaternion.hpp>
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
