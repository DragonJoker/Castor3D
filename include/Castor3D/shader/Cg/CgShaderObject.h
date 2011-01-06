/*
This source file is part of Castor3D (http://dragonjoker.co.cc

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
#ifndef ___C3D_CgShaderObject___
#define ___C3D_CgShaderObject___

#include "../ShaderObject.h"

namespace Castor3D
{
	//! Shader object
	/*!
	Cg Shader object representation, can be vertex, fragment or geometry shader
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	class C3D_API CgShaderObject : public ShaderObject
	{
		friend class CgShaderProgram;

	protected:
		CGprofile m_cgProfile;		//!< Cg Shader profile
		CGprogram m_cgProgram;		//!< Cg Shader program
		String m_compilerLog;		//!< Compiler log

	public:
		/**
		 * Constructor
		 */
		CgShaderObject();
		/**
		 * Destructor
		 */
		virtual ~CgShaderObject();
		/**
		 * Compiles program
		 */
		virtual bool Compile();

		virtual void Bind() = 0;

		virtual void Unbind() = 0;
		/**
		 * Creates the program
		 */
		virtual void CreateProgram()=0;
		/**
		 * Destroys the program
		 */
		virtual void DestroyProgram()=0;

		inline CGprofile	GetProfile	()const { return m_cgProfile; }
		inline CGprogram	GetProgram	()const { return m_cgProgram; }
	};
}

#endif