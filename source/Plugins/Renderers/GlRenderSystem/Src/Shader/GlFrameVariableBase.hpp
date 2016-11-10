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
#ifndef ___GL_FRAME_VARIABLE_BASE_H___
#define ___GL_FRAME_VARIABLE_BASE_H___

#include "GlRenderSystemPrerequisites.hpp"

#include "Shader/GlShaderProgram.hpp"

namespace GlRender
{
	class GlFrameVariableBase
		: public Holder
	{
	public:
		GlFrameVariableBase( OpenGl & p_gl, GlShaderProgram const & p_program );
		virtual ~GlFrameVariableBase();

		inline uint32_t GetGlName() const
		{
			return m_glName;
		}

	protected:
		void GetVariableLocation( char const * p_varName );
		template< typename Type > bool DoBind( Type const * p_value, uint32_t p_occurences )const;
		template< typename Type, int Count > bool DoBind( Type const * p_value, uint32_t p_occurences )const;
		template< typename Type, int Rows, int Columns > bool DoBind( Type const * p_value, uint32_t p_occurences )const;

	protected:
		uint32_t m_glName;
		GlShaderProgram const & m_program;
		bool m_presentInProgram;
	};
}

#include "GlFrameVariableBase.inl"

#endif
