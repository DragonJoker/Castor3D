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
#ifndef ___GL_POINT_FRAME_VARIABLE_H___
#define ___GL_POINT_FRAME_VARIABLE_H___

#include <Shader/PointFrameVariable.hpp>

#include "Shader/GlFrameVariableBase.hpp"

namespace GlRender
{
	template <typename T, uint32_t Count>
	class GlPointFrameVariable
		: public Castor3D::PointFrameVariable< T, Count >
		, public GlFrameVariableBase
	{
	public:
		GlPointFrameVariable( OpenGl & p_gl, uint32_t p_occurences, GlShaderProgram & p_program );
		GlPointFrameVariable( OpenGl & p_gl, Castor3D::PointFrameVariable< T, Count > & p_variable );
		virtual ~GlPointFrameVariable();
		/**
		 *\copydoc		Castor3D::FrameVariable::Initialise
		 */
		bool Initialise()override;
		/**
		 *\copydoc		Castor3D::FrameVariable::Cleanup
		 */
		void Cleanup()override;
		/**
		 *\copydoc		Castor3D::FrameVariable::Bind
		 */
		bool Bind()const override;
		/**
		 *\copydoc		Castor3D::FrameVariable::Unbind
		 */
		void Unbind()const override {}
	};
}

#include "GlPointFrameVariable.inl"

#endif
