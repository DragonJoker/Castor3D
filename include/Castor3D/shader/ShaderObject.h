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
#include "ShaderProgram.h"

namespace Castor3D
{
	class C3D_API ShaderObjectBase : public Serialisable, public Textable
	{
	protected:
		eSHADER_PROGRAM_TYPE m_programType;	//!< The program type
		String m_shaderSource;				//!< ASCII Source-Code
		Path m_pathFile;					//!< File containing Source-Code
		bool m_isCompiled;					//!< Tells if the program is compiled
		bool m_bError;						//!< Tells if the program is in error after compilation

	public:
		/**
		 * Constructor
		 */
		ShaderObjectBase( eSHADER_PROGRAM_TYPE p_eType)
			:	m_isCompiled( false)
			,	m_programType( p_eType)
			,	m_bError( false)
		{
		}
		/**
		 * Destructor
		 */
		virtual ~ShaderObjectBase()
		{
		}
		/**
		 * Loads a shader file.
		 *@param p_filename : [in] The name of the ASCII file containing the shader.
		 *@return returns 0 if everything is ok.\n -1: File not found\n -2: Empty File\n -3: no memory
		 */
		void SetFile( const Path & p_filename)
		{
			m_bError = false;

			m_pathFile.clear();
			m_shaderSource.clear();

			if ( ! p_filename.empty())
			{
				File l_file( p_filename.c_str(), File::eRead);

				if (l_file.IsOk())
				{
					size_t l_len = size_t( l_file.GetLength());

					if (l_len > 0)
					{
						m_pathFile = p_filename;
						l_file.CopyToString( m_shaderSource);
					}
				}
			}
		}
		/**
		 * Load program from text
		 *@param p_program : [in] Address of the memory containing the shader program.
		 */
		void SetSource( const String & p_program)
		{
			m_bError = false;
			m_shaderSource = p_program;
		}

		/**@name Inherited methods from Textable */
		//@{
		virtual bool Write( File & p_file)const
		{
			return true;
		}
		virtual bool Read( File & p_file)
		{
			return false;
		}
		//@}

		/**@name Inherited methods from Serialisable */
		//@{
		virtual bool Save( File & p_file)const
		{
			return true;
		}
		virtual bool Load( File & p_file)
		{
			return true;
		}
		//@}

		/**@name Accessors */
		//@{
		inline eSHADER_PROGRAM_TYPE		GetShaderType		()const	{ return m_programType; }
		inline bool						IsCompiled			()const	{ return m_isCompiled; }
		inline String &					GetSource			()		{ return m_shaderSource; }
		inline const String &			GetSource			()const	{ return m_shaderSource; }
		inline Path &					GetFile				()		{ return m_pathFile; }
		inline const Path &				GetFile				()const	{ return m_pathFile; }
		//@}
	};
	//! Shader object
	/*!
	Shader object representation, can be vertex, fragment or geometry shader
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	template <class ShObj>
	class C3D_API ShaderObject : public ShaderObjectBase
	{
	protected:
		ShaderProgram<ShObj> * m_pParent;
		FrameVariablePtrStrMap m_mapFrameVariables;

	public:
		/**
		 * Constructor
		 */
		ShaderObject( eSHADER_PROGRAM_TYPE p_eType)
			:	ShaderObjectBase( p_eType)
		{
		}
		/**
		 * Destructor
		 */
		virtual ~ShaderObject()
		{
		}
		/**
		 * Adds a uniform variable to pass to the shader objects
		 *@param p_pVariable : the variable to pass
		 */
		virtual void AddFrameVariable( FrameVariablePtr p_pVariable)
		{
			if (p_pVariable != NULL)
			{
				if (m_mapFrameVariables.find( p_pVariable->GetName()) == m_mapFrameVariables.end())
				{
					m_mapFrameVariables.insert( FrameVariablePtrStrMap::value_type( p_pVariable->GetName(), p_pVariable));
				}
			}
		}
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

		inline void	SetParent	( ShaderProgram<ShObj> * p_pParent) { m_pParent = p_pParent; }
		//@}
	};
	//! GLSL Shader object
	/*!
	Shader object representation, can be vertex, fragment or geometry shader
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	class C3D_API GlslShaderObject : public ShaderObject<GlslShaderObject>, public MemoryTraced<GlslShaderObject>
	{
	public:
		/**
		 * Constructor
		 */
		GlslShaderObject( eSHADER_PROGRAM_TYPE p_eType);
		/**
		 * Destructor
		 */
		virtual ~GlslShaderObject();
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

		const FrameVariablePtrStrMap &	GetFrameVariables	()const { return ShaderObject<GlslShaderObject>::GetFrameVariables(); }
		FrameVariablePtrStrMap &		GetFrameVariables	()		{ return ShaderObject<GlslShaderObject>::GetFrameVariables(); }
	};
	//! HLSL Shader object
	/*!
	Shader object representation, can be vertex, fragment or geometry shader
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	class C3D_API HlslShaderObject : public ShaderObject<HlslShaderObject>, public MemoryTraced<HlslShaderObject>
	{
	public:
		/**
		 * Constructor
		 */
		HlslShaderObject( eSHADER_PROGRAM_TYPE p_eType);
		/**
		 * Destructor
		 */
		virtual ~HlslShaderObject();
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

		const FrameVariablePtrStrMap &	GetFrameVariables	()const { return ShaderObject<HlslShaderObject>::GetFrameVariables(); }
		FrameVariablePtrStrMap &		GetFrameVariables	()		{ return ShaderObject<HlslShaderObject>::GetFrameVariables(); }
	};
	//! Shader object
	/*!
	Cg Shader object representation, can be vertex, fragment or geometry shader
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	class C3D_API CgShaderObject : public ShaderObject<CgShaderObject>
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
		CgShaderObject( eSHADER_PROGRAM_TYPE p_eType);
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

		const FrameVariablePtrStrMap &	GetFrameVariables	()const { return ShaderObject<CgShaderObject>::GetFrameVariables(); }
		FrameVariablePtrStrMap &		GetFrameVariables	()		{ return ShaderObject<CgShaderObject>::GetFrameVariables(); }
	};
}

#endif
