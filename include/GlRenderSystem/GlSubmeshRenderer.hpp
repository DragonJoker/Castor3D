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
#ifndef ___Gl_SubmeshRenderer___
#define ___Gl_SubmeshRenderer___

#include "Module_GlRender.hpp"
#include "GlBuffer.hpp"

namespace Castor3D
{
	class GlSubmeshRenderer : public SubmeshRenderer
	{
	private:
		typedef void (GlSubmeshRenderer :: * DrawBuffersFunc)( const Pass & p_pass);
		typedef DrawBuffersFunc PDrawBuffersFunc;

	protected:
		ePRIMITIVE_TYPE m_eDrawType;
		eNORMALS_MODE m_eNormalsMode;
		GlVertexArrayObjects m_vao;
		PDrawBuffersFunc m_pfnDrawBuffers;

	public:
		GlSubmeshRenderer();
		virtual ~GlSubmeshRenderer();

		virtual void Cleanup();
		virtual void Initialise();

	private:
		virtual void _drawBuffers( const Pass & p_pass);
		void _createVAOs( ShaderProgramPtr p_pProgram);
		void _drawBuffersGl2( const Pass & p_pass);
		void _drawBuffersGl3( const Pass & p_pass);
	};
}

#endif