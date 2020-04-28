/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SceneFileParser_H___
#define ___C3D_SceneFileParser_H___

#include "Castor3D/Material/Pass/PassModule.hpp"
#include "Castor3D/Model/Mesh/Submesh/SubmeshModule.hpp"
#include "Castor3D/Overlay/OverlayModule.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Scene/Animation/AnimationModule.hpp"
#include "Castor3D/Scene/Background/BackgroundModule.hpp"
#include "Castor3D/Scene/Light/LightModule.hpp"
#include "Castor3D/Scene/ParticleSystem/ParticleModule.hpp"

#include "Castor3D/Buffer/UniformBuffer.hpp"
#include "Castor3D/Material/Texture/TextureConfiguration.hpp"
#include "Castor3D/Material/Pass/SubsurfaceScattering.hpp"
#include "Castor3D/Render/Technique/Opaque/Ssao/SsaoConfig.hpp"
#include "Castor3D/Render/Technique/Opaque/Ssgi/SsgiConfig.hpp"

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
		shaderStage = CU_MakeSectionName( 'S', 'P', 'G', 'M' ),
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
		eSsgi = CU_MakeSectionName( 'S', 'S', 'G', 'I' ),
		eSubsurfaceScattering = CU_MakeSectionName( 'S', 'S', 'S', 'G' ),
		eTransmittanceProfile = CU_MakeSectionName( 'T', 'R', 'P', 'R' ),
		eHdrConfig = CU_MakeSectionName( 'H', 'D', 'R', 'C' ),
		eShadows = CU_MakeSectionName( 'S', 'H', 'D', 'W' ),
		eMeshDefaultMaterials = CU_MakeSectionName( 'M', 'M', 'T', 'L' ),
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
		SceneSPtr scene;
		RenderWindowSPtr window;
		SceneNodeSPtr sceneNode;
		GeometrySPtr geometry;
		MeshSPtr mesh;
		SubmeshSPtr submesh;
		LightSPtr light;
		CameraSPtr camera;
		MaterialSPtr material;
		bool createMaterial{};
		uint32_t passIndex{};
		SamplerSPtr sampler;
		RenderTargetSPtr renderTarget;
		PhongPassSPtr phongPass;
		MetallicRoughnessPbrPassSPtr pbrMRPass;
		SpecularGlossinessPbrPassSPtr pbrSGPass;
		PassSPtr pass;
		bool createPass{};
		uint32_t unitIndex{};
		ashes::ImageCreateInfo imageInfo;
		TextureUnitSPtr textureUnit;
		bool createUnit{};
		ShaderProgramSPtr shaderProgram;
		castor::PxBufferBaseSPtr buffer;
		castor::Path folder;
		castor::Path relative;
		VkShaderStageFlagBits shaderStage{};
		UniformBufferBaseUPtr uniformBuffer;
		OverlaySPtr overlay;
		BillboardListSPtr billboards;
		int face1{};
		int face2{};
		LightType lightType{};
		VkPrimitiveTopology primitiveType{};
		ViewportSPtr viewport;
		castor::String strName;
		castor::String strName2;
		castor::Path path;
		castor::Size size;
		castor::Point2f point2f;
		uint8_t uiUInt8{};
		uint16_t uiUInt16{};
		uint32_t uiUInt32{};
		uint64_t uiUInt64{};
		int8_t iInt8{};
		int16_t iInt16{};
		int32_t iInt32{};
		int64_t iInt64{};
		bool bBool1{};
		bool bBool2{};
		SceneNodeSPtr m_pGeneralParentMaterial;
		ScenePtrStrMap mapScenes;
		SceneFileParser * m_pParser{};
		FloatArray vertexPos;
		FloatArray vertexNml;
		FloatArray vertexTan;
		FloatArray vertexTex;
		UInt32Array faces;
		AnimatedObjectGroupSPtr pAnimGroup;
		AnimatedObjectSPtr pAnimSkeleton;
		AnimatedObjectSPtr pAnimMesh;
		AnimatedObjectSPtr pAnimMovable;
		AnimationInstanceRPtr pAnimation{};
		SceneBackgroundSPtr pBackground;
		ParticleSystemSPtr particleSystem;
		SsaoConfig ssaoConfig{};
		SsgiConfig ssgiConfig{};
		SubsurfaceScatteringUPtr subsurfaceScattering;
		std::shared_ptr< SkyboxBackground > skybox;
		TextureConfiguration textureConfiguration;
	};

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
		 *\~french
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
		 *\~french
		 *\brief		Analyse le fichier donné (s'attend à recevoir un fichier CSCN), en utilisant un contexte externe.
		 *\param[in]	path	Le chemin d'accès au fichier.
		 *\param[in]	context	Le contexte.
		 *\return		\p false si un problème est survenu.
		 */
		C3D_API bool parseFile( castor::Path const & path
			, SceneFileContextSPtr context );

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
		C3D_API bool doDelegateParser( castor::String const & CU_UnusedParam( line ) )override
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

		UInt32StrMap m_mapBlendFactors;
		UInt32StrMap m_mapTypes;
		UInt32StrMap m_mapComparisonFuncs;
		UInt32StrMap m_mapTextureArguments;
		UInt32StrMap m_mapTextureBlendModes;
		UInt32StrMap m_mapTextureChannels;
		UInt32StrMap m_mapLightTypes;
		UInt32StrMap m_mapPrimitiveTypes;
		UInt32StrMap m_mapPrimitiveOutputTypes;
		UInt32StrMap m_mapModels;
		UInt32StrMap m_mapViewportModes;
		UInt32StrMap m_mapFilters;
		UInt32StrMap m_mapMipmapModes;
		UInt32StrMap m_mapWrappingModes;
		UInt32StrMap m_mapBorderColours;
		UInt32StrMap m_mapShaderTypes;
		UInt32StrMap m_mapVariableTypes;
		UInt32StrMap m_mapElementTypes;
		UInt32StrMap m_mapMovables;
		UInt32StrMap m_mapTextWrappingModes;
		UInt32StrMap m_mapBorderPositions;
		UInt32StrMap m_mapBlendModes;
		UInt32StrMap m_mapVerticalAligns;
		UInt32StrMap m_mapHorizontalAligns;
		UInt32StrMap m_mapToneMappings;
		UInt32StrMap m_mapTextTexturingModes;
		UInt32StrMap m_mapLineSpacingModes;
		UInt32StrMap m_fogTypes;
		UInt32StrMap m_mapComparisonModes;
		UInt32StrMap m_mapBillboardTypes;
		UInt32StrMap m_mapBillboardSizes;
		UInt32StrMap m_mapMaterialTypes;
		UInt32StrMap m_mapShadowFilters;
	};
}

#endif
