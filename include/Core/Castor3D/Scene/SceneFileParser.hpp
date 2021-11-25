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
	};

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
		C3D_API SceneFileContext( castor::Path const & path
			, SceneFileParser * parser );
		/**
		 *\~english
		 *\brief		Initialises all variables.
		 *\~french
		 *\brief		Initialise toutes les variables.
		 */
		C3D_API void initialise();

	public:
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
		C3D_API castor::FileParserContextSPtr doInitialiseParser( castor::Path const & path )override;
		C3D_API void doCleanupParser( castor::PreprocessedFile & preprocessed )override;
		C3D_API void doValidate( castor::PreprocessedFile & preprocessed )override;
		C3D_API castor::String doGetSectionName( castor::SectionId section )const override;
		C3D_API std::unique_ptr< FileParser > doCreateParser()const override;

	private:
		castor::String m_strSceneFilePath;
		ScenePtrStrMap m_mapScenes;
		RenderWindowDesc m_renderWindow;

		UInt32StrMap m_blendFactors;
		UInt32StrMap m_types;
		UInt32StrMap m_comparisonFuncs;
		UInt32StrMap m_textureArguments;
		UInt32StrMap m_textureBlendModes;
		UInt32StrMap m_lightTypes;
		UInt32StrMap m_primitiveTypes;
		UInt32StrMap m_primitiveOutputTypes;
		UInt32StrMap m_models;
		UInt32StrMap m_viewportModes;
		UInt32StrMap m_filters;
		UInt32StrMap m_mipmapModes;
		UInt32StrMap m_wrappingModes;
		UInt32StrMap m_borderColours;
		UInt32StrMap m_shaderTypes;
		UInt32StrMap m_variableTypes;
		UInt32StrMap m_elementTypes;
		UInt32StrMap m_movables;
		UInt32StrMap m_textWrappingModes;
		UInt32StrMap m_borderPositions;
		UInt32StrMap m_verticalAligns;
		UInt32StrMap m_horizontalAligns;
		UInt32StrMap m_toneMappings;
		UInt32StrMap m_textTexturingModes;
		UInt32StrMap m_lineSpacingModes;
		UInt32StrMap m_fogTypes;
		UInt32StrMap m_comparisonModes;
		UInt32StrMap m_billboardTypes;
		UInt32StrMap m_billboardSizes;
		UInt32StrMap m_materialTypes;
		UInt32StrMap m_shadowFilters;
		UInt32StrMap m_globalIlluminations;
	};
}

#endif
