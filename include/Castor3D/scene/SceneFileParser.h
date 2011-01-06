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
#ifndef ___C3D_SceneFileParser___
#define ___C3D_SceneFileParser___

#include "../Prerequisites.h"
#include "../light/Light.h"
#include "../geometry/mesh/Mesh.h"

namespace Castor3D
{
	//! The context used into parsing functions
	/*!
	While parsing a scene file, the context holds the important data retrieved
	\author Sylvain DOREMUS
	\date 25/08/2010
	*/
	class SceneFileContext : public MemoryTraced<SceneFileContext>
	{
	public:
		//! Scene file sections Enum
		/*!
		The enumeration which defines all the sections and subsections of a scene file
		*/
		typedef enum eSECTION
		{
			eNone,				//!< No section id est root
			eCamera,			//!< Camera section
			eLight,				//!< Light section
			eNode,				//!< SceneNode section
			eObject,			//!< Geometry section
			eMesh,				//!< Mesh subsection of a geometry section
			eSubmesh,			//!< Submesh subsection of a Mesh subsection
			eSmoothingGroup,	//!< SmoothingGroup subsection of a Submesh subsection
			eMaterial,			//!< Material section
			ePass,				//!< Pass subsection of a material section
			eTextureUnit,		//!< TextureUnit subsection of a pass subsection
			eGlShader,			//!< GLSL Shader subsection of a pass subsection
			eGlShaderProgram,	//!< GLSL Shader Program subsection of a shader subsection
			eGlShaderVariable,	//!< GLSL Shader frame variable subsection of a shader subsection
			eCgShader,			//!< Cg Shader subsection of a pass subsection
			eCgShaderProgram,	//!< Cg Shader Program subsection of a shader subsection
			eCgShaderVariable,	//!< Cg Shader frame variable subsection of a shader program subsection
		}
		eSECTION;

	public:
		MaterialManager				*	m_pMaterialManager;
		SceneManager				*	m_pSceneManager;
		MeshManager					*	m_pMeshManager;
		ImageManager				*	m_pImageManager;
		ShaderManager				*	m_pShaderManager;
		ScenePtr						pScene;
		SceneNodePtr					pSceneNode;
		GeometryPtr						pGeometry;
		MeshPtr							pMesh;
		SubmeshPtr						pSubmesh;
		size_t							uiSmoothingGroup;
		LightPtr						pLight;
		CameraPtr						pCamera;
		MaterialPtr						pMaterial;
		size_t							uiPass;
		TextureUnitPtr					pTextureUnit;
		ShaderProgramPtr				pShaderProgram;
		ShaderObjectPtr					pShaderObject;
		FrameVariablePtr				pFrameVariable;
		CgShaderProgramPtr				pCgShaderProgram;
		CgShaderObjectPtr				pCgShaderObject;
		CgFrameVariablePtr				pCgFrameVariable;
		int								iFace1;
		int								iFace2;

		Light::eTYPE					eLightType;
		Mesh::eTYPE						eMeshType;

		String							strName;
		String							strName2;

		Castor::Utils::File	*	pFile;

		unsigned long long				ui64Line;

		eSECTION						eSection;

	public:
		/**
		 * Constructor
		 */
		SceneFileContext( SceneManager * p_pSceneManager);
		/**
		 * Initialises all variables
		 */
		void Initialise();
	};
	//! ESCN file parser
	/*!
	Reads ESCN files and extracts all 3D data from it
	\author Sylvain DOREMUS
	\date 25/08/2010
	*/
	class C3D_API SceneFileParser : public MemoryTraced<SceneFileParser>
	{
	private:
		AttributeParserMap m_mapRootParsers;
		AttributeParserMap m_mapSceneNodeParsers;
		AttributeParserMap m_mapObjectParsers;
		AttributeParserMap m_mapMeshParsers;
		AttributeParserMap m_mapSubmeshParsers;
		AttributeParserMap m_mapSmoothingGroupParsers;
		AttributeParserMap m_mapLightParsers;
		AttributeParserMap m_mapCameraParsers;
		AttributeParserMap m_mapMaterialParsers;
		AttributeParserMap m_mapPassParsers;
		AttributeParserMap m_mapTextureUnitParsers;
		AttributeParserMap m_mapGlShaderParsers;
		AttributeParserMap m_mapGlShaderProgramParsers;
		AttributeParserMap m_mapGlShaderVariableParsers;
		AttributeParserMap m_mapCgShaderParsers;
		AttributeParserMap m_mapCgShaderProgramParsers;
		AttributeParserMap m_mapCgShaderVariableParsers;

		SceneFileContextPtr m_pContext;

		String m_strSceneFilePath;

	public:
		/**
		 * Constructor
		 */
		SceneFileParser( SceneManager * p_pSceneManager);
		/**
		 * Destructor
		 */
		~SceneFileParser();
		/**
		 * Parses the given file (expecting it to be in ESCN file format
		 *@param p_strFileName : [in] The file path
		 *@return true if successful, false if not
		 */
		bool ParseFile( const String & p_strFileName);
		/**
		 * Logs an error in the log file
		 */
		static void ParseError( const String & p_strError, SceneFileContextPtr p_pContext);
		/**
		 * Logs a warning in the log file
		 */
		static void ParseWarning( const String & p_strWarning, SceneFileContextPtr p_pContext);
		/**
		 * Parses a Point
		 */
		template <typename T, size_t Count>
		static bool ParseVector( String & p_strParams, Point<T, Count> & p_vResult, SceneFileContextPtr p_pContext)
		{
			bool l_bReturn = false;

			StringArray l_arrayValues = p_strParams.Split( " ");

			if (l_arrayValues.size() >= Count)
			{
				if (l_arrayValues.size() > Count)
				{
					PARSING_WARNING( "More arguments than needed");
				}

				for (size_t i = 0 ; i < Count ; i++)
				{
					p_vResult[i] = T( atof( l_arrayValues[i].c_str()));
				}

				l_bReturn = true;
			}
			else
			{
				PARSING_ERROR( "Wrong number of args");
			}

			return l_bReturn;
		}

	private:
		bool _parseScriptLine( String & p_line);
		bool _invokeParser( String & p_line, const AttributeParserMap & p_parsers);
		virtual inline bool _delegateParser( String & p_line)	{ return false; }
	};
}

#endif