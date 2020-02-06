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
#ifndef ___Gl_CgShaderObject___
#define ___Gl_CgShaderObject___

#include "Module_GlRender.hpp"

#include <Castor3D/ShaderObject.hpp>

namespace Castor3D
{
	class CgGlShaderObject : public CgShaderObject, public MemoryTraced<CgGlShaderObject>
	{
		friend class CgGlShaderProgram;

	protected:
		String m_compilerLog;

	public:
		/**
		 * Constructor
		 */
		CgGlShaderObject( CgGlShaderProgram * p_pParent, eSHADER_TYPE p_eType);
		/**
		 * Destructor
		 */
		virtual ~CgGlShaderObject();
		/**
		 * Compiles program
		 */
		virtual bool Compile();
		/**
		 * Destroys the program
		 */
		virtual void DestroyProgram();

		virtual void Bind();

		virtual void Unbind();

		void CreateProgram();
	};
}

#endif