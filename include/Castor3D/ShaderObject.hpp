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
#ifndef ___C3D_ShaderObject___
#define ___C3D_ShaderObject___

#include "Prerequisites.hpp"

namespace Castor3D
{
	//!< Base shader object class C3D_API
	/*!
	Used to share functions through the three supported shader languages (Glsl, Hlsl and Cg)
	\author Sylvain DOREMUS
	\version 0.6.1.0
	\date 03/01/2011
	*/
	class C3D_API ShaderObjectBase : public Serialisable
	{
	protected:
		ShaderProgramBase		*	m_pParent;
		eSHADER_TYPE		m_eType;				//!< The program type
		String						m_strSource;			//!< ASCII Source-Code
		Path						m_pathFile;				//!< File containing Source-Code
		bool						m_bCompiled;			//!< Tells if the program is compiled
		bool						m_bError;				//!< Tells if the program is in error after compilation
		FrameVariablePtrList		m_listFrameVariables;

	public:
		/**
		 * Constructor
		 */
		ShaderObjectBase( ShaderProgramBase * p_pParent, eSHADER_TYPE p_eType);
		/**
		 * Destructor
		 */
		virtual ~ShaderObjectBase();
		/**
		 * Loads a shader file.
		 *@param p_filename : [in] The name of the ASCII file containing the shader.
		 *@return returns 0 if everything is ok.\n -1: File not found\n -2: Empty File\n -3: no memory
		 */
		void SetFile( Path const & p_filename);
		/**
		 * Load program from text
		 *@param p_program : [in] Address of the memory containing the shader program.
		 */
		void SetSource( String const & p_strSource);
		/**
		 *
		 */
		virtual void Bind()=0;
		/**
		 *
		 */
		virtual void Unbind()=0;
		/**
		 * Compiles program
		 */
		virtual bool Compile();
		/**
		 * Creates the program
		 */
		virtual void CreateProgram()=0;
		/**
		 * Destroys the program
		 */
		virtual void DestroyProgram()=0;
		/**
		 * Adds a uniform variable to pass to the shader objects
		 *@param p_pVariable : the variable to pass
		 */
		virtual void AddFrameVariable( FrameVariablePtr p_pVariable);

		/**@name Accessors */
		//@{
		inline eSHADER_TYPE					GetShaderType			()const	{ return m_eType; }
		inline bool									IsCompiled				()const	{ return m_bCompiled; }
		inline String							&	GetSource				()		{ return m_strSource; }
		inline const String						&	GetSource				()const	{ return m_strSource; }
		inline Path								&	GetFile					()		{ return m_pathFile; }
		inline const Path						&	GetFile					()const	{ return m_pathFile; }
		FrameVariablePtrList::const_iterator		GetFrameVariablesBegin	()const { return m_listFrameVariables.begin(); }
		FrameVariablePtrList::iterator				GetFrameVariablesBegin	()		{ return m_listFrameVariables.begin(); }
		FrameVariablePtrList::const_iterator		GetFrameVariablesEnd	()const	{ return m_listFrameVariables.end(); }
		//@}
	};
	//! GLSL Shader object
	/*!
	Shader object representation, can be vertex, fragment or geometry shader
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	class C3D_API GlslShaderObject : public ShaderObjectBase, public MemoryTraced<GlslShaderObject>
	{
	public:
		/**
		 * Constructor
		 */
		GlslShaderObject( GlslShaderProgram * p_pParent, eSHADER_TYPE p_eType);
		/**
		 * Destructor
		 */
		virtual ~GlslShaderObject();
		DECLARE_SERIALISE_MAP()
	};
	//! HLSL Shader object
	/*!
	Shader object representation, can be vertex, fragment or geometry shader
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	class C3D_API HlslShaderObject : public ShaderObjectBase, public MemoryTraced<HlslShaderObject>
	{
	protected:
		String m_strEntryPoint;

	public:
		/**
		 * Constructor
		 */
		HlslShaderObject( HlslShaderProgram * p_pParent, eSHADER_TYPE p_eType);
		/**
		 * Destructor
		 */
		virtual ~HlslShaderObject();

		inline void SetEntryPoint( String const & p_strName)	{ m_strEntryPoint = p_strName; }

		inline const String	&	GetEntryPoint	()const	{ return m_strEntryPoint; }
		DECLARE_SERIALISE_MAP()
	};
	//! Shader object
	/*!
	Cg Shader object representation, can be vertex, fragment or geometry shader
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	class C3D_API CgShaderObject : public ShaderObjectBase
	{
		friend class CgShaderProgram;

	protected:
		CGprofile					m_cgProfile;		//!< Cg Shader profile
		CGprogram					m_cgProgram;		//!< Cg Shader program
		String						m_compilerLog;		//!< Compiler log
		String						m_strEntryPoint;

	public:
		/**
		 * Constructor
		 */
		CgShaderObject( CgShaderProgram * p_pParent, eSHADER_TYPE p_eType);
		/**
		 * Destructor
		 */
		virtual ~CgShaderObject();

		inline void			SetEntryPoint	( String const & p_strName)	{ m_strEntryPoint = p_strName; }
		inline CGprofile	GetProfile		()const						{ return m_cgProfile; }
		inline CGprogram	GetProgram		()const						{ return m_cgProgram; }

		inline String const &					GetEntryPoint		()const	{ return m_strEntryPoint; }
		DECLARE_SERIALISE_MAP()
	};
}

#endif
