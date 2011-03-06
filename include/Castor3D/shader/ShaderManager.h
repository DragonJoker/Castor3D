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
#ifndef ___C3D_ShaderManager___
#define ___C3D_ShaderManager___

#include "../Prerequisites.h"

namespace Castor3D
{
	//! Shader programs manager
	/*!
	Manager used to hold the shader programs, holds it, destroys it during a rendering loop
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	class C3D_API ShaderManager
	{
	private:
		GlslShaderProgramPtrArray	m_arrayGLSLPrograms;	//!< The loaded GLSLshader programs
		GlslShaderProgramPtrArray	m_arrayGLSLToDelete;	//!< The array of shaders to destroy
		HlslShaderProgramPtrArray	m_arrayHLSLPrograms;	//!< The loaded HLSL shader programs
		HlslShaderProgramPtrArray	m_arrayHLSLToDelete;	//!< The array of shaders to destroy
		CgShaderProgramPtrArray		m_arrayCgPrograms;		//!< The loaded shader programs
		CgShaderProgramPtrArray		m_arrayCgToDelete;		//!< The array of shaders to destroy
		int m_nbVerticesOut;								//!< The maximal number of vertices the geometry shader can output

	public:
		/**
		 * Constructor
		 */
		ShaderManager();
		/**
		 * Destructor
		 */
		virtual ~ShaderManager();
		/**
		 * Destroys all the shaders of the array of shaders to destroy, flushes the list then
		 */
		void Update();
		/**
		 * Flushes the active shaders list and adds all it's shaders to the array of shaders to destroy
		 */
		void ClearShaders();
		/**
		* Adds a shader to the active GLSL shaders list
		*/
		void AddGLSLProgram( GlslShaderProgramPtr p_program);
		/**
		* Adds a shader to the active GLSL shaders list
		*/
		void AddHLSLProgram( HlslShaderProgramPtr p_program);
		/**
		* Remove the shader from the active Cg shaders list
		*/
		void AddCgProgram( CgShaderProgramPtr p_program);
		/**
		* Remove the shader from the active GLSL shaders list and add it to the array of shaders to destroy
		*/
		bool RemoveGLSLProgram( GlslShaderProgramPtr p_program);
		/**
		* Remove the shader from the active HLSL shaders list and add it to the array of shaders to destroy
		*/
		bool RemoveHLSLProgram( HlslShaderProgramPtr p_program);
		/**
		* Remove the shader from the active Cg shaders list and add it to the array of shaders to destroy
		*/
		bool RemoveCgProgram( CgShaderProgramPtr p_program);
	};
}

#endif