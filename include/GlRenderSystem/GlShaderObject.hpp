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
#ifndef ___Gl_ShaderObject___
#define ___Gl_ShaderObject___

#include "Module_GlRender.hpp"

namespace Castor3D
{
	class GlShaderObject : public GlslShaderObject
	{
		friend class GlShaderProgram;

	protected:
		GLuint m_shaderObject;				//!< Shader Object
		String m_compilerLog;
		GlShaderProgram * m_pShaderProgram;

	public:
		/**
		 * Constructor
		 */
		GlShaderObject( GlShaderProgram * p_pParent, eSHADER_TYPE p_eType);
		/**
		 * Destructor
		 */
		virtual ~GlShaderObject();
		/**
		 * Creates the GL Shader program
		 */
		virtual void CreateProgram();
		/**
		 * Compiles program
		 */
		virtual bool Compile();
		/**
		 * Compiles program
		 */
		inline void Bind() {}
		/**
		 * Compiles program
		 */
		inline void Unbind() {}
		/**
		 * Get compiler messages
		 */
		void RetrieveCompilerLog( String & p_strCompilerLog);
		/**
		 * Detaches this shader from it's program
		 */
		void Detach();
		/**
		 * Attaches this shader to the given program
		 */
		void AttachTo( GlShaderProgram * p_pProgram);
		/**
		 * Destroys the GL Shader Program
		 */
		virtual void DestroyProgram();
	};
}

#endif