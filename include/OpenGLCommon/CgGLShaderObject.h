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
#ifndef ___Gl_CgShaderObject___
#define ___Gl_CgShaderObject___

#include "Module_GlRender.h"

#include <Castor3D/shader/ShaderObject.h>

namespace Castor3D
{
	class CgGlShaderObject : public CgShaderObject
	{
		friend class CgGlShaderProgram;

	protected:
		String m_compilerLog;

	public:
		/**
		 * Constructor
		 */
		CgGlShaderObject( eSHADER_PROGRAM_TYPE p_eType);
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
	};

	//! Class holding Vertex Shader
	class CgGlVertexShader : public CgGlShaderObject
	{
	public:
		/**
		 * Constructor
		 */
		CgGlVertexShader();
		/**
		 * Destructor
		 */
		virtual ~CgGlVertexShader();
		/**
		 * Creates the program
		 */
		virtual void CreateProgram();
	};

	//! Class holding Fragment Shader
	class CgGlFragmentShader : public CgGlShaderObject
	{
	public:
		/**
		 * Constructor
		 */
		CgGlFragmentShader();
		/**
		 * Destructor
		 */
		virtual ~CgGlFragmentShader();
		/**
		 * Creates the program
		 */
		virtual void CreateProgram();

	};

	//! Class holding Geometry Shader
	class CgGlGeometryShader : public CgGlShaderObject
	{
	public:
		/**
		 * Constructor
		 */
		CgGlGeometryShader();
		/**
		 * Destructor
		 */
		virtual ~CgGlGeometryShader();
		/**
		 * Creates the program
		 */
		virtual void CreateProgram();
	};
}

#endif