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
#ifndef ___C3D_SceneFileParser___
#define ___C3D_SceneFileParser___

#include "Module_Scene.h"
#include "../geometry/Module_Geometry.h"
#include "../light/Module_Light.h"
#include "../camera/Module_Camera.h"
#include "../light/Light.h"
#include "../geometry/mesh/Mesh.h"
#include "../shader/Module_Shader.h"
namespace Castor3D
{
	//! The context used into parsing functions
	/*!
	While parsing a scene file, the context holds the important data retrieved
	\author Sylvain DOREMUS
	\date 25/08/2010
	*/
	class SceneFileContext
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
			eShader,			//!< Shader subsection of a pass subsection
			eShaderVariable		//!< Shader uniform variable subsection of a shader subsection

		} eSECTION;

	public:
		Scene						*	pScene;
		SceneNode					*	pSceneNode;
		Geometry					*	pGeometry;
		Mesh						*	pMesh;
		Submesh						*	pSubmesh;
		SmoothingGroup				*	pSmoothingGroup;
		Light						*	pLight;
		Camera						*	pCamera;
		Material					*	pMaterial;
		Pass						*	pPass;
		TextureUnit					*	pTextureUnit;
		ShaderProgram				*	pShaderProgram;
		UniformVariable				*	pUniformVariable;
		Face						*	pFace1;
		Face						*	pFace2;

		Light::eTYPE					eLightType;
		Mesh::eTYPE						eMeshType;

		String							strName;
		String							strName2;

		General::Utils::FileStream	*	pFile;

		unsigned long long				ui64Line;

		eSECTION						eSection;

	public:
		/**
		 * Constructor
		 */
		SceneFileContext()
		{
			Initialise();
		}
		/**
		 * Initialises all variables
		 */
		void Initialise()
		{
			pScene				= NULL;
			pSceneNode			= NULL;
			pGeometry			= NULL;
			pMesh				= NULL;
			pSubmesh			= NULL;
			pSmoothingGroup		= NULL;
			pLight				= NULL;
			pCamera				= NULL;
			pMaterial			= NULL;
			pPass				= NULL;
			pTextureUnit		= NULL;
			pShaderProgram		= NULL;
			pUniformVariable	= NULL;
			pFace1				= NULL;
			pFace2				= NULL;

			strName.clear();
			strName2.clear();
			
			pFile				= NULL;

			ui64Line			= 0;
		}
	};
	//! ESCN file parser
	/*!
	Reads ESCN files and extracts all 3D data from it
	\author Sylvain DOREMUS
	\date 25/08/2010
	*/
	class CS3D_API SceneFileParser
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
		AttributeParserMap m_mapShaderParsers;
		AttributeParserMap m_mapShaderVariableParsers;

		SceneFileContext * m_pContext;

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
		 *@param p_strFileName : [in] The file path
		 *@return true if successful, false if not
		 */
		bool ParseFile( const String & p_strFileName);
		/**
		 * Logs an error in the log file
		 */
		static void ParseError( const String & p_strError, SceneFileContext * p_pContext);
		/**
		 * Logs a warning in the log file
		 */
		static void ParseWarning( const String & p_strWarning, SceneFileContext * p_pContext);
		/**
		 * Parses a Point2D
		 */
		template <typename T>
		static bool ParseVector2( String & p_strParams, Point2D<T> & p_vResult, SceneFileContext * p_pContext)
		{
			bool l_bReturn = false;

			StringArray l_arrayValues = p_strParams.Split( " ");

			if (l_arrayValues.size() >= 2)
			{
				if (l_arrayValues.size() > 2)
				{
					PARSING_WARNING( "More arguments than needed");
				}

				p_vResult.x = T( atof( l_arrayValues[0].c_str()));
				p_vResult.y = T( atof( l_arrayValues[1].c_str()));

				l_bReturn = true;
			}
			else
			{
				PARSING_ERROR( "Wrong number of args");
			}

			return l_bReturn;
		}
		/**
		 * Parses a Point3D
		 */
		template <typename T>
		static bool ParseVector3( String & p_strParams, Point3D<T> & p_vResult, SceneFileContext * p_pContext)
		{
			bool l_bReturn = false;

			StringArray l_arrayValues = p_strParams.Split( " ");

			if (l_arrayValues.size() >= 3)
			{
				if (l_arrayValues.size() > 3)
				{
					PARSING_WARNING( "More arguments than needed");
				}

				p_vResult.x = T( atof( l_arrayValues[0].c_str()));
				p_vResult.y = T( atof( l_arrayValues[1].c_str()));
				p_vResult.z = T( atof( l_arrayValues[2].c_str()));

				l_bReturn = true;
			}
			else
			{
				PARSING_ERROR( "Wrong number of args");
			}

			return l_bReturn;
		}
		/**
		 * Parses a Point4D
		 */
		template <typename T>
		static bool ParseVector4( String & p_strParams, Point4D<T> & p_vResult, SceneFileContext * p_pContext)
		{
			bool l_bReturn = false;

			StringArray l_arrayValues = p_strParams.Split( " ");

			if (l_arrayValues.size() >= 4)
			{
				if (l_arrayValues.size() > 4)
				{
					PARSING_WARNING( "More arguments than needed");
				}

				p_vResult.x = T( atof( l_arrayValues[0].c_str()));
				p_vResult.y = T( atof( l_arrayValues[1].c_str()));
				p_vResult.z = T( atof( l_arrayValues[2].c_str()));
				p_vResult.w = T( atof( l_arrayValues[3].c_str()));

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