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
		 * Loads a shader file.
		 *@param filename The name of the ASCII file containing the shader.
		 *@return returns 0 if everything is ok.\n -1: File not found\n -2: Empty File\n -3: no memory
		 */
		virtual int Load( const String & p_filename);
		/**
		 * Load program from null-terminated char array.
		 *@param program Address of the memory containing the shader program.
		 */
		virtual int LoadFromMemory( const String & p_program);
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
	};
}

#endif