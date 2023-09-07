/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SceneFileParser_H___
#define ___C3D_SceneFileParser_H___

#include "Castor3D/Material/Pass/PassModule.hpp"
#include "Castor3D/Material/Pass/Component/ComponentModule.hpp"
#include "Castor3D/Material/Texture/Animation/TextureAnimationModule.hpp"
#include "Castor3D/Model/Mesh/Submesh/SubmeshModule.hpp"
#include "Castor3D/Model/Skeleton/SkeletonModule.hpp"
#include "Castor3D/Overlay/OverlayModule.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Render/GlobalIllumination/GlobalIlluminationModule.hpp"
#include "Castor3D/Scene/Animation/AnimationModule.hpp"
#include "Castor3D/Material/Texture/Animation/TextureAnimation.hpp"
#include "Castor3D/Scene/Background/BackgroundModule.hpp"
#include "Castor3D/Scene/Light/LightModule.hpp"
#include "Castor3D/Scene/ParticleSystem/ParticleModule.hpp"

#include "Castor3D/Buffer/UniformBuffer.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureConfiguration.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Pass/SubsurfaceScattering.hpp"
#include "Castor3D/Model/Mesh/Mesh.hpp"
#include "Castor3D/Model/Mesh/MeshImporter.hpp"
#include "Castor3D/Model/Mesh/Animation/MeshAnimation.hpp"
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
		eSkeleton = CU_MakeSectionName( 'S', 'K', 'E', 'L' ),
		eMorphAnimation = CU_MakeSectionName( 'M', 'T', 'A', 'N' ),
		eTextureRemapChannel = CU_MakeSectionName( 'T', 'X', 'R', 'C' ),
		eTextureRemap = CU_MakeSectionName( 'T', 'X', 'R', 'P' ),
	};

	class SceneFileContext
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	logger	The logger instance.
		 *\param[in]	parser	The parser.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	logger	L'instance du logger.
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
			castor::PathArray files;
			castor::PathArray animFiles;
			castor::String prefix;
			std::map< PassComponentTextureFlag, TextureConfiguration > textureRemaps;
			float rescale{ 1.0f };
			float pitch{ 0.0f };
			float yaw{ 0.0f };
			float roll{ 0.0f };
			bool noOptimisations{ false };
			float emissiveMult{ 1.0f };
			std::map< PassComponentTextureFlag, TextureConfiguration >::iterator textureRemapIt;
			castor::String centerCamera;
			castor::String preferredImporter{ cuT( "any" ) };
		};

		struct SceneNodeConfig
		{
			bool isCameraNode{};
			bool isStatic{};
			castor::String name;
			SceneNode * parent{};
			castor::Point3f position{};
			castor::Quaternion orientation{ castor::Quaternion::identity() };
			castor::Point3f scale{ 1.0f, 1.0f, 1.0f };
		};

		castor::LoggerInstance * logger{};
		castor::PathArray files;
		castor::PathArray csnaFiles;
		SceneRPtr scene{};
		SceneUPtr ownScene{};
		RenderWindowDesc window{};
		bool inWindow{};
		SceneNodeConfig nodeConfig{};
		SceneNodeRPtr parentNode{};
		GeometryUPtr ownGeometry{};
		GeometryRPtr geometry{};
		SkeletonRPtr skeleton{};
		MeshResPtr mesh{};
		MeshRes ownMesh{};
		SubmeshRPtr submesh{};
		LightUPtr ownLight{};
		LightRPtr light{};
		castor::PixelFormat pixelFormat{};
		MaterialObs material{};
		MaterialPtr ownMaterial{};
		bool createMaterial{ true };
		bool enableFullLoading{ false };
		uint32_t passIndex{};
		SamplerObs sampler{};
		SamplerPtr ownSampler{};
		TargetType targetType{};
		RenderTargetRPtr renderTarget{};
		RenderTargetRPtr textureRenderTarget{};
		PassRPtr pass{};
		PassComponent * passComponent{};
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
		ShaderProgramRPtr shaderProgram{};
		castor::PxBufferBaseUPtr buffer{};
		castor::Path folder{};
		castor::Path relative{};
		castor::ImageRPtr image{};
		VkShaderStageFlagBits shaderStage{};
		UniformBufferBaseUPtr uniformBuffer{};
		struct OverlayPtr
		{
			OverlayUPtr uptr{};
			OverlayRPtr rptr{};
		};
		OverlayPtr overlay;
		std::vector< OverlayPtr > parentOverlays{};
		BillboardListUPtr ownBillboards{};
		BillboardListRPtr billboards{};
		int face1{ -1 };
		int face2{ -1 };
		LightType lightType{ LightType::eCount };
		VkPrimitiveTopology primitiveType{ VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST };
		ViewportUPtr viewport{};
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
		AnimatedObjectGroupRPtr animGroup{};
		AnimatedObjectRPtr animSkeleton{};
		AnimatedObjectRPtr animMesh{};
		AnimatedObjectRPtr animNode{};
		AnimatedObjectRPtr animTexture{};
		TextureAnimationUPtr textureAnimation{};
		MeshAnimationUPtr morphAnimation{};
		SceneBackgroundUPtr background{};
		ParticleSystemUPtr ownParticleSystem{};
		ParticleSystemRPtr particleSystem{};
		SsaoConfig ssaoConfig{};
		SubsurfaceScatteringUPtr subsurfaceScattering{};
		SkyboxBackgroundUPtr skybox{};
		TextureConfiguration textureConfiguration{};
		TextureTransform textureTransform{};
		uint32_t texcoordSet{};
		SceneImportConfig sceneImportConfig;
		MeshImporterUPtr importer;
	};

	C3D_API SceneFileContext & getSceneParserContext( castor::FileParserContext & context );

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
		C3D_API static UInt32StrMap comparisonModes;
	};

	C3D_API SceneFileContext & getParserContext( castor::FileParserContext & context );

	template< typename ComponentT >
	ComponentT & getPassComponent( SceneFileContext & parsingContext )
	{
		if ( !parsingContext.passComponent
			|| parsingContext.passComponent->getOwner() != parsingContext.pass
			|| getPassComponentType( *parsingContext.passComponent ) != ComponentT::TypeName )
		{
			if ( parsingContext.pass->template hasComponent< ComponentT >() )
			{
				parsingContext.passComponent = parsingContext.pass->template getComponent< ComponentT >();
			}
			else
			{
				parsingContext.passComponent = parsingContext.pass->template createComponent< ComponentT >();
			}
		}

		return static_cast< ComponentT & >( *parsingContext.passComponent );
	}

	template< typename Type >
	struct LimitedType
	{
	};
}

namespace castor
{
	template<>
	struct ParserEnumTraits< LengthUnit >
	{
		static inline xchar const * const Name = cuT( "LengthUnit" );
		static inline UInt32StrMap const Values = []()
		{
			UInt32StrMap result;
			result = castor3d::getEnumMapT< LengthUnit >();
			return result;
		}( );
	};

	template<>
	struct ParserEnumTraits< VkShaderStageFlagBits >
	{
		static inline xchar const * const Name = cuT( "ShaderStage" );
		static inline UInt32StrMap const Values = []()
		{
			UInt32StrMap result;
			result[ashes::getName( VK_SHADER_STAGE_VERTEX_BIT )] = uint32_t( VK_SHADER_STAGE_VERTEX_BIT );
			result[ashes::getName( VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT )] = uint32_t( VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT );
			result[ashes::getName( VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT )] = uint32_t( VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT );
			result[ashes::getName( VK_SHADER_STAGE_GEOMETRY_BIT )] = uint32_t( VK_SHADER_STAGE_GEOMETRY_BIT );
			result[ashes::getName( VK_SHADER_STAGE_FRAGMENT_BIT )] = uint32_t( VK_SHADER_STAGE_FRAGMENT_BIT );
			result[ashes::getName( VK_SHADER_STAGE_COMPUTE_BIT )] = uint32_t( VK_SHADER_STAGE_COMPUTE_BIT );
			return result;
		}( );
	};

	template<>
	struct ParserEnumTraits< VkSamplerMipmapMode >
	{
		static inline xchar const * const Name = cuT( "MipmapMode" );
		static inline UInt32StrMap const Values = []()
		{
			UInt32StrMap result;
			result = castor3d::getEnumMapT( VK_SAMPLER_MIPMAP_MODE_NEAREST, VK_SAMPLER_MIPMAP_MODE_LINEAR );
			result[cuT( "none" )] = uint32_t( VK_SAMPLER_MIPMAP_MODE_NEAREST );
			return result;
		}( );
	};

	template<>
	struct ParserEnumTraits< VkBlendFactor >
	{
		static inline xchar const * const Name = cuT( "BlendFactor" );
		static inline UInt32StrMap const Values = []()
		{
			UInt32StrMap result;
			result = castor3d::getEnumMapT( VK_BLEND_FACTOR_ZERO, VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA );
			return result;
		}( );
	};

	template<>
	struct ParserEnumTraits< VkImageType >
	{
		static inline xchar const * const Name = cuT( "ImageType" );
		static inline UInt32StrMap const Values = []()
		{
			UInt32StrMap result;
			result = castor3d::getEnumMapT( VK_IMAGE_TYPE_1D, VK_IMAGE_TYPE_3D );
			return result;
		}( );
	};

	template<>
	struct ParserEnumTraits< VkBlendOp >
	{
		static inline xchar const * const Name = cuT( "BlendOp" );
		static inline UInt32StrMap const Values = []()
		{
			UInt32StrMap result;
			result = castor3d::getEnumMapT( VK_BLEND_OP_ADD, VK_BLEND_OP_MAX );
			return result;
		}( );
	};

	template<>
	struct ParserEnumTraits< VkFilter >
	{
		static inline xchar const * const Name = cuT( "Filter" );
		static inline UInt32StrMap const Values = []()
		{
			UInt32StrMap result;
			result = castor3d::getEnumMapT( VK_FILTER_NEAREST, VK_FILTER_LINEAR );
			return result;
		}( );
	};

	template<>
	struct ParserEnumTraits< VkSamplerAddressMode >
	{
		static inline xchar const * const Name = cuT( "AddressMode" );
		static inline UInt32StrMap const Values = []()
		{
			UInt32StrMap result;
			result = castor3d::getEnumMapT( VK_SAMPLER_ADDRESS_MODE_REPEAT, VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE );
			return result;
		}( );
	};

	template<>
	struct ParserEnumTraits< VkBorderColor >
	{
		static inline xchar const * const Name = cuT( "BorderColor" );
		static inline UInt32StrMap const Values = []()
		{
			UInt32StrMap result;
			result = castor3d::getEnumMapT( VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK, VK_BORDER_COLOR_INT_OPAQUE_WHITE );
			return result;
		}( );
	};

	template<>
	struct ParserEnumTraits< VkCompareOp >
	{
		static inline xchar const * const Name = cuT( "CompareFunc" );
		static inline UInt32StrMap const Values = []()
		{
			UInt32StrMap result;
			result = castor3d::getEnumMapT( VK_COMPARE_OP_NEVER, VK_COMPARE_OP_ALWAYS );
			return result;
		}( );
	};

	template<>
	struct ParserEnumTraits< VkPrimitiveTopology >
	{
		static inline xchar const * const Name = cuT( "PrimitiveTopology" );
		static inline UInt32StrMap const Values = []()
		{
			UInt32StrMap result;
			result = castor3d::getEnumMapT( VK_PRIMITIVE_TOPOLOGY_POINT_LIST, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY );
			return result;
		}( );
	};

	template<>
	struct ParserEnumTraits< castor3d::LimitedType< VkCompareOp > >
	{
		static inline xchar const * const Name = cuT( "CompareOp" );
		static inline UInt32StrMap const Values = []()
		{
			UInt32StrMap result;
			result[cuT( "none" )] = uint32_t( false );
			result[cuT( "ref_to_texture" )] = uint32_t( true );
			return result;
		}( );
	};

	template<>
	struct ParserEnumTraits< castor3d::LimitedType< VkPrimitiveTopology > >
	{
		static inline xchar const * const Name = cuT( "PrimitiveType" );
		static inline UInt32StrMap const Values = []()
		{
			UInt32StrMap result;
			result[ashes::getName( VK_PRIMITIVE_TOPOLOGY_POINT_LIST )] = uint32_t( VK_PRIMITIVE_TOPOLOGY_POINT_LIST );
			result[ashes::getName( VK_PRIMITIVE_TOPOLOGY_LINE_STRIP )] = uint32_t( VK_PRIMITIVE_TOPOLOGY_LINE_STRIP );
			result[ashes::getName( VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP )] = uint32_t( VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP );
			return result;
		}( );
	};

	template<>
	struct ParserEnumTraits< castor3d::BlendMode >
	{
		static inline xchar const * const Name = cuT( "BlendMode" );
		static inline UInt32StrMap const Values = []()
		{
			UInt32StrMap result;
			result = castor3d::getEnumMapT< castor3d::BlendMode >();
			return result;
		}( );
	};

	template<>
	struct ParserEnumTraits< castor3d::ParallaxOcclusionMode >
	{
		static inline xchar const * const Name = cuT( "ParallaxOcclusionMode" );
		static inline UInt32StrMap const Values = []()
		{
			UInt32StrMap result;
			result = castor3d::getEnumMapT< castor3d::ParallaxOcclusionMode >();
			return result;
		}( );
	};

	template<>
	struct ParserEnumTraits< castor3d::LightType >
	{
		static inline xchar const * const Name = cuT( "LightType" );
		static inline UInt32StrMap const Values = []()
		{
			UInt32StrMap result;
			result = castor3d::getEnumMapT< castor3d::LightType >();
			return result;
		}( );
	};

	template<>
	struct ParserEnumTraits< castor3d::ViewportType >
	{
		static inline xchar const * const Name = cuT( "ViewportType" );
		static inline UInt32StrMap const Values = []()
		{
			UInt32StrMap result;
			result = castor3d::getEnumMapT< castor3d::ViewportType >();
			return result;
		}( );
	};

	template<>
	struct ParserEnumTraits< castor3d::ParticleFormat >
	{
		static inline xchar const * const Name = cuT( "ParticleFormat" );
		static inline UInt32StrMap const Values = []()
		{
			UInt32StrMap result;
			result = castor3d::getEnumMapT< castor3d::ParticleFormat >();
			return result;
		}( );
	};

	template<>
	struct ParserEnumTraits< castor3d::MovableType >
	{
		static inline xchar const * const Name = cuT( "MovableType" );
		static inline UInt32StrMap const Values = []()
		{
			UInt32StrMap result;
			result = castor3d::getEnumMapT< castor3d::MovableType >();
			return result;
		}( );
	};

	template<>
	struct ParserEnumTraits< castor3d::FogType >
	{
		static inline xchar const * const Name = cuT( "FogType" );
		static inline UInt32StrMap const Values = []()
		{
			UInt32StrMap result;
			result = castor3d::getEnumMapT< castor3d::FogType >();
			return result;
		}( );
	};

	template<>
	struct ParserEnumTraits< castor3d::BillboardType >
	{
		static inline xchar const * const Name = cuT( "BillboardType" );
		static inline UInt32StrMap const Values = []()
		{
			UInt32StrMap result;
			result = castor3d::getEnumMapT< castor3d::BillboardType >();
			return result;
		}( );
	};

	template<>
	struct ParserEnumTraits< castor3d::BillboardSize >
	{
		static inline xchar const * const Name = cuT( "BillboardSize" );
		static inline UInt32StrMap const Values = []()
		{
			UInt32StrMap result;
			result = castor3d::getEnumMapT< castor3d::BillboardSize >();
			return result;
		}( );
	};

	template<>
	struct ParserEnumTraits< castor3d::TextWrappingMode >
	{
		static inline xchar const * const Name = cuT( "TextWrappingMode" );
		static inline UInt32StrMap const Values = []()
		{
			UInt32StrMap result;
			result = castor3d::getEnumMapT< castor3d::TextWrappingMode >();
			return result;
		}( );
	};

	template<>
	struct ParserEnumTraits< castor3d::BorderPosition >
	{
		static inline xchar const * const Name = cuT( "BorderPosition" );
		static inline UInt32StrMap const Values = []()
		{
			UInt32StrMap result;
			result = castor3d::getEnumMapT< castor3d::BorderPosition >();
			return result;
		}( );
	};

	template<>
	struct ParserEnumTraits< castor3d::VAlign >
	{
		static inline xchar const * const Name = cuT( "VAlign" );
		static inline UInt32StrMap const Values = []()
		{
			UInt32StrMap result;
			result = castor3d::getEnumMapT< castor3d::VAlign >();
			return result;
		}( );
	};

	template<>
	struct ParserEnumTraits< castor3d::HAlign >
	{
		static inline xchar const * const Name = cuT( "HAlign" );
		static inline UInt32StrMap const Values = []()
		{
			UInt32StrMap result;
			result = castor3d::getEnumMapT< castor3d::HAlign >();
			return result;
		}( );
	};

	template<>
	struct ParserEnumTraits< castor3d::TextTexturingMode >
	{
		static inline xchar const * const Name = cuT( "TextTexturingMode" );
		static inline UInt32StrMap const Values = []()
		{
			UInt32StrMap result;
			result = castor3d::getEnumMapT< castor3d::TextTexturingMode >();
			return result;
		}( );
	};

	template<>
	struct ParserEnumTraits< castor3d::TextLineSpacingMode >
	{
		static inline xchar const * const Name = cuT( "TextLineSpacingMode" );
		static inline UInt32StrMap const Values = []()
		{
			UInt32StrMap result;
			result = castor3d::getEnumMapT< castor3d::TextLineSpacingMode >();
			return result;
		}( );
	};

	template<>
	struct ParserEnumTraits< castor3d::ShadowType >
	{
		static inline xchar const * const Name = cuT( "ShadowType" );
		static inline UInt32StrMap const Values = []()
		{
			UInt32StrMap result;
			result = castor3d::getEnumMapT< castor3d::ShadowType >();
			return result;
		}( );
	};

	template<>
	struct ParserEnumTraits< castor3d::GlobalIlluminationType >
	{
		static inline xchar const * const Name = cuT( "GlobalIlluminationType" );
		static inline UInt32StrMap const Values = []()
		{
			UInt32StrMap result;
			result = castor3d::getEnumMapT< castor3d::GlobalIlluminationType >();
			return result;
		}( );
	};

	template<>
	struct ParserEnumTraits< castor3d::InterpolatorType >
	{
		static inline xchar const * const Name = cuT( "InterpolatorType" );
		static inline UInt32StrMap const Values = []()
		{
			UInt32StrMap result;
			result = castor3d::getEnumMapT< castor3d::InterpolatorType >();
			return result;
		}( );
	};
}

#endif
