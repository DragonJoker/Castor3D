/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___CUT_CastorUtilsMatrixTest___
#define ___CUT_CastorUtilsMatrixTest___

#include "CastorUtilsTestPrerequisites.hpp"

#include <SquareMatrix.hpp>
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
		void DoRegisterTests() override;

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
		Castor::Matrix4x4r m_mtx1;
		Castor::Matrix4x4r m_mtx2;

#if defined( CASTOR_USE_GLM )

		glm::mat4 m_mtx1glm;
		glm::mat4 m_mtx2glm;

#endif

	};
}

#endif
