/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SCENE_FILE_PARSER_H___
#define ___C3D_SCENE_FILE_PARSER_H___

#include "Castor3DPrerequisites.hpp"

#include <FileParser/FileParser.hpp>
#include <FileParser/FileParserContext.hpp>

#include "Mesh/Submesh.hpp"
#include "Scene/Skybox.hpp"
#include "Technique/Opaque/Ssao/SsaoConfig.hpp"
#include "Material/SubsurfaceScattering.hpp"

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
		shaderStage = MAKE_SECTION_NAME( 'S', 'P', 'G', 'M' ),
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
		eSubsurfaceScattering = MAKE_SECTION_NAME( 'S', 'S', 'S', 'G' ),
		eTransmittanceProfile = MAKE_SECTION_NAME( 'T', 'R', 'P', 'R' ),
		eHdrConfig = MAKE_SECTION_NAME( 'H', 'D', 'R', 'C' ),
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		25/08/2010
	\~english
	\brief		The context used into parsing functions.
	\~french
	\brief		Le contexte utilisé lors de l'analyse des fonctions.
	*/
	class SceneFileContext
		: public castor::FileParserContext
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	path	The file access path.
		 *\param[in]	parser	The parser.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	path	Le chemin d'accès au fichier.
		 *\param[in]	parser	L'analyseur.
		 */
		C3D_API SceneFileContext( castor::Path const & path, SceneFileParser * parser );
		/**
		 *\~english
		 *\brief		Initialises all variables.
		 *\~french
		 *\brief		Initialise toutes les variables.
		 */
		C3D_API void initialise();

	public:
		SceneSPtr scene;
		RenderWindowSPtr window;
		SceneNodeSPtr sceneNode;
		GeometrySPtr geometry;
		MeshSPtr mesh;
		SubmeshSPtr submesh;
		LightSPtr light;
		CameraSPtr camera;
		MaterialSPtr material;
		SamplerSPtr sampler;
		RenderTargetSPtr renderTarget;
		LegacyPassSPtr legacyPass;
		MetallicRoughnessPbrPassSPtr pbrMRPass;
		SpecularGlossinessPbrPassSPtr pbrSGPass;
		PassSPtr pass;
		TextureUnitSPtr textureUnit;
		renderer::ShaderStageStateArray shaderProgram;
		renderer::ShaderStageFlag shaderStage;
		renderer::UniformBufferBasePtr uniformBuffer;
		OverlaySPtr overlay;
		BillboardListSPtr billboards;
		int face1;
		int face2;
		LightType lightType;
		renderer::PrimitiveTopology primitiveType;
		ViewportSPtr viewport;
		castor::String strName;
		castor::String strName2;
		castor::Path path;
		castor::Size size;
		castor::Point2f point2f;
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
		SubsurfaceScatteringUPtr subsurfaceScattering;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		25/08/2010
	\~english
	\brief		CSCN file parser.
	\remarks	Reads CSCN files and extracts all 3D data from it.
	\~french
	\brief		Analyseur de fichiers CSCN.
	\remarks	Lit les fichiers CSCN et en extrait toutes les données 3D.
	*/
	class SceneFileParser
		: public castor::FileParser
		, public castor::OwnedBy< Engine >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 */
		C3D_API explicit SceneFileParser( Engine & engine );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~SceneFileParser();
		/**
		 *\~english
		 *\return		The render window defined by the scene.
		 *\~french
		 *\return		La fenêtre de rendu définie par la scène.
		 */
		C3D_API RenderWindowSPtr getRenderWindow();
		/**
		 *\~english
		 *\brief		Parses the given file (expecting it to be in CSCN file format).
		 *\param[in]	path	The file access path.
		 *\return		\p false if any problem occured.
		 *\~english
		 *\brief		Analyse le fichier donné (s'attend à recevoir un fichier CSCN).
		 *\param[in]	path	Le chemin d'accès au fichier.
		 *\return		\p false si un problème est survenu.
		 */
		C3D_API bool parseFile( castor::Path const & path );
		/**
		 *\~english
		 *\brief		Parses the given file (expecting it to be in CSCN file format), using an external context.
		 *\param[in]	path	The file access path.
		 *\param[in]	context	The context.
		 *\return		\p false if any problem occured.
		 *\~english
		 *\brief		Analyse le fichier donné (s'attend à recevoir un fichier CSCN), en utilisant un contexte externe.
		 *\param[in]	path	Le chemin d'accès au fichier.
		 *\param[in]	context	Le contexte.
		 *\return		\p false si un problème est survenu.
		 */
		C3D_API bool parseFile( castor::Path const & path, SceneFileContextSPtr context );

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
		C3D_API void doInitialiseParser( castor::Path const & path )override;
		C3D_API void doCleanupParser()override;
		C3D_API bool doDelegateParser( castor::String const & CU_PARAM_UNUSED( line ) )override
		{
			return false;
		}
		C3D_API bool doDiscardParser( castor::String const & line )override;
		C3D_API void doValidate()override;
		C3D_API castor::String doGetSectionName( uint32_t section )override;

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
		UIntStrMap m_mapBorderColours;
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
