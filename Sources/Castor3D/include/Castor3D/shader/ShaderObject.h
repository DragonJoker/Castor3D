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
#ifndef ___C3D_ShaderObject___
#define ___C3D_ShaderObject___

namespace Castor3D
{
	//! Shader object
	/*!
	Shader object representation, can be vertex, fragment or geometry shader
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	class CS3D_API ShaderObject
	{
	public:
		//! Shader object type
		/*!
		The different shader object types : vertex, fragment, geometry
		*/
		typedef enum eTYPE
		{
			eNone,
			eVertex,
			eFragment,
			eGeometry,
		} eTYPE;

	protected:
		friend class ShaderProgram;

		eTYPE m_programType;				//!< The program type
		unsigned char * m_shaderSource;		//!< ASCII Source-Code
		bool m_isCompiled;					//!< Tells if the program is compiled
		bool m_bError;						//!< Tells if the program is in error after compilation (
		bool m_memAlloc;					//!< true if memory for shader source was allocated

	public:
		/**
		 * Constructor
		 */
		ShaderObject()
			:	m_isCompiled( false),
				m_programType( eNone),
				m_shaderSource( NULL),
				m_memAlloc( false),
				m_bError( false)
		{}
		/**
		 * Destructor
		 */
		virtual ~ShaderObject()
		{
			if (m_shaderSource != NULL && m_memAlloc)
			{
				delete [] m_shaderSource;
			}
		}
		/**
		 * Loads a shader file.
		 *@param p_filename : [in] The name of the ASCII file containing the shader.
		 *@return returns 0 if everything is ok.\n -1: File not found\n -2: Empty File\n -3: no memory
		 */
		virtual int Load( const String & p_filename)=0;
		/**
		 * Load program from null-terminated char array.
		 *@param p_program : [in] Address of the memory containing the shader program.
		 */
		virtual int LoadFromMemory( const String & p_program)=0;
		/**
		 * Compiles program
		 */
		virtual bool Compile()=0;
	};
}

#endif