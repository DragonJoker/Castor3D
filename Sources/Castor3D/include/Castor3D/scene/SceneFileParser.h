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
	class SceneFileContext
	{
	public:
		typedef enum eSECTION
		{
			eNone,
			eCamera,
			eLight,
			eNode,
			eObject,
			eMesh,
			eSubmesh,
			eSmoothingGroup,
			eMaterial,
			ePass,
			eTextureUnit,
			eShader,
			eShaderVariable

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

		Light::eTYPE					eLightType;
		Mesh::eTYPE						eMeshType;

		String							strName;
		String							strName2;

		General::Utils::FileStream	*	pFile;

		unsigned long long				ui64Line;

		eSECTION						eSection;

	public:
		SceneFileContext()
		{
			Initialise();
		}

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

			strName.clear();
			strName2.clear();
			
			pFile				= NULL;

			ui64Line			= 0;
		}
	};

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
		SceneFileParser();
		~SceneFileParser();

		bool ParseFile( const String & p_strFileName);

		static void ParseError( const String & p_strError, SceneFileContext * p_pContext);
		static void ParseWarning( const String & p_strWarning, SceneFileContext * p_pContext);
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

				p_vResult.x = T( atof( l_arrayValues[0].char_str()));
				p_vResult.y = T( atof( l_arrayValues[1].char_str()));
				p_vResult.z = T( atof( l_arrayValues[2].char_str()));

				l_bReturn = true;
			}
			else
			{
				PARSING_ERROR( "Wrong number of args");
			}

			return l_bReturn;
		}
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

				p_vResult.x = T( atof( l_arrayValues[0].char_str()));
				p_vResult.y = T( atof( l_arrayValues[1].char_str()));
				p_vResult.z = T( atof( l_arrayValues[2].char_str()));
				p_vResult.w = T( atof( l_arrayValues[3].char_str()));

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

	public:
		virtual inline bool _delegateParser( String & p_line)	{ return false; }
	};
}

#endif
