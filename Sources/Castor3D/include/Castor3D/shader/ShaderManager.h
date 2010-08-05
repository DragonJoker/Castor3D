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
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___C3D_ShaderManager___
#define ___C3D_ShaderManager___

#include "../material/Module_Material.h"

namespace Castor3D
{
	//! Shader programs manager
	/*!
	Manager used to hold the shader programs, holds it, destroys it during a rendering loop
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	class CS3D_API ShaderManager : public General::Theory::AutoSingleton <ShaderManager>
	{
	private:
		ShaderProgramPtrArray	m_shaderPrograms;	//!< The loaded shader programs
		ShaderProgramPtrArray	m_arrayToDelete;	//!< The array of shaders to destroy
		DrawType m_inputPrimitiveType;				//!< The input primitive type for the geometry shader (GL_TRIANGLES, ...)
		DrawType m_outputPrimitiveType;				//!< The output primitive type for the geometry shader (GL_TRIANGLES, ...)
		int m_nbVerticesOut;						//!< The maximal number of vertices the geometry shader can output

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
		 * Adds a shader program to the manager
		 */
		void AddProgram( ShaderProgram * p_program);
		/**
		* Remove the shader from the active shaders list and add it to the array of shaders to destroy
		*/
		bool RemoveProgram( ShaderProgram * p_program);
	};
}

#endif