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
#ifndef ___TRS_POINT_FRAME_VARIABLE_H___
#define ___TRS_POINT_FRAME_VARIABLE_H___

#include "TestRenderSystemPrerequisites.hpp"

#include <Shader/PointFrameVariable.hpp>

namespace TestRender
{
	template <typename T, uint32_t Count>
	class TestPointFrameVariable
		: public Castor3D::PointFrameVariable< T, Count >
	{
	public:
		TestPointFrameVariable( uint32_t p_occurences, TestShaderProgram & p_program );
		TestPointFrameVariable( Castor3D::PointFrameVariable< T, Count > & p_variable );
		virtual ~TestPointFrameVariable();

		virtual bool Initialise();
		virtual void Cleanup();
		virtual bool Bind()const;
		virtual void Unbind()const {}
	};
}

#include "TestPointFrameVariable.inl"

#endif
