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
#ifndef ___C3DGL_PROGRAM_INPUT_LAYOUT_H___
#define ___C3DGL_PROGRAM_INPUT_LAYOUT_H___

#include "GlRenderSystemPrerequisites.hpp"

#include "GlHolder.hpp"

#include <ProgramInputLayout.hpp>

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
	};
}

#endif
