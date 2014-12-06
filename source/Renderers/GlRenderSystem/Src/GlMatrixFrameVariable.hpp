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
#ifndef ___GL_MATRIX_FRAME_VARIABLE_H___
#define ___GL_MATRIX_FRAME_VARIABLE_H___

#include <MatrixFrameVariable.hpp>

#include "GlFrameVariableBase.hpp"

namespace GlRender
{
	template <typename T, uint32_t Rows, uint32_t Columns>
	class GlMatrixFrameVariable
		:	public Castor3D::MatrixFrameVariable<T, Rows, Columns>
		,	public GlFrameVariableBase
	{
	public:
		GlMatrixFrameVariable( OpenGl & p_gl, uint32_t p_uiOcc, GlShaderProgram * p_pProgram = NULL );
		GlMatrixFrameVariable( OpenGl & p_gl, Castor3D::MatrixFrameVariable<T, Rows, Columns> * p_pVariable );
		virtual ~GlMatrixFrameVariable();

		virtual bool Initialise();
		virtual void Cleanup();
		virtual void Bind() {}
		virtual void Apply();
	};
}

#include "GlMatrixFrameVariable.inl"

#endif
