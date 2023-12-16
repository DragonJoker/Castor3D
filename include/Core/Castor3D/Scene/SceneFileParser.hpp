/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SceneFileParser_H___
#define ___C3D_SceneFileParser_H___

#include "Castor3D/Material/Pass/PassModule.hpp"
#include "Castor3D/Material/Pass/Component/ComponentModule.hpp"
#include "Castor3D/Material/Texture/Animation/TextureAnimationModule.hpp"
#include "Castor3D/Model/Mesh/Submesh/SubmeshModule.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/ComponentModule.hpp"
#include "Castor3D/Model/Skeleton/SkeletonModule.hpp"
#include "Castor3D/Overlay/OverlayModule.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Render/Clustered/ClusteredModule.hpp"
#include "Castor3D/Render/GlobalIllumination/GlobalIlluminationModule.hpp"
#include "Castor3D/Render/ToneMapping/ColourGradingConfig.hpp"
#include "Castor3D/Render/ToneMapping/HdrConfig.hpp"
#include "Castor3D/Scene/Animation/AnimationModule.hpp"
#include "Castor3D/Scene/Background/BackgroundModule.hpp"
#include "Castor3D/Scene/Light/LightModule.hpp"
#include "Castor3D/Scene/ParticleSystem/ParticleModule.hpp"

#include "Castor3D/Buffer/UniformBuffer.hpp"
#include "Castor3D/Gui/Layout/LayoutItemFlags.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureConfiguration.hpp"
#include "Castor3D/Material/Texture/Animation/TextureAnimation.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Pass/SubsurfaceScattering.hpp"
#include "Castor3D/Model/Mesh/Mesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/SubmeshComponent.hpp"
#include "Castor3D/Model/Mesh/MeshImporter.hpp"
#include "Castor3D/Model/Mesh/Animation/MeshAnimation.hpp"
#include "Castor3D/Render/RenderWindow.hpp"
#include "Castor3D/Render/Ssao/SsaoConfig.hpp"

#include <CastorUtils/FileParser/FileParser.hpp>
#include <CastorUtils/FileParser/FileParserContext.hpp>

#include <stack>

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
		eClusters = CU_MakeSectionName( 'C', 'L', 'S', 'T' ),
		eTexture = CU_MakeSectionName( 'T', 'X', 'U', 'R' ),
		eColourGrading = CU_MakeSectionName( 'C', 'L', 'G', 'D' ),
	};

	struct RootContext;

	struct OverlayContext
	{
		struct OverlayPtr
		{
			OverlayUPtr uptr{};
			OverlayRPtr rptr{};
		};
		SceneRPtr scene{};
		OverlayPtr overlay;
		std::vector< OverlayPtr > parentOverlays{};
	};

	struct MaterialContext
	{
		SceneRPtr scene{};
		castor::String name{};
		MaterialObs material{};
		MaterialPtr ownMaterial{};
		uint32_t passIndex{};
		bool createMaterial{ true };
		RootContext * root{};
	};

	struct PassContext
	{
		SceneRPtr scene{};
		PassRPtr pass{};
		PassComponent * passComponent{};
		bool createPass{ true };
		uint32_t unitIndex{};
		MaterialContext * material{};
		RootContext * root{};
	};

	struct TextureContext
	{
		SceneRPtr scene{};
		castor::String name{};
		castor::Path folder{};
		castor::Path relative{};
		castor::ImageRPtr image{};
		uint32_t mipLevels{ ashes::RemainingArrayLayers };
		uint32_t texcoordSet{};
		TextureConfiguration configuration{};
		TextureAnimationUPtr textureAnimation{};
		SamplerObs sampler{};
		TextureTransform textureTransform{};
		RenderTargetRPtr renderTarget{};
		PassContext * pass{};
		RootContext * root{};
	};

	struct WindowContext
	{
		RenderWindowDesc window{};
		RootContext * root{};
	};

	struct TargetContext
	{
		RenderWindowDesc * window{};
		TextureContext * texture{};
		TargetType targetType{};
		SsaoConfig ssaoConfig{};
		castor::Size size{};
		castor::PixelFormat hdrPixelFormat{};
		castor::PixelFormat srgbPixelFormat{};
		RenderTargetRPtr renderTarget{};
		RootContext * root{};
	};

	struct SamplerContext
	{
		SamplerObs sampler{};
		SamplerPtr ownSampler{};
	};

	struct SceneContext
	{
		SceneRPtr scene{};
		SceneUPtr ownScene{};
		castor::String fontName{};
		OverlayContext overlays{};
		RootContext * root{};
	};

	struct MovableContext
	{
		castor::String name{};
		SceneRPtr scene{};
		SceneNodeRPtr parentNode{};
	};

	struct SceneImportContext
	{
		SceneRPtr scene{};
		castor::PathArray files{};
		castor::PathArray animFiles{};
		castor::String prefix{};
		std::map< PassComponentTextureFlag, TextureConfiguration > textureRemaps;
		float rescale{ 1.0f };
		float pitch{ 0.0f };
		float yaw{ 0.0f };
		float roll{ 0.0f };
		bool noOptimisations{ false };
		float emissiveMult{ 1.0f };
		std::map< PassComponentTextureFlag, TextureConfiguration >::iterator textureRemapIt;
		castor::String centerCamera{};
		castor::String preferredImporter{ cuT( "any" ) };
		RootContext * root{};
	};

	struct CameraContext
		: public MovableContext
	{
		ViewportUPtr viewport{};
		VkPrimitiveTopology primitiveType{ VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST };
		HdrConfig hdrConfig{};
		ColourGradingConfig colourGradingConfig{};
	};

	struct LightContext
		: public MovableContext
	{
		LightUPtr ownLight{};
		LightRPtr light{};
		LightType lightType{ LightType::eCount };
		ShadowConfigUPtr shadowConfig;
	};

	struct NodeContext
		: public MovableContext
	{
		bool isCameraNode{};
		bool isStatic{};
		castor::Point3f position{};
		castor::Quaternion orientation{ castor::Quaternion::identity() };
		castor::Point3f scale{ 1.0f, 1.0f, 1.0f };
		RootContext * root{};
	};

	struct ObjectContext
		: public MovableContext
	{
		GeometryUPtr ownGeometry{};
		GeometryRPtr geometry{};
		RootContext * root{};
	};

	struct BillboardsContext
		: public MovableContext
	{
		BillboardListUPtr ownBillboards{};
		BillboardListRPtr billboards{};
	};

	struct AnimGroupContext
	{
		SceneRPtr scene{};
		AnimatedObjectGroupRPtr animGroup{};
		AnimatedObjectRPtr animSkeleton{};
		AnimatedObjectRPtr animMesh{};
		AnimatedObjectRPtr animNode{};
		AnimatedObjectRPtr animTexture{};
		castor::String animName{};
	};

	struct SkyboxContext
	{
		SkyboxBackgroundUPtr skybox{};
	};

	struct ParticleSystemContext
		: public MovableContext
	{
		MaterialObs material{};
		castor::Point2f dimensions{};
		uint32_t particleCount{};
		ParticleSystemUPtr ownParticleSystem{};
		ParticleSystemRPtr particleSystem{};
	};

	struct MeshContext
	{
		MeshResPtr mesh{};
		MeshRes ownMesh{};
		MeshAnimationUPtr morphAnimation{};
		SubmeshRPtr submesh{};
		SubmeshComponent * submeshComponent{};
		GeometryRPtr geometry{};
		SceneRPtr scene{};
		RootContext * root{};
	};

	struct SkeletonContext
	{
		SceneRPtr scene{};
		SkeletonRPtr skeleton{};
	};

	struct SubmeshContext
	{
		SubmeshRPtr submesh{};
		int face1{ -1 };
		int face2{ -1 };
		FloatArray vertexPos{};
		FloatArray vertexNml{};
		FloatArray vertexTan{};
		FloatArray vertexTex{};
		UInt32Array faces{};
		RootContext * root{};
	};

	struct ProgramContext
	{
		castor::String name{};
		ShaderProgramRPtr shaderProgram{};
		VkShaderStageFlagBits shaderStage{};
		ParticleSystemContext * particleSystem{};
	};

	struct FontContext
	{
		SceneRPtr scene{};
		castor::String name{};
		castor::Path path{};
		int16_t fontHeight{};
		RootContext * root{};
	};

	struct GuiContext
	{
		SceneRPtr scene{};
		ControlsManager * controls{};
		std::stack< ControlRPtr > parents{};
		std::stack< ControlStyleRPtr > styles{};
		std::stack< StylesHolderRPtr > stylesHolder{};
		castor::String controlName{};
		ButtonCtrlRPtr button{};
		ComboBoxCtrlRPtr combo{};
		EditCtrlRPtr edit{};
		ListBoxCtrlRPtr listbox{};
		SliderCtrlRPtr slider{};
		StaticCtrlRPtr staticTxt{};
		PanelCtrlRPtr panel{};
		ProgressCtrlRPtr progress{};
		ExpandablePanelCtrlRPtr expandablePanel{};
		FrameCtrlRPtr frame{};
		ScrollableCtrlRPtr scrollable{};
		ThemeRPtr theme{};
		ButtonStyleRPtr buttonStyle{};
		ComboBoxStyleRPtr comboStyle{};
		EditStyleRPtr editStyle{};
		ListBoxStyleRPtr listboxStyle{};
		SliderStyleRPtr sliderStyle{};
		StaticStyleRPtr staticStyle{};
		PanelStyleRPtr panelStyle{};
		ProgressStyleRPtr progressStyle{};
		ExpandablePanelStyleRPtr expandablePanelStyle{};
		FrameStyleRPtr frameStyle{};
		ScrollBarStyleRPtr scrollBarStyle{};
		ScrollableStyleRPtr scrollableStyle{};
		LayoutUPtr layout{};
		LayoutItemFlags layoutCtrlFlags{};

		C3D_API ControlRPtr getTopControl()const;
		C3D_API void popControl();
		C3D_API void pushStylesHolder( StylesHolder * style );
		C3D_API void popStylesHolder( StylesHolder const * style );
		C3D_API ControlStyleRPtr getTopStyle()const;
		C3D_API void popStyle();

		template< typename StyleT >
		void pushStyle( StyleT * style
			, StyleT *& result )
		{
			styles.push( style );
			result = style;

			if constexpr ( std::is_base_of_v< StylesHolder, StyleT > )
			{
				pushStylesHolder( style );
			}

			if constexpr ( std::is_base_of_v< ScrollableStyle, StyleT > )
			{
				scrollableStyle = style;
			}
		}
	};
	struct RootContext
	{
		Engine * engine{};
		OverlayContext overlays{};
		GuiContext gui{};
		bool enableFullLoading{ false };
		ScenePtrStrMap mapScenes{};
		RenderWindowDesc window{};
		castor::LoggerInstance * logger{};
		castor::PathArray files;
		castor::PathArray csnaFiles;
		std::map< castor::String, TextureSourceInfoUPtr > sourceInfos{};
	};

	template< typename BlockContextT >
	using ParserFunctionT = bool( * )( castor::FileParserContext &, BlockContextT *, castor::ParserParameterArray const & );

	static CU_ImplementAttributeParser( parserdefaultEnd )
	{
	}
	CU_EndAttributePop()

	template< typename BlockContextT >
	struct BlockParserContextT
	{
		template< typename SectionT, typename SectionU >
		explicit BlockParserContextT( castor::AttributeParsers & pparsers
			, SectionT psection
			, SectionU poldSection )
			: section{ uint32_t( psection ) }
			, oldSection{ uint32_t( poldSection ) }
			, parsers{ pparsers }
		{
		}

		template< typename SectionT >
		explicit BlockParserContextT( castor::AttributeParsers & pparsers
			, SectionT psection )
			: BlockParserContextT{ pparsers, psection, castor::PreviousSection }
		{
		}

		template< typename BlockContextU >
		void addParser( castor::String name
			, ParserFunctionT< BlockContextU > function
			, castor::ParserParameterArray params = castor::ParserParameterArray{} )
		{
			castor::addParser( parsers
				, section
				, std::move( name )
				, reinterpret_cast< ParserFunctionT< void > >( function )
				, std::move( params ) );
		}

		template< typename BlockContextU, typename SectionT >
		void addPushParser( castor::String name
			, SectionT newSection
			, ParserFunctionT< BlockContextU > function
			, castor::ParserParameterArray params = castor::ParserParameterArray{} )
		{
			castor::addParser( parsers
				, section
				, uint32_t( newSection )
				, std::move( name )
				, reinterpret_cast< ParserFunctionT< void > >( function )
				, std::move( params ) );
		}

		template< typename BlockContextU >
		void addPopParser( castor::String name
			, ParserFunctionT< BlockContextU > function )
		{
			castor::addParser( parsers
				, section
				, oldSection
				, std::move( name )
				, reinterpret_cast< ParserFunctionT< void > >( function ) );
		}

		void addDefaultPopParser()
		{
			castor::addParser( parsers
				, section
				, oldSection
				, "}"
				, parserdefaultEnd );
		}

		uint32_t section;
		uint32_t oldSection;
		castor::AttributeParsers & parsers;
	};

	class SceneFileParser
		: public castor::OwnedBy< Engine >
		, public castor::DataHolderT< RootContext >
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
		C3D_API castor::FileParserContextUPtr initialiseParser( castor::Path const & path );
		/**
		 *\~english
		 *\return		The render window defined by the scene.
		 *\~french
		 *\return		La fenêtre de rendu définie par la scène.
		 */
		RenderWindowDesc getRenderWindow()
		{
			return getData().window;
		}

		ScenePtrStrMap::iterator scenesBegin()
		{
			return getData().mapScenes.begin();
		}

		ScenePtrStrMap::const_iterator scenesBegin()const
		{
			return getData().mapScenes.begin();
		}

		ScenePtrStrMap::const_iterator scenesEnd()const
		{
			return getData().mapScenes.end();
		}

	private:
		C3D_API castor::FileParserContextUPtr doInitialiseParser( castor::Path const & path )override;
		C3D_API void doCleanupParser( castor::PreprocessedFile & preprocessed )override;
		C3D_API void doValidate( castor::PreprocessedFile & preprocessed )override;
		C3D_API castor::String doGetSectionName( castor::SectionId section )const override;
		C3D_API std::unique_ptr< FileParser > doCreateParser()const override;

	private:
		castor::String m_strSceneFilePath;

	public:
		C3D_API static UInt32StrMap comparisonModes;
	};

	template< typename ComponentT >
	ComponentT & getPassComponent( PassContext & context )
	{
		if ( !context.passComponent
			|| context.passComponent->getOwner() != context.pass
			|| getPassComponentType( *context.passComponent ) != ComponentT::TypeName )
		{
			if ( context.pass->template hasComponent< ComponentT >() )
			{
				context.passComponent = context.pass->template getComponent< ComponentT >();
			}
			else
			{
				context.passComponent = context.pass->template createComponent< ComponentT >();
			}
		}

		return static_cast< ComponentT & >( *context.passComponent );
	}

	template< typename ComponentT >
	ComponentT & getPassComponent( TextureContext & context )
	{
		return getPassComponent< ComponentT >( *context.pass );
	}

	template< typename ComponentT >
	ComponentT & getSubmeshComponent( MeshContext & context )
	{
		if ( !context.submesh )
		{
			context.submesh = context.mesh->createSubmesh();
		}

		if ( !context.submeshComponent
			|| context.submeshComponent->getOwner() != context.submesh
			|| getSubmeshComponentType( *context.submeshComponent ) != ComponentT::TypeName )
		{
			if ( context.submesh->template hasComponent< ComponentT >() )
			{
				context.submeshComponent = context.submesh->template getComponent< ComponentT >();
			}
			else
			{
				context.submeshComponent = context.submesh->template createComponent< ComponentT >();
			}
		}

		return static_cast< ComponentT & >( *context.submeshComponent );
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
