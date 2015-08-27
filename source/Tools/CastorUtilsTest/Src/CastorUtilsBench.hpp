/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

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
#ifndef ___CUT_CastorUtilsBenchmark___
#define ___CUT_CastorUtilsBenchmark___

#include "Benchmark.hpp"

#include <Matrix.hpp>
#include <TransformationMatrix.hpp>
#include <glm/glm.hpp>

namespace Testing
{
	class CastorUtilsBench
		:	public BenchCase
	{
	public:
		CastorUtilsBench();
		virtual ~CastorUtilsBench();
		virtual void Execute();

	private:
		void MatrixMultiplicationsCastor();
		void MatrixMultiplicationsGlm();
		void MatrixInversionCastor();
		void MatrixInversionGlm();
		void MatrixCopyCastor();
		void MatrixCopyGlm();
		void StrToWStrUsingConvert();
		void StrToWStrUsingWiden();
		void WStrToStrUsingConvert();
		void WStrToStrUsingNarrow();

		Castor::Matrix4x4r m_mtx1;
		Castor::Matrix4x4r m_mtx2;
		glm::mat4 m_mtx1glm;
		glm::mat4 m_mtx2glm;
		std::string m_strIn;
		std::wstring m_wstrIn;
		std::wstring m_wstrOut;
		std::string m_strOut;
	};
}

#endif
