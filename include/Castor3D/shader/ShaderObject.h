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
#ifndef ___C3D_ShaderObject___
#define ___C3D_ShaderObject___

#include "../Prerequisites.h"

namespace Castor3D
{
	//! Shader object
	/*!
	Shader object representation, can be vertex, fragment or geometry shader
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	class C3D_API ShaderObject : public MemoryTraced<ShaderObject>
	{
	protected:
		ShaderProgram * m_pParent;
		eSHADER_PROGRAM_TYPE m_programType;	//!< The program type
		String m_shaderSource;		//!< ASCII Source-Code
		String m_strFile;			//!< File containing Source-Code
		bool m_isCompiled;			//!< Tells if the program is compiled
		bool m_bError;				//!< Tells if the program is in error after compilation
		FrameVariablePtrStrMap m_mapFrameVariables;

	public:
		/**
		 * Constructor
		 */
		ShaderObject();
		/**
		 * Destructor
		 */
		virtual ~ShaderObject();
		/**
		 * Loads a shader file.
		 *@param p_filename : [in] The name of the ASCII file containing the shader.
		 *@return returns 0 if everything is ok.\n -1: File not found\n -2: Empty File\n -3: no memory
		 */
		void SetFile( const String & p_filename);
		/**
		 * Load program from null-terminated char array.
		 *@param p_program : [in] Address of the memory containing the shader program.
		 */
		void SetSource( const String & p_program);
		/**
		 * Adds a uniform variable to pass to the shader objects
		 *@param p_pVariable : the variable to pass
		 */
		virtual void AddFrameVariable( FrameVariablePtr p_pVariable);
		/**
		 * Compiles program
		 */
		virtual bool Compile()=0;
		/**
		 * Creates the program
		 */
		virtual void CreateProgram()=0;
		/**
		 * Destroys the program
		 */
		virtual void DestroyProgram()=0;

	public:
		/**@name Accessors */
		//@{
		const FrameVariablePtrStrMap &	GetFrameVariables	()const { return m_mapFrameVariables; }
		FrameVariablePtrStrMap &		GetFrameVariables	()		{ return m_mapFrameVariables; }
		inline eSHADER_PROGRAM_TYPE		GetShaderType		()const	{ return m_programType; }
		inline bool						IsCompiled			()const	{ return m_isCompiled; }
		inline String &					GetSource			()		{ return m_shaderSource; }
		inline const String &			GetSource			()const	{ return m_shaderSource; }
		inline String &					GetFile				()		{ return m_strFile; }
		inline const String &			GetFile				()const	{ return m_strFile; }

		inline void	SetParent	( ShaderProgram * p_pParent) { m_pParent = p_pParent; }
		//@}

	};
}

#endif