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
#ifndef ___CUT_CastorUtilsMatrixTest___
#define ___CUT_CastorUtilsMatrixTest___

#include "CastorUtilsTestPrerequisites.hpp"

#include <Math/SquareMatrix.hpp>
#if defined( CASTOR_USE_GLM )
#	include <glm/glm.hpp>
#endif

namespace Testing
{
	class CastorUtilsMatrixTest
		: public TestCase
	{
	public:
		CastorUtilsMatrixTest();
		virtual ~CastorUtilsMatrixTest();

	private:
		void doRegisterTests() override;
		bool compare( castor::Matrix3x3f const & lhs, castor::Matrix3x3f const & rhs );
		bool compare( castor::Matrix3x3d const & lhs, castor::Matrix3x3d const & rhs );
		bool compare( castor::Matrix4x4f const & lhs, castor::Matrix4x4f const & rhs );
		bool compare( castor::Matrix4x4d const & lhs, castor::Matrix4x4d const & rhs );

	private:
		void MatrixInversion();

#if defined( CASTOR_USE_GLM )

		void MatrixInversionComparison();
		void MatrixMultiplicationComparison();
		void TransformationMatrixComparison();
		void ProjectionMatrixComparison();

#if GLM_VERSION >= 95

		void QuaternionComparison();

#endif

#endif

	};

	class CastorUtilsMatrixBench
		: public BenchCase
	{
	public:
		CastorUtilsMatrixBench();
		virtual ~CastorUtilsMatrixBench();
		virtual void Execute();

	private:
		void MatrixMultiplicationsCastor();
		void MatrixMultiplicationsGlm();
		void MatrixInversionCastor();
		void MatrixInversionGlm();
		void MatrixCopyCastor();
		void MatrixCopyGlm();

	private:
		castor::Matrix4x4r m_mtx1;
		castor::Matrix4x4r m_mtx2;

#if defined( CASTOR_USE_GLM )

		glm::mat4 m_mtx1glm;
		glm::mat4 m_mtx2glm;

#endif

	};
}

#endif
