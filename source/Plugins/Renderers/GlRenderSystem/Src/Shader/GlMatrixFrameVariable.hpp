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
#ifndef ___GL_MATRIX_FRAME_VARIABLE_H___
#define ___GL_MATRIX_FRAME_VARIABLE_H___

#include <Shader/MatrixFrameVariable.hpp>

#include "Shader/GlFrameVariableBase.hpp"

namespace GlRender
{
	template< typename T, uint32_t Rows, uint32_t Columns >
	class GlMatrixFrameVariable
		: public Castor3D::MatrixFrameVariable<T, Rows, Columns>
		, public GlFrameVariableBase
	{
	public:
		GlMatrixFrameVariable( OpenGl & p_gl, uint32_t p_occurences, GlShaderProgram & p_program );
		GlMatrixFrameVariable( OpenGl & p_gl, Castor3D::MatrixFrameVariable<T, Rows, Columns> & p_variable );
		virtual ~GlMatrixFrameVariable();

		virtual bool Initialise();
		virtual void Cleanup();
		virtual void Bind();
		virtual void Unbind() {}
	};
}

#include "GlMatrixFrameVariable.inl"

#endif
