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
#ifndef ___TRS_ONE_FRAME_VARIABLE_H___
#define ___TRS_ONE_FRAME_VARIABLE_H___

#include "TestRenderSystemPrerequisites.hpp"

#include <Shader/OneUniform.hpp>

namespace TestRender
{
	template< typename T >
	class TestOneUniform
		: public Castor3D::OneUniform< T >
	{
	public:
		TestOneUniform( uint32_t p_occurences, TestShaderProgram & p_program );
		TestOneUniform( Castor3D::OneUniform< T > & p_variable );
		virtual ~TestOneUniform();

		virtual bool Initialise();
		virtual void Cleanup();
		virtual void Bind()const;
		virtual void Unbind()const {}
	};
}

#include "TestOneUniform.inl"

#endif
