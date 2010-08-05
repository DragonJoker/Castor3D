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
#ifndef ___C3D_ShaderProgram___
#define ___C3D_ShaderProgram___

#include "../material/Module_Material.h"

namespace Castor3D
{
	//! Shader program representation
	/*!
	A shader programs holds it's geometry, fragment and vertex shader objects
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	class CS3D_API ShaderProgram
	{
	protected:
		friend class ShaderManager;

		bool m_isLinked;					//!< Tells if the program is linked
		bool m_bError;
		ShaderObject * m_pVertexShader;
		ShaderObject * m_pFragmentShader;
		ShaderObject * m_pGeometryShader;
		bool m_enabled;						//!< Tells if this shader is enabled
		bool m_usesGeometryShader;			//!< Tells if this program uses a geometry shader
		bool m_manageMemory;				//!< Tells that the program manages it's memory
		DrawType m_nInputPrimitiveType;
		DrawType m_nOutputPrimitiveType;
		int m_nVerticesOut;
		String m_vertexFile;
		String m_fragmentFile;
		String m_geometryFile;
		String m_vertexProgram;
		String m_fragmentProgram;
		String m_geometryProgram;
		std::map <String, UniformVariable *> m_mapUniformVariables;
		bool m_bFromMemory;
		int m_iTangentAttribLocation;
		int m_iBitangentAttribLocation;

	public:
		/**
		 * Constructor
		 */
		ShaderProgram( const String & p_vertexShaderFile, const String & p_fragmentShaderFile, const String & p_geometryShaderFile);
		ShaderProgram();
		/**
		 * Destructor
		 */
		virtual ~ShaderProgram()=0;
		/**
		 * Adds a uniform variable to pass to the shader objects
		 *@param p_pUniformVariable : the variable to pass
		 */
		virtual void AddUniformVariable( UniformVariable * p_pUniformVariable);
		/**
		 * Retrieves the uniform variables this program holds
		 *@return the map of variables held by this program
		 */
		std::map <String, UniformVariable *> GetUniformVariables()const { return m_mapUniformVariables; }
		/**
		 * Cleans this up
		 */
		virtual void Cleanup();
		/** 
		 * Adds a Vertex, Fragment or Geometry Program 
		 *@param p_shaderObject : [in] The shader object.
		 */
//		void AddShader( ShaderObject * p_shaderObject);
		virtual void Initialise();
		/**
		 * Links all Shaders held by this program
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

	public:
		inline void 	UsesGeometryShader		( bool p_use)		{ m_usesGeometryShader = p_use;}
		inline void 	Enable					()					{ m_enabled = true; }
		inline void 	Disable					()					{ m_enabled = false; } 
		inline void 	ManageMemory			()					{ m_manageMemory = true;}
		inline void 	SetInputPrimitiveType	( DrawType p_type)	{ m_nInputPrimitiveType = p_type; }
		inline void 	SetOutputPrimitiveType	( DrawType p_type)	{ m_nOutputPrimitiveType = p_type; }
		inline void 	SetVerticesOut			( int p_nb)			{ m_nVerticesOut = p_nb; }

		inline bool 	UsesGeometryShader		()const { return m_usesGeometryShader; }
		inline String	GetVertexFile			()const { return m_vertexFile; }
		inline String	GetFragmentFile			()const { return m_fragmentFile; }
		inline String	GetGeometryFile			()const { return m_geometryFile; }
		inline int		GetTangentAttribLocation()const { return m_iTangentAttribLocation; }
		inline int		GetBitangentAttribLocation()const { return m_iBitangentAttribLocation; }

	private:
		virtual void _initialiseFromMemory();
		virtual void _initialiseFromFile();
	};
}

#endif