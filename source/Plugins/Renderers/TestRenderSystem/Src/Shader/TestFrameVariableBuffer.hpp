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
		virtual Castor3D::FrameVariableSPtr DoCreateVariable( Castor3D::ShaderProgram * p_program, Castor3D::eFRAME_VARIABLE_TYPE p_type, Castor::String const & p_name, uint32_t p_occurences = 1 );
		virtual bool DoInitialise( Castor3D::ShaderProgram * p_program );
		virtual void DoCleanup();
		virtual bool DoBind( uint32_t p_index );
		virtual void DoUnbind( uint32_t p_index );
	};
}

#endif
