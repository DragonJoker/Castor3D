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
#ifndef ___Cg_Dx10ShaderObject___
#define ___Cg_Dx10ShaderObject___

#include "Module_CgShader.hpp"

#if defined( Cg_Direct3D10 )
#include "CgShaderObject.hpp"

namespace CgShader
{
	class CgD3D10ShaderObject : public CgShaderObject
	{
		friend class CgD3D10ShaderProgram;

	protected:
		Castor::String m_compilerLog;

	public:
		/**
		 * Constructor
		 */
		CgD3D10ShaderObject( CgD3D10ShaderProgram * p_pParent, Castor3D::eSHADER_TYPE p_eType);
		/**
		 * Destructor
		 */
		virtual ~CgD3D10ShaderObject();
		/**
		 * Actually just assigns the CGprofile
		 */
		virtual void CreateProgram();
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
	};
}
#endif
#endif
