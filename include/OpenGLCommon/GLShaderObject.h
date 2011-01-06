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
#ifndef ___GL_ShaderObject___
#define ___GL_ShaderObject___

#include "Module_GLRender.h"

namespace Castor3D
{
	class GLShaderObject : public ShaderObject
	{
		friend class GLShaderProgram;

	protected:
		GLuint m_shaderObject;				//!< Shader Object
		String m_compilerLog;
		GLShaderProgram * m_pShaderProgram;

	public:
		/**
		 * Constructor
		 */
		GLShaderObject();
		/**
		 * Destructor
		 */
		virtual ~GLShaderObject();
		/**
		 * Compiles program
		 */
		virtual bool Compile();
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
		void AttachTo( GLShaderProgram * p_pProgram);
		/**
		 * Destroys the GL Shader Program
		 */
		virtual void DestroyProgram();
	};

	//! Class holding Vertex Shader
	class GLVertexShader : public GLShaderObject
	{
	public:
		/**
		 * Constructor
		 */
		GLVertexShader();
		/**
		 * Destructor
		 */
		virtual ~GLVertexShader();
		/**
		 * Creates the GL Shader program
		 */
		virtual void CreateProgram();
	};

	//! Class holding Fragment Shader
	class GLFragmentShader : public GLShaderObject
	{
	public:
		/**
		 * Constructor
		 */
		GLFragmentShader();
		/**
		 * Destructor
		 */
		virtual ~GLFragmentShader();
		/**
		 * Creates the GL Shader program
		 */
		virtual void CreateProgram();

	};

	//! Class holding Geometry Shader
	class GLGeometryShader : public GLShaderObject
	{
	public:
		/**
		 * Constructor
		 */
		GLGeometryShader();
		/**
		 * Destructor
		 */
		virtual ~GLGeometryShader();
		/**
		 * Creates the GL Shader program
		 */
		virtual void CreateProgram();
	};
}

#endif