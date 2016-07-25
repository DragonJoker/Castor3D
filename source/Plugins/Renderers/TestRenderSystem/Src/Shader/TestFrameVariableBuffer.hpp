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
#ifndef ___TRS_FRAME_VARIABLE_BUFFER_H___
#define ___TRS_FRAME_VARIABLE_BUFFER_H___

#include "TestRenderSystemPrerequisites.hpp"

#include <Shader/FrameVariableBuffer.hpp>

namespace TestRender
{
	class TestFrameVariableBuffer
		: public Castor3D::FrameVariableBuffer
	{
	public:
		TestFrameVariableBuffer( Castor::String const & p_name, TestRenderSystem & p_renderSystem );
		virtual ~TestFrameVariableBuffer();

	private:
		virtual Castor3D::FrameVariableSPtr DoCreateVariable( Castor3D::ShaderProgram * p_program, Castor3D::FrameVariableType p_type, Castor::String const & p_name, uint32_t p_occurences = 1 );
		virtual bool DoInitialise( Castor3D::ShaderProgram * p_program );
		virtual void DoCleanup();
		virtual bool DoBind( uint32_t p_index );
		virtual void DoUnbind( uint32_t p_index );
	};
}

#endif
