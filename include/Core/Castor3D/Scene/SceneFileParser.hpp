/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SceneFileParser_H___
#define ___C3D_SceneFileParser_H___

#include "Castor3D/Material/Pass/PassModule.hpp"
#include "Castor3D/Material/Texture/Animation/TextureAnimationModule.hpp"
#include "Castor3D/Model/Mesh/Submesh/SubmeshModule.hpp"
#include "Castor3D/Overlay/OverlayModule.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Scene/Animation/AnimationModule.hpp"
#include "Castor3D/Material/Texture/Animation/TextureAnimation.hpp"
#include "Castor3D/Scene/Background/BackgroundModule.hpp"
#include "Castor3D/Scene/Light/LightModule.hpp"
#include "Castor3D/Scene/ParticleSystem/ParticleModule.hpp"

#include "Castor3D/Buffer/UniformBuffer.hpp"
#include "Castor3D/Material/Texture/TextureConfiguration.hpp"
#include "Castor3D/Material/Pass/SubsurfaceScattering.hpp"
#include "Castor3D/Render/RenderWindow.hpp"
#include "Castor3D/Render/Ssao/SsaoConfig.hpp"

#include <CastorUtils/FileParser/FileParser.hpp>
#include <CastorUtils/FileParser/FileParserContext.hpp>

namespace castor3d
{
	/**
	*\~english
	*\brief
	*	Defines all the sections and subsections of a scene file.
	*\~french
	*\brief
	*	Définit toutes les sections et soussections d'un fichier de scène.
	*/
	enum class CSCNSection
		: uint32_t
	{
		eRoot = CU_MakeSectionName( 'R', 'O', 'O', 'T' ),
		eScene = CU_MakeSectionName( 'S', 'C', 'N', 'E' ),
		eWindow = CU_MakeSectionName( 'W', 'N', 'D', 'W' ),
		eSampler = CU_MakeSectionName( 'S', 'M', 'P', 'R' ),
		eCamera = CU_MakeSectionName( 'C', 'A', 'M', 'R' ),
		eViewport = CU_MakeSectionName( 'V', 'W', 'P', 'T' ),
		eLight = CU_MakeSectionName( 'L', 'G', 'H', 'T' ),
		eNode = CU_MakeSectionName( 'N', 'O', 'D', 'E' ),
		eObject = CU_MakeSectionName( 'O', 'B', 'J', 'T' ),
		eObjectMaterials = CU_MakeSectionName( 'O', 'M', 'T', 'L' ),
		eFont = CU_MakeSectionName( 'F', 'O', 'N', 'T' ),
		ePanelOverlay = CU_MakeSectionName( 'P', 'O', 'V', 'L' ),
		eBorderPanelOverlay = CU_MakeSectionName( 'B', 'O', 'V', 'L' ),
		eTextOverlay = CU_MakeSectionName( 'T', 'O', 'V', 'L' ),
		eMesh = CU_MakeSectionName( 'M', 'E', 'S', 'H' ),
		eSubmesh = CU_MakeSectionName( 'S', 'M', 'S', 'H' ),
		eMaterial = CU_MakeSectionName( 'M', 'T', 'R', 'L' ),
		ePass = CU_MakeSectionName( 'P', 'A', 'S', 'S' ),
		eTextureUnit = CU_MakeSectionName( 'U', 'N', 'I', 'T' ),
		eRenderTarget = CU_MakeSectionName( 'R', 'T', 'G', 'R' ),
		eShaderProgram = CU_MakeSectionName( 'G', 'L', 'S', 'L' ),
		eShaderStage = CU_MakeSectionName( 'S', 'P', 'G', 'M' ),
		eShaderUBO = CU_MakeSectionName( 'S', 'U', 'B', 'O' ),
		eUBOVariable = CU_MakeSectionName( 'S', 'U', 'B', 'V' ),
		eBillboard = CU_MakeSectionName( 'B', 'L', 'B', 'd' ),
		eBillboardList = CU_MakeSectionName( 'B', 'L', 'B', 'L' ),
		eAnimGroup = CU_MakeSectionName( 'A', 'N', 'G', 'P' ),
		eAnimation = CU_MakeSectionName( 'A', 'N', 'M', 'T' ),
		eSkybox = CU_MakeSectionName( 'S', 'K', 'B', 'X' ),
		eParticleSystem = CU_MakeSectionName( 'P', 'L', 'S', 'M' ),
		eParticle = CU_MakeSectionName( 'P', 'T', 'C', 'L' ),
		eSsao = CU_MakeSectionName( 'S', 'S', 'A', 'O' ),
		eSubsurfaceScattering = CU_MakeSectionName( 'S', 'S', 'S', 'G' ),
		eTransmittanceProfile = CU_MakeSectionName( 'T', 'R', 'P', 'R' ),
		eHdrConfig = CU_MakeSectionName( 'H', 'D', 'R', 'C' ),
		eShadows = CU_MakeSectionName( 'S', 'H', 'D', 'W' ),
		eMeshDefaultMaterials = CU_MakeSectionName( 'M', 'M', 'T', 'L' ),
		eRsm = CU_MakeSectionName( 'R', 'F', 'S', 'M' ),
		eLpv = CU_MakeSectionName( 'L', 'P', 'V', 'I' ),
		eRaw = CU_MakeSectionName( 'R', 'A', 'W', 'S' ),
		ePcf = CU_MakeSectionName( 'P', 'C', 'F', 'S' ),
		eVsm = CU_MakeSectionName( 'V', 'S', 'M', 'S' ),
		eTextureAnimation = CU_MakeSectionName( 'T', 'X', 'A', 'N' ),
		eVoxelConeTracing = CU_MakeSectionName( 'V', 'C', 'T', 'C' ),
		eTextureTransform = CU_MakeSectionName( 'T', 'X', 'T', 'R' ),
		eSceneImport = CU_MakeSectionName( 'I', 'M', 'P', 'T' ),
	};

	class SceneFileContext
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	parser	The parser.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	parser	L'analyseur.
		 */
		C3D_API SceneFileContext( castor::LoggerInstance & logger
			, SceneFileParser * parser );
		/**
		 *\~english
		 *\brief		Initialises all variables.
		 *\~french
		 *\brief		Initialise toutes les variables.
		 */
		C3D_API void initialise();

	public:
		struct SceneImportConfig
		{
			castor::Path file;
			std::map< TextureFlag, TextureConfiguration > textureRemaps;
			float rescale{ 1.0f };
			float pitch{ 0.0f };
			float yaw{ 0.0f };
			float roll{ 0.0f };
			std::map< TextureFlag, TextureConfiguration >::iterator textureRemapIt;
		};

		castor::LoggerInstance * logger{};
		SceneSPtr scene{};
		RenderWindowDesc window{};
		bool inWindow{};
		SceneNodeSPtr sceneNode{};
		GeometrySPtr geometry{};
		MeshResPtr mesh{};
		MeshRes ownMesh{};
		SubmeshSPtr submesh{};
		LightSPtr light{};
		CameraSPtr camera{};
		castor::PixelFormat pixelFormat{};
		MaterialRPtr material{};
		MaterialRes ownMaterial{};
		bool createMaterial{ true };
		uint32_t passIndex{};
		SamplerResPtr sampler{};
		SamplerRes ownSampler{};
		TargetType targetType{};
		RenderTargetSPtr renderTarget{};
		RenderTargetSPtr textureRenderTarget{};
		PassSPtr pass{};
		bool createPass{ true };
		uint32_t unitIndex{};
		ashes::ImageCreateInfo imageInfo{ 0u
			, VK_IMAGE_TYPE_2D
			, VK_FORMAT_UNDEFINED
			, { 1u, 1u, 1u }
			, 0u
			, 1u
			, VK_SAMPLE_COUNT_1_BIT
			, VK_IMAGE_TILING_OPTIMAL
			, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT };
		ShaderProgramSPtr shaderProgram{};
		castor::PxBufferBaseSPtr buffer{};
		castor::Path folder{};
		castor::Path relative{};
		VkShaderStageFlagBits shaderStage{};
		UniformBufferBaseUPtr uniformBuffer{};
		OverlayRes overlay{};
		std::vector< OverlayRes > parentOverlays{};
		BillboardListSPtr billboards{};
		int face1{ -1 };
		int face2{ -1 };
		LightType lightType{ LightType::eCount };
		VkPrimitiveTopology primitiveType{ VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST };
		ViewportSPtr viewport{};
		castor::String strName{};
		castor::String strName2{};
		castor::Path path{};
		castor::Size size{};
		castor::Point2f point2f{};
		uint32_t particleCount{};
		int16_t fontHeight{};
		ScenePtrStrMap mapScenes{};
		SceneFileParser * parser{};
		FloatArray vertexPos{};
		FloatArray vertexNml{};
		FloatArray vertexTan{};
		FloatArray vertexTex{};
		UInt32Array faces{};
		AnimatedObjectGroupSPtr animGroup{};
		AnimatedObjectSPtr animSkeleton{};
		AnimatedObjectSPtr animMesh{};
		AnimatedObjectSPtr animMovable{};
		AnimatedObjectSPtr animTexture{};
		TextureAnimationUPtr textureAnimation{};
		SceneBackgroundSPtr background{};
		ParticleSystemSPtr particleSystem{};
		SsaoConfig ssaoConfig{};
		SubsurfaceScatteringUPtr subsurfaceScattering{};
		std::shared_ptr< SkyboxBackground > skybox{};
		TextureConfiguration textureConfiguration{};
		TextureTransform textureTransform{};
		SceneImportConfig sceneImportConfig;
	};

	class SceneFileParser
		: public castor::OwnedBy< Engine >
		, public castor::FileParser
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
		 *\return		The render window defined by the scene.
		 *\~french
		 *\return		La fenêtre de rendu définie par la scène.
		 */
		C3D_API RenderWindowDesc getRenderWindow();
		/**
		 *\~english
		 *\brief		Parses the given file (expecting it to be in CSCN file format).
		 *\param[in]	path	The file access path.
		 *\return		\p false if any problem occured.
		 *\~french
		 *\brief		Analyse le fichier donné (s'attend à recevoir un fichier CSCN).
		 *\param[in]	path	Le chemin d'accès au fichier.
		 *\return		\p false si un problème est survenu.
		 */
		C3D_API bool parseFile( castor::Path const & path );

		ScenePtrStrMap::iterator scenesBegin()
		{
			return m_mapScenes.begin();
		}

		ScenePtrStrMap::const_iterator scenesBegin()const
		{
			return m_mapScenes.begin();
		}

		ScenePtrStrMap::const_iterator scenesEnd()const
		{
			return m_mapScenes.end();
		}

	private:
		C3D_API void doCleanupParser( castor::PreprocessedFile & preprocessed )override;
		C3D_API void doValidate( castor::PreprocessedFile & preprocessed )override;
		C3D_API castor::String doGetSectionName( castor::SectionId section )const override;
		C3D_API std::unique_ptr< FileParser > doCreateParser()const override;

	private:
		castor::String m_strSceneFilePath;
		ScenePtrStrMap m_mapScenes;
		RenderWindowDesc m_renderWindow;

	public:
		C3D_API static UInt32StrMap blendFactors;
		C3D_API static UInt32StrMap types;
		C3D_API static UInt32StrMap comparisonFuncs;
		C3D_API static UInt32StrMap textureArguments;
		C3D_API static UInt32StrMap textureBlendModes;
		C3D_API static UInt32StrMap lightTypes;
		C3D_API static UInt32StrMap primitiveTypes;
		C3D_API static UInt32StrMap primitiveOutputTypes;
		C3D_API static UInt32StrMap models;
		C3D_API static UInt32StrMap viewportModes;
		C3D_API static UInt32StrMap filters;
		C3D_API static UInt32StrMap mipmapModes;
		C3D_API static UInt32StrMap wrappingModes;
		C3D_API static UInt32StrMap borderColours;
		C3D_API static UInt32StrMap shaderTypes;
		C3D_API static UInt32StrMap variableTypes;
		C3D_API static UInt32StrMap elementTypes;
		C3D_API static UInt32StrMap movables;
		C3D_API static UInt32StrMap textWrappingModes;
		C3D_API static UInt32StrMap borderPositions;
		C3D_API static UInt32StrMap verticalAligns;
		C3D_API static UInt32StrMap horizontalAligns;
		C3D_API static UInt32StrMap toneMappings;
		C3D_API static UInt32StrMap textTexturingModes;
		C3D_API static UInt32StrMap lineSpacingModes;
		C3D_API static UInt32StrMap fogTypes;
		C3D_API static UInt32StrMap comparisonModes;
		C3D_API static UInt32StrMap billboardTypes;
		C3D_API static UInt32StrMap billboardSizes;
		C3D_API static UInt32StrMap materialTypes;
		C3D_API static UInt32StrMap shadowFilters;
		C3D_API static UInt32StrMap globalIlluminations;
	};

	C3D_API SceneFileContext & getParserContext( castor::FileParserContext & context );
}

#endif
