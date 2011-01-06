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
#ifndef ___C3D_ShaderProgram___
#define ___C3D_ShaderProgram___

#include "../Prerequisites.h"

namespace Castor3D
{
	//! Shader program representation
	/*!
	A shader programs holds it's geometry, fragment and vertex shader objects
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	class C3D_API ShaderProgram : public MemoryTraced<ShaderProgram>
	{
	public:
		typedef enum
		{
			eGlShader,
			eCgShader,
		}
		eSHADER_TYPE;

	protected:
		friend class ShaderManager;

		bool m_isLinked;					//!< Tells if the program is linked
		bool m_bError;
		ShaderObjectPtr m_pShaders[eNbShaderTypes];
		bool m_enabled;						//!< Tells if the shader is enabled
		bool m_usesGeometryShader;			//!< Tells if the program uses a geometry shader
		FrameVariablePtrStrMap m_mapFrameVariables;
		eSHADER_TYPE m_eType;

	public:
		/**
		 * Constructor
		 *@param p_vertexShaderFile : [in] The vertex shader file
		 *@param p_fragmentShaderFile : [in] The fragment shader file
		 *@param p_geometryShaderFile : [in] The geometry shader file
		 */
		ShaderProgram( const String & p_vertexShaderFile, const String & p_fragmentShaderFile, const String & p_geometryShaderFile);
		ShaderProgram();
		/**
		 * Destructor
		 */
		virtual ~ShaderProgram()=0;
		/**
		 * Adds a uniform variable to pass to the shader objects
		 *@param p_pVariable : the variable to pass
		 *@param p_pObject : [in] The target shader object
		 */
		virtual void AddFrameVariable( FrameVariablePtr p_pVariable, ShaderObjectPtr p_pObject);
		/**
		 * Adds a uniform variable to pass to the shader objects
		 *@param p_pVariable : the variable to pass
		 *@param p_eType : [in] The target shader object type
		 */
		virtual void AddFrameVariable( FrameVariablePtr p_pVariable, eSHADER_PROGRAM_TYPE p_eType);
		/**
		 * Retrieves the uniform variables the program holds
		 *@return the map of variables held by the program
		 */
		FrameVariablePtrStrMap GetFrameVariables()const { return m_mapFrameVariables; }
		/**
		 * Cleans the program up
		 */
		virtual void Cleanup();
		/** 
		 * Initialises the program
		 */
		virtual void Initialise();
		/**
		 * Links all Shaders held by the program
		 */
		virtual bool Link()=0;
		/**
		 * Begins the program
		 */
		virtual void Begin()=0;
		/**
		 * Pass all the variables to the shader objects
		 */
		virtual void ApplyAllVariables()=0;
		/**
		 * Ends the program
		 */
		virtual void End()=0;
		/**
		 * Loads and sets the vertex program to the one contained in the file
		 *@param p_strFile : The file address
		 */
		void SetVertexFile( const String & p_strFile);
		/**
		 * Loads and sets the fragment program to the one contained in the file
		 *@param p_strFile : The file address
		 */
		void SetFragmentFile( const String & p_strFile);
		/**
		 * Loads and sets the geometry program to the one contained in the file
		 *@param p_strFile : The file address
		 */
		void SetGeometryFile( const String & p_strFile);
		/**
		 * Loads and sets the vertex program to the one contained in the text
		 *@param p_strContent : The shader code
		 */
		void SetVertexText( const String & p_strContent);
		/**
		 * Loads and sets the fragment program to the one contained in the text
		 *@param p_strContent : The shader code
		 */
		void SetFragmentText( const String & p_strContent);
		/**
		 * Loads and sets the geometry program to the one contained in the text
		 *@param p_strContent : The shader code
		 */
		void SetGeometryText( const String & p_strContent);

	public:
		inline void 	UsesGeometryShader		( bool p_use)			{ m_usesGeometryShader = p_use;}
		inline void 	Enable					()						{ m_enabled = true; }
		inline void 	Disable					()						{ m_enabled = false; }

		inline bool 			UsesGeometryShader			()const							{ return m_usesGeometryShader; }
		inline ShaderObjectPtr	GetProgram					( eSHADER_PROGRAM_TYPE p_eType)const	{ return m_pShaders[p_eType]; }
		inline ShaderObjectPtr	GetVertexProgram			()const 						{ return m_pShaders[eVertexShader]; }
		inline ShaderObjectPtr	GetFragmentProgram			()const 						{ return m_pShaders[ePixelShader]; }
		inline ShaderObjectPtr	GetGeometryProgram			()const 						{ return m_pShaders[eGeometryShader]; }
		inline bool				IsLinked					()const 						{ return m_isLinked; }
		inline eSHADER_TYPE		GetType						()const							{ return m_eType; }

	private:
		void _setProgramFile( const String & p_strFile, eSHADER_PROGRAM_TYPE p_eType);
		void _setProgramText( const String & p_strFile, eSHADER_PROGRAM_TYPE p_eType);
	};
}

#endif