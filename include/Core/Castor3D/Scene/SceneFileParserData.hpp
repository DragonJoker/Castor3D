/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SceneFileParserData_H___
#define ___C3D_SceneFileParserData_H___

#include "Castor3D/Gui/GuiModule.hpp"
#include "Castor3D/Miscellaneous/MiscellaneousModule.hpp"
#include "Castor3D/Overlay/OverlayModule.hpp"
#include "Castor3D/Render/RenderModule.hpp"

#include <CastorUtils/FileParser/FileParserModule.hpp>

namespace c3d
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
		: SectionId
	{
		eRoot = c3d::makeSectionName( 'R', 'O', 'O', 'T' ),
		eScene = c3d::makeSectionName( 'S', 'C', 'N', 'E' ),
		eWindow = c3d::makeSectionName( 'W', 'N', 'D', 'W' ),
		eSampler = c3d::makeSectionName( 'S', 'M', 'P', 'R' ),
		eCamera = c3d::makeSectionName( 'C', 'A', 'M', 'R' ),
		eViewport = c3d::makeSectionName( 'V', 'W', 'P', 'T' ),
		eLight = c3d::makeSectionName( 'L', 'G', 'H', 'T' ),
		eNode = c3d::makeSectionName( 'N', 'O', 'D', 'E' ),
		eObject = c3d::makeSectionName( 'O', 'B', 'J', 'T' ),
		eObjectMaterials = c3d::makeSectionName( 'O', 'M', 'T', 'L' ),
		eFont = c3d::makeSectionName( 'F', 'O', 'N', 'T' ),
		ePanelOverlay = c3d::makeSectionName( 'P', 'O', 'V', 'L' ),
		eBorderPanelOverlay = c3d::makeSectionName( 'B', 'O', 'V', 'L' ),
		eTextOverlay = c3d::makeSectionName( 'T', 'O', 'V', 'L' ),
		eMesh = c3d::makeSectionName( 'M', 'E', 'S', 'H' ),
		eSubmesh = c3d::makeSectionName( 'S', 'M', 'S', 'H' ),
		eMaterial = c3d::makeSectionName( 'M', 'T', 'R', 'L' ),
		ePass = c3d::makeSectionName( 'P', 'A', 'S', 'S' ),
		eTextureUnit = c3d::makeSectionName( 'U', 'N', 'I', 'T' ),
		eRenderTarget = c3d::makeSectionName( 'R', 'T', 'G', 'R' ),
		eShaderProgram = c3d::makeSectionName( 'G', 'L', 'S', 'L' ),
		eShaderStage = c3d::makeSectionName( 'S', 'P', 'G', 'M' ),
		eUBOVariable = c3d::makeSectionName( 'S', 'U', 'B', 'V' ),
		eBillboard = c3d::makeSectionName( 'B', 'L', 'B', 'd' ),
		eBillboardList = c3d::makeSectionName( 'B', 'L', 'B', 'L' ),
		eAnimGroup = c3d::makeSectionName( 'A', 'N', 'G', 'P' ),
		eAnimation = c3d::makeSectionName( 'A', 'N', 'M', 'T' ),
		eSkybox = c3d::makeSectionName( 'S', 'K', 'B', 'X' ),
		eParticleSystem = c3d::makeSectionName( 'P', 'L', 'S', 'M' ),
		eParticle = c3d::makeSectionName( 'P', 'T', 'C', 'L' ),
		eSsao = c3d::makeSectionName( 'S', 'S', 'A', 'O' ),
		eHdrConfig = c3d::makeSectionName( 'H', 'D', 'R', 'C' ),
		eShadows = c3d::makeSectionName( 'S', 'H', 'D', 'W' ),
		eMeshDefaultMaterials = c3d::makeSectionName( 'M', 'M', 'T', 'L' ),
		eLpv = c3d::makeSectionName( 'L', 'P', 'V', 'I' ),
		eRaw = c3d::makeSectionName( 'R', 'A', 'W', 'S' ),
		ePcf = c3d::makeSectionName( 'P', 'C', 'F', 'S' ),
		eVsm = c3d::makeSectionName( 'V', 'S', 'M', 'S' ),
		eRsm = c3d::makeSectionName( 'R', 'S', 'M', 'S' ),
		eTextureAnimation = c3d::makeSectionName( 'T', 'X', 'A', 'N' ),
		eVoxelConeTracing = c3d::makeSectionName( 'V', 'C', 'T', 'C' ),
		eTextureTransform = c3d::makeSectionName( 'T', 'X', 'T', 'R' ),
		eSceneImport = c3d::makeSectionName( 'I', 'M', 'P', 'T' ),
		eSkeleton = c3d::makeSectionName( 'S', 'K', 'E', 'L' ),
		eMorphAnimation = c3d::makeSectionName( 'M', 'O', 'R', 'P', 'H', 'A', 'N', 'M' ),
		eTextureRemapChannel = c3d::makeSectionName( 'T', 'X', 'R', 'C' ),
		eTextureRemap = c3d::makeSectionName( 'T', 'X', 'R', 'P' ),
		eClusters = c3d::makeSectionName( 'C', 'L', 'S', 'T' ),
		eTexture = c3d::makeSectionName( 'T', 'X', 'U', 'R' ),
		eColourGrading = c3d::makeSectionName( 'C', 'L', 'G', 'D' ),
		eSdfFont = c3d::makeSectionName( 'S', 'D', 'F', 'T' ),
		eLightingModel = c3d::makeSectionName( 'L', 'G', 'M', 'L' ),
		eDefaultLightingModel = c3d::makeSectionName( 'D', 'L', 'M', 'L' ),
		eLightGroup = c3d::makeSectionName( 'L', 'T', 'G', 'P' ),
		eLightGroupInstances = c3d::makeSectionName( 'L', 'T', 'G', 'I' ),
		eLightGroupShadows = c3d::makeSectionName( 'L', 'T', 'G', 'S' ),
		eLightGroupShadowsLpv = c3d::makeSectionName( 'L', 'P', 'V', 'G' ),
		eLightGroupShadowsRaw = c3d::makeSectionName( 'R', 'A', 'W', 'G' ),
		eLightGroupShadowsPcf = c3d::makeSectionName( 'P', 'C', 'F', 'G' ),
		eLightGroupShadowsVsm = c3d::makeSectionName( 'V', 'S', 'M', 'G' ),
		eLightGroupShadowsRsm = c3d::makeSectionName( 'R', 'S', 'M', 'G' ),
		eUpscaling = c3d::makeSectionName( 'U', 'P', 'S', 'C' ),
		eSkeletonArmature = c3d::makeSectionName( 'S', 'K', 'E', 'L', 'A', 'R', 'M', 'A' ),
		eSkeletonNode = c3d::makeSectionName( 'S', 'K', 'L', 'A', 'R', 'N', 'O', 'D' ),
		eSkeletonBone = c3d::makeSectionName( 'S', 'K', 'L', 'A', 'R', 'B', 'O', 'N' ),
		eSkeletonAnimation = c3d::makeSectionName( 'S', 'K', 'E', 'L', 'A', 'N', 'I', 'M' ),
		eSkeletonAnimationObject = c3d::makeSectionName( 'S', 'K', 'L', 'A', 'N', 'O', 'B', 'J' ),
		eSkeletonAnimationKeyframe = c3d::makeSectionName( 'S', 'K', 'L', 'A', 'N', 'K', 'F', 'M' ),
		eSkeletonAnimationKeyframeObject = c3d::makeSectionName( 'S', 'K', 'A', 'N', 'K', 'F', 'O', 'B' ),
		eMeshAnimation = c3d::makeSectionName( 'M', 'E', 'S', 'H', 'A', 'N', 'I', 'M' ),
		eMeshAnimationKeyframe = c3d::makeSectionName( 'M', 'S', 'H', 'A', 'N', 'K', 'F', 'M' ),
		eMeshAnimationKeyframeWeights = c3d::makeSectionName( 'M', 'H', 'A', 'N', 'K', 'F', 'W', 'T' ),
		eNodeAnimation = c3d::makeSectionName( 'N', 'O', 'D', 'E', 'A', 'N', 'I', 'M' ),
		eNodeAnimationKeyframe = c3d::makeSectionName( 'N', 'O', 'D', 'A', 'N', 'K', 'F', 'M' ),
	};

	struct SceneContext;

	template< typename ContextT >
	static String getPrefixedName( String const & name
		, ContextT const & blockContext )
	{
		auto prefix = getPrefix( blockContext );
		return prefix.empty()
			? name
			: prefix + name;
	}

	struct RootContext
	{
		Engine * engine{};
		OverlayContextUPtr overlays{};
		GuiContextUPtr gui{};
		bool enableFullLoading{};
		ScenePtrStrMap mapScenes{};
		RenderWindowDesc window{};
		LoggerInstance * logger{};
		PathArray files{};
		PathArray csnaFiles{};
		StringMap< TextureSourceInfoUPtr > sourceInfos{};
		ProgressBar * progress{};
	};

	inline String getPrefix( RootContext const & context )
	{
		return String{};
	}

	inline Engine * getEngine( RootContext const & context )
	{
		return context.engine;
	}

	struct FontContext
	{
		RootContext * root{};
		SceneContext * scene{};
		String name{};
		Path path{};
		int16_t height{};
	};

	inline Engine * getEngine( FontContext const & context )
	{
		return getEngine( *context.root );
	}

	struct LightContext;
	struct LightGroupContext;
	struct ShadowContext
	{
		LightContext * light{};
		LightGroupContext * lightGroup{};
		ShadowConfigRPtr shadowConfig{};
	};

	C3D_API void fillMeshImportParameters( FileParserContext & context
		, String const & meshParams
		, Parameters & parameters );

	C3D_API CU_DeclareAttributeParser( parserDefaultEnd )

	template< typename BlockContextT >
	struct BlockParserContextT
	{
		template< typename SectionT, typename SectionU >
		explicit BlockParserContextT( AttributeParsers & pparsers
			, SectionT psection
			, SectionU poldSection )
			: section{ SectionId( psection ) }
			, oldSection{ SectionId( poldSection ) }
			, parsers{ pparsers }
		{
		}

		template< typename SectionT >
		explicit BlockParserContextT( AttributeParsers & pparsers
			, SectionT psection )
			: BlockParserContextT{ pparsers, psection, PreviousSection }
		{
		}

		template< typename BlockContextU >
		void addParser( String const & name
			, ParserFunctionT< BlockContextU > function
			, ParserParameterArray params = ParserParameterArray{} )
		{
			c3d::addParser( parsers
				, section
				, name
				, ParserFunctionT< void >( function )
				, c3d::move( params ) );
		}

		template< typename BlockContextU >
		void addParser( String const & name
			, RawParserFunctionT< BlockContextU > function
			, ParserParameterArray params = ParserParameterArray{} )
		{
			c3d::addParser( parsers
				, section
				, name
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
				, RawParserFunctionT< void >( function )
#pragma clang diagnostic pop
				, c3d::move( params ) );
		}

		template< typename BlockContextU, typename SectionT >
		void addPushParser( String const & name
			, SectionT newSection
			, ParserFunctionT< BlockContextU > function
			, ParserParameterArray params = ParserParameterArray{} )
		{
			c3d::addParser( parsers
				, section
				, SectionId( newSection )
				, name
				, ParserFunctionT< void >( function )
				, c3d::move( params ) );
		}

		template< typename BlockContextU, typename SectionT >
		void addPushParser( String const & name
			, SectionT newSection
			, RawParserFunctionT< BlockContextU > function
			, ParserParameterArray params = ParserParameterArray{} )
		{
			c3d::addParser( parsers
				, section
				, SectionId( newSection )
				, name
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
				, RawParserFunctionT< void >( function )
#pragma clang diagnostic pop
				, c3d::move( params ) );
		}

		template< typename BlockContextU >
		void addPopParser( String const & name
			, ParserFunctionT< BlockContextU > function )
		{
			c3d::addParser( parsers
				, section
				, oldSection
				, name
				, ParserFunctionT< void >( function ) );
		}

		template< typename BlockContextU >
		void addPopParser( String const & name
			, RawParserFunctionT< BlockContextU > function )
		{
			c3d::addParser( parsers
				, section
				, oldSection
				, name
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
				, RawParserFunctionT< void >( function ) );
#pragma clang diagnostic pop
		}

		void addDefaultPopParser()
		{
			c3d::addParser( parsers
				, section
				, oldSection
				, cuT( "}" )
				, parserDefaultEnd );
		}

		SectionId section;
		SectionId oldSection;
		AttributeParsers & parsers;
	};

	template< typename Type >
	struct LimitedType
	{
	};

	template<>
	struct ParserEnumTraits< LengthUnit >
	{
		static inline xchar const * const Name = cuT( "LengthUnit" );
		static inline UInt32StrMap const Values = []()
			{
				UInt32StrMap result;
				result = getEnumMapT< LengthUnit >();
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
				result[makeString( ashes::getName( VK_SHADER_STAGE_VERTEX_BIT ) )] = uint32_t( VK_SHADER_STAGE_VERTEX_BIT );
				result[makeString( ashes::getName( VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT ) )] = uint32_t( VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT );
				result[makeString( ashes::getName( VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT ) )] = uint32_t( VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT );
				result[makeString( ashes::getName( VK_SHADER_STAGE_GEOMETRY_BIT ) )] = uint32_t( VK_SHADER_STAGE_GEOMETRY_BIT );
				result[makeString( ashes::getName( VK_SHADER_STAGE_FRAGMENT_BIT ) )] = uint32_t( VK_SHADER_STAGE_FRAGMENT_BIT );
				result[makeString( ashes::getName( VK_SHADER_STAGE_COMPUTE_BIT ) )] = uint32_t( VK_SHADER_STAGE_COMPUTE_BIT );
				return result;
			}( );
	};

	template<>
	struct ParserEnumTraits< MipmapMode >
	{
		static inline xchar const * const Name = cuT( "MipmapMode" );
		static inline UInt32StrMap const Values = []()
			{
				UInt32StrMap result;
				result = getEnumMapT( MipmapMode::eNearest, MipmapMode::eLinear );
				result[cuT( "none" )] = uint32_t( MipmapMode::eNearest );
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
				result = getEnumMapT( VK_BLEND_FACTOR_ZERO, VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA );
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
				result = getEnumMapT( VK_IMAGE_TYPE_1D, VK_IMAGE_TYPE_3D );
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
				result = getEnumMapT( VK_BLEND_OP_ADD, VK_BLEND_OP_MAX );
				return result;
			}( );
	};

	template<>
	struct ParserEnumTraits< FilterMode >
	{
		static inline xchar const * const Name = cuT( "Filter" );
		static inline UInt32StrMap const Values = []()
			{
				UInt32StrMap result;
				result = getEnumMapT( FilterMode::eNearest, FilterMode::eLinear );
				return result;
			}( );
	};

	template<>
	struct ParserEnumTraits< WrapMode >
	{
		static inline xchar const * const Name = cuT( "AddressMode" );
		static inline UInt32StrMap const Values = []()
			{
				UInt32StrMap result;
				result = getEnumMapT( WrapMode::eRepeat, WrapMode::eMirrorClampToEdge );
				return result;
			}( );
	};

	template<>
	struct ParserEnumTraits< BorderColour >
	{
		static inline xchar const * const Name = cuT( "BorderColor" );
		static inline UInt32StrMap const Values = []()
			{
				UInt32StrMap result;
				result = getEnumMapT( BorderColour::eMin, BorderColour::eMax );
				return result;
			}( );
	};

	template<>
	struct ParserEnumTraits< ComparisonFunc >
	{
		static inline xchar const * const Name = cuT( "ComparisonFunc" );
		static inline UInt32StrMap const Values = []()
			{
				UInt32StrMap result;
				result = getEnumMapT( ComparisonFunc::eMin, ComparisonFunc::eMax );
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
				result = getEnumMapT( VK_PRIMITIVE_TOPOLOGY_POINT_LIST, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY );
				return result;
			}( );
	};

	template<>
	struct ParserEnumTraits< LimitedType< ComparisonFunc > >
	{
		static inline xchar const * const Name = cuT( "ComparisonMode" );
		static inline UInt32StrMap const Values = []()
			{
				UInt32StrMap result;
				result[cuT( "none" )] = uint32_t( false );
				result[cuT( "ref_to_texture" )] = uint32_t( true );
				return result;
			}( );
	};

	template<>
	struct ParserEnumTraits< LimitedType< VkPrimitiveTopology > >
	{
		static inline xchar const * const Name = cuT( "PrimitiveType" );
		static inline UInt32StrMap const Values = []()
			{
				UInt32StrMap result;
				result[makeString( ashes::getName( VK_PRIMITIVE_TOPOLOGY_POINT_LIST ) )] = uint32_t( VK_PRIMITIVE_TOPOLOGY_POINT_LIST );
				result[makeString( ashes::getName( VK_PRIMITIVE_TOPOLOGY_LINE_STRIP ) )] = uint32_t( VK_PRIMITIVE_TOPOLOGY_LINE_STRIP );
				result[makeString( ashes::getName( VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP ) )] = uint32_t( VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP );
				return result;
			}( );
	};
}

#endif
