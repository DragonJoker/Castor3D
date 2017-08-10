/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___C3D_SCENE_FILE_PARSER_H___
#define ___C3D_SCENE_FILE_PARSER_H___

#include "Castor3DPrerequisites.hpp"

#include <FileParser/FileParser.hpp>
#include <FileParser/FileParserContext.hpp>

#include "Mesh/Submesh.hpp"
#include "Scene/Skybox.hpp"
#include "Miscellaneous/SsaoConfig.hpp"

#include <Design/OwnedBy.hpp>

namespace castor3d
{
	//! Scene file sections Enum
	/*!
	The enumeration which defines all the sections and subsections of a scene file
	*/
	enum class CSCNSection
		: uint32_t
	{
		eRoot = MAKE_SECTION_NAME( 'R', 'O', 'O', 'T' ),
		eScene = MAKE_SECTION_NAME( 'S', 'C', 'N', 'E' ),
		eWindow = MAKE_SECTION_NAME( 'W', 'N', 'D', 'W' ),
		eSampler = MAKE_SECTION_NAME( 'S', 'M', 'P', 'R' ),
		eCamera = MAKE_SECTION_NAME( 'C', 'A', 'M', 'R' ),
		eViewport = MAKE_SECTION_NAME( 'V', 'W', 'P', 'T' ),
		eLight = MAKE_SECTION_NAME( 'L', 'G', 'H', 'T' ),
		eNode = MAKE_SECTION_NAME( 'N', 'O', 'D', 'E' ),
		eObject = MAKE_SECTION_NAME( 'O', 'B', 'J', 'T' ),
		eObjectMaterials = MAKE_SECTION_NAME( 'O', 'M', 'T', 'L' ),
		eFont = MAKE_SECTION_NAME( 'F', 'O', 'N', 'T' ),
		ePanelOverlay = MAKE_SECTION_NAME( 'P', 'O', 'V', 'L' ),
		eBorderPanelOverlay = MAKE_SECTION_NAME( 'B', 'O', 'V', 'L' ),
		eTextOverlay = MAKE_SECTION_NAME( 'T', 'O', 'V', 'L' ),
		eMesh = MAKE_SECTION_NAME( 'M', 'E', 'S', 'H' ),
		eSubmesh = MAKE_SECTION_NAME( 'S', 'M', 'S', 'H' ),
		eMaterial = MAKE_SECTION_NAME( 'M', 'T', 'R', 'L' ),
		ePass = MAKE_SECTION_NAME( 'P', 'A', 'S', 'S' ),
		eTextureUnit = MAKE_SECTION_NAME( 'U', 'N', 'I', 'T' ),
		eRenderTarget = MAKE_SECTION_NAME( 'R', 'T', 'G', 'R' ),
		eShaderProgram = MAKE_SECTION_NAME( 'G', 'L', 'S', 'L' ),
		eShaderObject = MAKE_SECTION_NAME( 'S', 'P', 'G', 'M' ),
		eShaderUBO = MAKE_SECTION_NAME( 'S', 'U', 'B', 'O' ),
		eUBOVariable = MAKE_SECTION_NAME( 'S', 'U', 'B', 'V' ),
		eBillboard = MAKE_SECTION_NAME( 'B', 'L', 'B', 'd' ),
		eBillboardList = MAKE_SECTION_NAME( 'B', 'L', 'B', 'L' ),
		eAnimGroup = MAKE_SECTION_NAME( 'A', 'N', 'G', 'P' ),
		eAnimation = MAKE_SECTION_NAME( 'A', 'N', 'M', 'T' ),
		eSkybox = MAKE_SECTION_NAME( 'S', 'K', 'B', 'X' ),
		eParticleSystem = MAKE_SECTION_NAME( 'P', 'L', 'S', 'M' ),
		eParticle = MAKE_SECTION_NAME( 'P', 'T', 'C', 'L' ),
		eSsao = MAKE_SECTION_NAME( 'S', 'S', 'A', 'O' ),
	};
	//! The context used into parsing functions
	/*!
	While parsing a scene file, the context holds the important data retrieved
	\author Sylvain DOREMUS
	\date 25/08/2010
	*/
	class SceneFileContext
		: public castor::FileParserContext
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
		LegacyPassSPtr legacyPass;
		MetallicRoughnessPbrPassSPtr pbrMRPass;
		SpecularGlossinessPbrPassSPtr pbrSGPass;
		PassSPtr pass;
		TextureUnitSPtr pTextureUnit;
		ShaderProgramSPtr pShaderProgram;
		ShaderType eShaderObject;
		UniformBufferUPtr pUniformBuffer;
		UniformSPtr pUniform;
		PushUniform1sSPtr pSamplerUniform;
		OverlaySPtr pOverlay;
		BillboardListSPtr pBillboards;
		int iFace1;
		int iFace2;
		LightType eLightType;
		MeshType eMeshType;
		Topology ePrimitiveType;
		ViewportSPtr pViewport;
		castor::String strName;
		castor::String strName2;
		castor::Path path;
		castor::Size size;
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
		AnimatedObjectSPtr pAnimSkeleton;
		AnimatedObjectSPtr pAnimMesh;
		AnimatedObjectSPtr pAnimMovable;
		AnimationInstanceRPtr pAnimation;
		SkyboxUPtr pSkybox;
		ParticleSystemSPtr particleSystem;
		SsaoConfig ssaoConfig;

	public:
		/**
		 * Constructor
		 */
		C3D_API SceneFileContext( SceneFileParser * p_pParser, castor::TextFile * p_pFile );
		/**
		 * Initialises all variables
		 */
		C3D_API void initialise();
	};
	//! ESCN file parser
	/*!
	Reads ESCN files and extracts all 3D data from it
	\author Sylvain DOREMUS
	\date 25/08/2010
	*/
	class SceneFileParser
		: public castor::FileParser
		, public castor::OwnedBy< Engine >
	{
	public:
		/**
		 * Constructor
		 */
		C3D_API explicit SceneFileParser( Engine & engine );
		/**
		 * Destructor
		 */
		C3D_API ~SceneFileParser();
		/**
		 * Retrieves the render window defined by the scene
		 */
		C3D_API RenderWindowSPtr getRenderWindow();
		/**
		 * Parses the given file (expecting it to be in ESCN file format)
		 *\param[in,out]	p_file	The file
		 *\return	the parsed scene
		 */
		C3D_API bool parseFile( castor::TextFile & p_file );
		/**
		 * Parses the given file (expecting it to be in ESCN file format)
		 *\param[in]	p_pathFile	The file path
		 *\return	true if successful, false if not
		 */
		C3D_API bool parseFile( castor::Path const & p_pathFile );
		/**
		 * Parses the given file (expecting it to be in ESCN file format), using an external context
		 *\param[in]	p_pathFile	The file path.
		 *\param[in]	p_context	The context.
		 *\return		true if successful, false if not.
		 */
		C3D_API bool parseFile( castor::Path const & p_pathFile, SceneFileContextSPtr p_context );

		inline ScenePtrStrMap::iterator scenesBegin()
		{
			return m_mapScenes.begin();
		}
		inline ScenePtrStrMap::const_iterator scenesBegin()const
		{
			return m_mapScenes.begin();
		}
		inline ScenePtrStrMap::const_iterator scenesEnd()const
		{
			return m_mapScenes.end();
		}

	private:
		C3D_API virtual void doInitialiseParser( castor::TextFile & p_file );
		C3D_API virtual void doCleanupParser();
		C3D_API virtual bool doDelegateParser( castor::String const & CU_PARAM_UNUSED( p_line ) )
		{
			return false;
		}
		C3D_API virtual bool doDiscardParser( castor::String const & p_line );
		C3D_API virtual void doValidate();
		C3D_API virtual castor::String doGetSectionName( uint32_t p_section );

	private:
		castor::String m_strSceneFilePath;
		ScenePtrStrMap m_mapScenes;
		RenderWindowSPtr m_renderWindow;

		UIntStrMap m_mapBlendFactors;
		UIntStrMap m_mapTypes;
		UIntStrMap m_mapComparisonFuncs;
		UIntStrMap m_mapTextureArguments;
		UIntStrMap m_mapTextureBlendModes;
		UIntStrMap m_mapTextureChannels;
		UIntStrMap m_mapLightTypes;
		UIntStrMap m_mapPrimitiveTypes;
		UIntStrMap m_mapPrimitiveOutputTypes;
		UIntStrMap m_mapModels;
		UIntStrMap m_mapViewportModes;
		UIntStrMap m_mapInterpolationModes;
		UIntStrMap m_mapWrappingModes;
		UIntStrMap m_mapShaderTypes;
		UIntStrMap m_mapVariableTypes;
		UIntStrMap m_mapElementTypes;
		UIntStrMap m_mapMovables;
		UIntStrMap m_mapTextWrappingModes;
		UIntStrMap m_mapBorderPositions;
		UIntStrMap m_mapBlendModes;
		UIntStrMap m_mapVerticalAligns;
		UIntStrMap m_mapHorizontalAligns;
		UIntStrMap m_mapToneMappings;
		UIntStrMap m_mapTextTexturingModes;
		UIntStrMap m_mapLineSpacingModes;
		UIntStrMap m_fogTypes;
		UIntStrMap m_mapComparisonModes;
		UIntStrMap m_mapBillboardTypes;
		UIntStrMap m_mapBillboardSizes;
		UIntStrMap m_mapMaterialTypes;
	};
}

#endif
