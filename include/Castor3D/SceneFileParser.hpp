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
#ifndef ___C3D_SceneFileParser___
#define ___C3D_SceneFileParser___

#include "Prerequisites.hpp"
#include "Light.hpp"
#include "Mesh.hpp"
#include "Viewport.hpp"

namespace Castor3D
{
	//! Scene file sections Enum
	/*!
	The enumeration which defines all the sections and subsections of a scene file
	*/
	typedef enum
	{	eSECTION_ROOT					//!< No section id est root
	,	eSECTION_CAMERA					//!< Camera section
	,	eSECTION_LIGHT					//!< Light section
	,	eSECTION_NODE					//!< SceneNode section
	,	eSECTION_OBJECT					//!< Geometry section
	,	eSECTION_FONT					//!< Font section
	,	eSECTION_TEXT_OVERLAY			//!< Text Overlay sections
	,	eSECTION_MESH					//!< Mesh subsection of a geometry section
	,	eSECTION_SUBMESH				//!< Submesh subsection of a Mesh subsection
	,	eSECTION_SMOOTHING_GROUP		//!< SmoothingGroup subsection of a Submesh subsection
	,	eSECTION_MATERIAL				//!< Material section
	,	eSECTION_PASS					//!< Pass subsection of a material section
	,	eSECTION_TEXTURE_UNIT			//!< TextureUnit subsection of a pass subsection
	,	eSECTION_GLSL_SHADER			//!< GLSL Shader subsection of a pass subsection
	,	eSECTION_GLSL_SHADER_PROGRAM	//!< GLSL Shader Program subsection of a shader subsection
	,	eSECTION_GLSL_SHADER_VARIABLE	//!< GLSL Shader frame variable subsection of a shader subsection
	,	eSECTION_HLSL_SHADER			//!< HLSL Shader subsection of a pass subsection
	,	eSECTION_HLSL_SHADER_PROGRAM	//!< HLSL Shader subsection of a pass subsection
	,	eSECTION_HLSL_SHADER_VARIABLE	//!< HLSL Shader frame variable subsection of a shader subsection
	,	eSECTION_CG_SHADER				//!< Cg Shader subsection of a pass subsection
	,	eSECTION_CG_SHADER_PROGRAM		//!< Cg Shader Program subsection of a shader subsection
	,	eSECTION_CG_SHADER_VARIABLE		//!< Cg Shader frame variable subsection of a shader program subsection
	,	eSECTION_COUNT
	}	eSECTION;
	//! The context used into parsing functions
	/*!
	While parsing a scene file, the context holds the important data retrieved
	\author Sylvain DOREMUS
	\date 25/08/2010
	*/
	class C3D_API SceneFileContext : public MemoryTraced<SceneFileContext>
	{
	public:

	public:
		Scene				*	pScene;
		SceneNodePtr			pSceneNode;
		GeometryPtr				pGeometry;
		MeshPtr					pMesh;
		SubmeshPtr				pSubmesh;
		size_t					uiSmoothingGroup;
		LightPtr				pLight;
		CameraPtr				pCamera;
		MaterialPtr				pMaterial;
		size_t					uiPass;
		TextureUnitPtr			pTextureUnit;
		ShaderProgramPtr		pShaderProgram;
		ShaderObjectPtr			pShaderObject;
		FrameVariablePtr		pFrameVariable;
		Overlay 			*	pOverlay;
		TextOverlayPtr			pTextOverlay;
		int						iFace1;
		int						iFace2;

		eLIGHT_TYPE			eLightType;
		eMESH_TYPE				eMeshType;
		eVIEWPORT_TYPE			eViewportType;

		String					strName;
		String					strName2;
		size_t					uiUInt;

		SceneNodePtr			m_pGeneralParentNode;

		Castor::Utils::File	*	pFile;

		unsigned long long		ui64Line;

		eSECTION				eSection;
		eSECTION				ePrevSection;

	public:
		/**
		 * Constructor
		 */
		SceneFileContext();
		/**
		 * Initialises all variables
		 */
		void Initialise( SceneNodePtr p_pNode);
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
		array<AttributeParserMap, eSECTION_COUNT> m_arrayMapParsers;

		SceneFileContextPtr m_pContext;

		String m_strSceneFilePath;

	public:
		/**
		 * Constructor
		 */
		SceneFileParser();
		/**
		 * Destructor
		 */
		~SceneFileParser();
		/**
		 * Parses the given file (expecting it to be in ESCN file format
		 *@param p_pathFile : [in] The file path
		 *@param p_pNode : [in] The optional node considered as root node for parsing
		 *@return true if successful, false if not
		 */
		bool ParseFile( Path const & p_pathFile, SceneNodePtr p_pNode=SceneNodePtr());
		/**
		 * Parses the given file (expecting it to be in ESCN file format
		 *@param p_scene : [in/out] The scene where all objects will be put
		 *@param p_file : [in/out] The file
		 *@param p_pNode : [in] The optional node considered as root node for parsing
		 *@return true if successful, false if not
		 */
		bool ParseFile( Scene & p_scene, File & p_file, SceneNodePtr p_pNode=SceneNodePtr());
		/**
		 * Parses the given file (expecting it to be in ESCN file format
		 *@param p_scene : [in/out] The scene where all objects will be put
		 *@param p_pathFile : [in] The file path
		 *@param p_pNode : [in] The optional node considered as root node for parsing
		 *@return true if successful, false if not
		 */
		bool ParseFile( Scene & p_scene, Path const & p_pathFile, SceneNodePtr p_pNode=SceneNodePtr());
		/**
		 * Parses the given file (expecting it to be in ESCN file format
		 *@param p_strSceneName : [in] The scene name
		 *@param p_pathFile : [in] The file path
		 *@return the parsed scene
		 */
		ScenePtr ParseFile( String const & p_strSceneName, Path const & p_pathFile);
		/**
		 * Parses the given file (expecting it to be in ESCN file format
		 *@param p_file : [in/out] The file
		 *@param p_pNode : [in] The optional node considered as root node for parsing
		 *@return true if successful, false if not
		 */
		bool ParseFile( File & p_file, SceneNodePtr p_pNode=SceneNodePtr());
		/**
		 * Parses the given file (expecting it to be in ESCN file format
		 *@param p_strSceneName : [in] The scene name
		 *@param p_file : [in/out] The file
		 *@return the parsed scene
		 */
		ScenePtr ParseFile( String const & p_strSceneName, File & p_file);
		/**
		 * Logs an error in the log file
		 */
		static void ParseError( String const & p_strError, SceneFileContextPtr p_pContext);
		/**
		 * Logs a warning in the log file
		 */
		static void ParseWarning( String const & p_strWarning, SceneFileContextPtr p_pContext);
		/**
		 * Parses a Point
		 */
		template <typename T, size_t Count>
		static bool ParseVector( String & p_strParams, Point<T, Count> & p_vResult, SceneFileContextPtr p_pContext)
		{
			bool l_bReturn = false;

			StringArray l_arrayValues = p_strParams.split( " \t,;");

			if (l_arrayValues.size() >= Count)
			{
				if (l_arrayValues.size() > Count)
				{
					PARSING_WARNING( "More arguments than needed, keeping only the first ones");
				}

				for (size_t i = 0 ; i < Count ; i++)
				{
					std::basic_istringstream<xchar> l_stream( l_arrayValues[i]);
					l_stream >> p_vResult[i];
				}

				l_bReturn = true;
			}
			else
			{
				PARSING_ERROR( "Wrong number of args");
			}

			return l_bReturn;
		}
		/**
		 * Parses a Point
		 */
		template <typename T, size_t Count>
		static bool ParseVector( String & p_strParams, Point<T, Count> & p_vResult)
		{
			bool l_bReturn = false;

			StringArray l_arrayValues = p_strParams.split( " \t,;");

			if (l_arrayValues.size() >= Count)
			{
				if (l_arrayValues.size() > Count)
				{
					Logger::LogWarning( "More arguments than needed, keeping only the last ones");

					while (l_arrayValues.size() > Count)
					{
						l_arrayValues.erase( l_arrayValues.begin());
					}
				}

				for (size_t i = 0 ; i < Count ; i++)
				{
					std::basic_istringstream<xchar> l_stream( l_arrayValues[i]);
					l_stream >> p_vResult[i];
				}

				l_bReturn = true;
			}
			else
			{
				Logger::LogError( "Wrong number of args");
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
