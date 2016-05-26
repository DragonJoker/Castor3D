/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

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
#ifndef ___C3D_SCENE_FILE_PARSER_H___
#define ___C3D_SCENE_FILE_PARSER_H___

#include "Castor3DPrerequisites.hpp"

#include <FileParser.hpp>
#include <FileParserContext.hpp>

#include "Mesh/Submesh.hpp"

#include <OwnedBy.hpp>

namespace Castor3D
{
	//! Scene file sections Enum
	/*!
	The enumeration which defines all the sections and subsections of a scene file
	*/
	typedef enum eSECTION
		: uint32_t
	{
		eSECTION_ROOT = MAKE_SECTION_NAME( 'R', 'O', 'O', 'T' ),
		eSECTION_SCENE = MAKE_SECTION_NAME( 'S', 'C', 'N', 'E' ),
		eSECTION_WINDOW = MAKE_SECTION_NAME( 'W', 'N', 'D', 'W' ),
		eSECTION_SAMPLER = MAKE_SECTION_NAME( 'S', 'M', 'P', 'R' ),
		eSECTION_CAMERA = MAKE_SECTION_NAME( 'C', 'A', 'M', 'R' ),
		eSECTION_VIEWPORT = MAKE_SECTION_NAME( 'V', 'W', 'P', 'T' ),
		eSECTION_LIGHT = MAKE_SECTION_NAME( 'L', 'G', 'H', 'T' ),
		eSECTION_NODE = MAKE_SECTION_NAME( 'N', 'O', 'D', 'E' ),
		eSECTION_OBJECT = MAKE_SECTION_NAME( 'O', 'B', 'J', 'T' ),
		eSECTION_OBJECT_MATERIALS = MAKE_SECTION_NAME( 'O', 'M', 'T', 'L' ),
		eSECTION_FONT = MAKE_SECTION_NAME( 'F', 'O', 'N', 'T' ),
		eSECTION_PANEL_OVERLAY = MAKE_SECTION_NAME( 'P', 'O', 'V', 'L' ),
		eSECTION_BORDER_PANEL_OVERLAY = MAKE_SECTION_NAME( 'B', 'O', 'V', 'L' ),
		eSECTION_TEXT_OVERLAY = MAKE_SECTION_NAME( 'T', 'O', 'V', 'L' ),
		eSECTION_MESH = MAKE_SECTION_NAME( 'M', 'E', 'S', 'H' ),
		eSECTION_SUBMESH = MAKE_SECTION_NAME( 'S', 'M', 'S', 'H' ),
		eSECTION_MATERIAL = MAKE_SECTION_NAME( 'M', 'T', 'R', 'L' ),
		eSECTION_PASS = MAKE_SECTION_NAME( 'P', 'A', 'S', 'S' ),
		eSECTION_TEXTURE_UNIT = MAKE_SECTION_NAME( 'U', 'N', 'I', 'T' ),
		eSECTION_RENDER_TARGET = MAKE_SECTION_NAME( 'R', 'T', 'G', 'R' ),
		eSECTION_GLSL_SHADER = MAKE_SECTION_NAME( 'G', 'L', 'S', 'L' ),
		eSECTION_SHADER_PROGRAM = MAKE_SECTION_NAME( 'S', 'P', 'G', 'M' ),
		eSECTION_SHADER_UBO = MAKE_SECTION_NAME( 'S', 'U', 'B', 'O' ),
		eSECTION_SHADER_UBO_VARIABLE = MAKE_SECTION_NAME( 'S', 'U', 'B', 'V' ),
		eSECTION_BILLBOARD = MAKE_SECTION_NAME( 'B', 'L', 'B', 'd' ),
		eSECTION_BILLBOARD_LIST = MAKE_SECTION_NAME( 'B', 'L', 'B', 'L' ),
		eSECTION_ANIMGROUP = MAKE_SECTION_NAME( 'A', 'N', 'G', 'P' ),
		eSECTION_ANIMATED_OBJECT = MAKE_SECTION_NAME( 'A', 'N', 'O', 'J' ),
		eSECTION_ANIMATION = MAKE_SECTION_NAME( 'A', 'N', 'M', 'T' ),
		eSECTION_SKYBOX = MAKE_SECTION_NAME( 'S', 'K', 'B', 'X' ),
	}	eSECTION;
	//! The context used into parsing functions
	/*!
	While parsing a scene file, the context holds the important data retrieved
	\author Sylvain DOREMUS
	\date 25/08/2010
	*/
	class SceneFileContext
		: public Castor::FileParserContext
	{
	public:
		SceneSPtr pScene;
		RenderWindowSPtr pWindow;
		SceneNodeSPtr pSceneNode;
		GeometrySPtr pGeometry;
		MeshSPtr pMesh;
		SubmeshSPtr pSubmesh;
		LightSPtr pLight;
		CameraSPtr pCamera;
		MaterialSPtr pMaterial;
		SamplerSPtr pSampler;
		RenderTargetSPtr pRenderTarget;
		PassSPtr pPass;
		TextureUnitSPtr pTextureUnit;
		ShaderProgramSPtr pShaderProgram;
		eSHADER_TYPE eShaderObject;
		FrameVariableBufferSPtr pFrameVariableBuffer;
		FrameVariableSPtr pFrameVariable;
		OneIntFrameVariableSPtr pSamplerFrameVariable;
		OverlaySPtr pOverlay;
		BillboardListSPtr pBillboards;
		int iFace1;
		int iFace2;
		eLIGHT_TYPE eLightType;
		eMESH_TYPE eMeshType;
		eTOPOLOGY ePrimitiveType;
		ViewportSPtr pViewport;
		Castor::String strName;
		Castor::String strName2;
		Castor::Path path;
		uint8_t uiUInt8;
		uint16_t uiUInt16;
		uint32_t uiUInt32;
		uint64_t uiUInt64;
		int8_t iInt8;
		int16_t iInt16;
		int32_t iInt32;
		int64_t iInt64;
		bool bBool1;
		bool bBool2;
		SceneNodeSPtr m_pGeneralParentMaterial;
		ScenePtrStrMap mapScenes;
		SceneFileParser * m_pParser;
		RealArray vertexPos;
		RealArray vertexNml;
		RealArray vertexTan;
		RealArray vertexTex;
		UIntArray faces;
		AnimatedObjectGroupSPtr pAnimGroup;
		AnimatedObjectSPtr pAnimObject;
		AnimationSPtr pAnimation;
		SkyboxSPtr pSkybox;

	public:
		/**
		 * Constructor
		 */
		C3D_API SceneFileContext( SceneFileParser * p_pParser, Castor::TextFile * p_pFile );
		/**
		 * Initialises all variables
		 */
		C3D_API void Initialise();
	};
	//! ESCN file parser
	/*!
	Reads ESCN files and extracts all 3D data from it
	\author Sylvain DOREMUS
	\date 25/08/2010
	*/
	class SceneFileParser
		: public Castor::FileParser
		, public Castor::OwnedBy< Engine >
	{
	public:
		/**
		 * Constructor
		 */
		C3D_API SceneFileParser( Engine & p_engine );
		/**
		 * Destructor
		 */
		C3D_API ~SceneFileParser();
		/**
		 * Retrieves the render window defined by the scene
		 */
		C3D_API RenderWindowSPtr GetRenderWindow();
		/**
		 * Parses the given file (expecting it to be in ESCN file format)
		 *\param[in,out]	p_file	The file
		 *\return	the parsed scene
		 */
		C3D_API bool ParseFile( Castor::TextFile & p_file );
		/**
		 * Parses the given file (expecting it to be in ESCN file format)
		 *\param[in]	p_pathFile	The file path
		 *\return	true if successful, false if not
		 */
		C3D_API bool ParseFile( Castor::Path const & p_pathFile );

		inline ScenePtrStrMap::iterator ScenesBegin()
		{
			return m_mapScenes.begin();
		}
		inline ScenePtrStrMap::const_iterator ScenesBegin()const
		{
			return m_mapScenes.begin();
		}
		inline ScenePtrStrMap::const_iterator ScenesEnd()const
		{
			return m_mapScenes.end();
		}

	private:
		C3D_API virtual void DoInitialiseParser( Castor::TextFile & p_file );
		C3D_API virtual void DoCleanupParser();
		C3D_API virtual bool DoDelegateParser( Castor::String const & CU_PARAM_UNUSED( p_line ) )
		{
			return false;
		}
		C3D_API virtual bool DoDiscardParser( Castor::String const & p_line );
		C3D_API virtual void DoValidate();
		C3D_API virtual Castor::String DoGetSectionName( uint32_t p_section );

	private:
		Castor::String m_strSceneFilePath;
		ScenePtrStrMap m_mapScenes;
		RenderWindowSPtr m_renderWindow;

		UIntStrMap m_mapBlendFactors;
		UIntStrMap m_mapTypes;
		UIntStrMap m_mapAlphaFuncs;
		UIntStrMap m_mapTextureArguments;
		UIntStrMap m_mapTextureRgbFunctions;
		UIntStrMap m_mapTextureAlphaFunctions;
		UIntStrMap m_mapTextureChannels;
		UIntStrMap m_mapNormalModes;
		UIntStrMap m_mapLightTypes;
		UIntStrMap m_mapPrimitiveTypes;
		UIntStrMap m_mapPrimitiveOutputTypes;
		UIntStrMap m_mapModels;
		UIntStrMap m_mapViewportModes;
		UIntStrMap m_mapInterpolationModes;
		UIntStrMap m_mapWrappingModes;
		UInt64StrMap m_mapShaderTypes;
		UIntStrMap m_mapVariableTypes;
		UIntStrMap m_mapMovables;
		UIntStrMap m_mapTextWrappingModes;
		UIntStrMap m_mapBorderPositions;
		UIntStrMap m_mapBlendModes;
		UIntStrMap m_mapVerticalAligns;
		UIntStrMap m_mapHorizontalAligns;
		UIntStrMap m_mapInterpolatorModes;
		UIntStrMap m_mapToneMappings;
		UIntStrMap m_mapTextTexturingModes;
		UIntStrMap m_mapLineSpacingModes;
	};
}

#endif
