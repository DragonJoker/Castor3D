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
		eLpv = CU_MakeSectionName( 'L', 'P', 'V', 'I' ),
		eRaw = CU_MakeSectionName( 'R', 'A', 'W', 'S' ),
		ePcf = CU_MakeSectionName( 'P', 'C', 'F', 'S' ),
		eVsm = CU_MakeSectionName( 'V', 'S', 'M', 'S' ),
		eRsm = CU_MakeSectionName( 'R', 'S', 'M', 'S' ),
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
		eSdfFont = CU_MakeSectionName( 'S', 'D', 'F', 'T' ),
		eLightingModel = CU_MakeSectionName( 'L', 'G', 'M', 'L' ),
		eDefaultLightingModel = CU_MakeSectionName( 'D', 'L', 'M', 'L' ),
		eLightGroup = CU_MakeSectionName( 'L', 'T', 'G', 'P' ),
		eLightGroupInstances = CU_MakeSectionName( 'L', 'T', 'G', 'I' ),
		eLightGroupShadows = CU_MakeSectionName( 'L', 'T', 'G', 'S' ),
		eLightGroupShadowsLpv = CU_MakeSectionName( 'L', 'P', 'V', 'G' ),
		eLightGroupShadowsRaw = CU_MakeSectionName( 'R', 'A', 'W', 'G' ),
		eLightGroupShadowsPcf = CU_MakeSectionName( 'P', 'C', 'F', 'G' ),
		eLightGroupShadowsVsm = CU_MakeSectionName( 'V', 'S', 'M', 'G' ),
		eLightGroupShadowsRsm = CU_MakeSectionName( 'R', 'S', 'M', 'G' ),
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
			: section{ uint32_t( psection ) }
			, oldSection{ uint32_t( poldSection ) }
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
				, RawParserFunctionT< void >( function )
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
				, uint32_t( newSection )
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
				, uint32_t( newSection )
				, name
				, RawParserFunctionT< void >( function )
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
				, RawParserFunctionT< void >( function ) );
		}

		void addDefaultPopParser()
		{
			c3d::addParser( parsers
				, section
				, oldSection
				, cuT( "}" )
				, parserDefaultEnd );
		}

		uint32_t section;
		uint32_t oldSection;
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
