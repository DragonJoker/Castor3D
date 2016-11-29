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
#ifndef ___C3DGL_PROGRAM_INPUT_LAYOUT_H___
#define ___C3DGL_PROGRAM_INPUT_LAYOUT_H___

#include "GlRenderSystemPrerequisites.hpp"

#include "Common/GlHolder.hpp"

#include <Shader/ProgramInputLayout.hpp>

namespace GlRender
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.8.0
	\date		03/02/2016
	\brief		ProgramInputLayout class.
	\remark		Holds the vertex buffer declaration, initialised by the shader program when compiled.
	*/
	class GlProgramInputLayout
		: public Castor3D::ProgramInputLayout
		, public Holder
	{
	public:
		/**
		 *\brief		Constructor
		 *\param[in]	p_gl	The OpenGL APIs.
		 */
		GlProgramInputLayout( OpenGl & p_gl, Castor3D::RenderSystem & p_renderSystem );
		/**
		 *\~english
		 *\brief		Destructor.
		 */
		~GlProgramInputLayout();
		/**
		 *\copydoc		Castor3D::ProgramInputLayout::Initialise
		 */
		virtual bool Initialise( Castor3D::ShaderProgram const & p_program );
		/**
		 *\copydoc		Castor3D::ProgramInputLayout::Cleanup
		 */
		virtual void Cleanup();

	private:
		std::multimap< int, Castor3D::BufferElementDeclaration > DoListAttributes( Castor3D::ShaderProgram const & p_program );
		void DoListOther( Castor3D::ShaderProgram const & p_program );
	};
}

#endif
