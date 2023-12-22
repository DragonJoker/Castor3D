/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SceneFileParserData_H___
#define ___C3D_SceneFileParserData_H___

#include "Castor3D/Gui/GuiModule.hpp"
#include "Castor3D/Overlay/OverlayModule.hpp"
#include "Castor3D/Render/RenderModule.hpp"

#include <CastorUtils/FileParser/FileParserModule.hpp>

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

	struct SceneContext;

	struct RootContext
	{
		Engine * engine{};
		OverlayContextUPtr overlays{};
		GuiContextUPtr gui{};
		bool enableFullLoading{ false };
		ScenePtrStrMap mapScenes{};
		RenderWindowDesc window{};
		castor::LoggerInstance * logger{};
		castor::PathArray files;
		castor::PathArray csnaFiles;
		std::map< castor::String, TextureSourceInfoUPtr > sourceInfos{};
	};

	inline Engine * getEngine( RootContext const & context )
	{
		return context.engine;
	}

	struct FontContext
	{
		RootContext * root{};
		SceneContext * scene{};
		castor::String name{};
		castor::Path path{};
		int16_t fontHeight{};
	};

	inline Engine * getEngine( FontContext const & context )
	{
		return getEngine( *context.root );
	}

	template< typename BlockContextT >
	using ParserFunctionT = bool( * )( castor::FileParserContext &, BlockContextT *, castor::ParserParameterArray const & );

	C3D_API CU_DeclareAttributeParser( parserDefaultEnd )

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
				, parserDefaultEnd );
		}

		uint32_t section;
		uint32_t oldSection;
		castor::AttributeParsers & parsers;
	};

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
}

#endif
