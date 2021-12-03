#include "Castor3D/Material/Pass/Pass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Cache/ShaderCache.hpp"
#include "Castor3D/Cache/TextureCache.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Pass/PassVisitor.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureConfiguration.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureSource.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Material/Texture/Animation/TextureAnimation.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Node/PassRenderNode.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneFileParser.hpp"
#include "Castor3D/Scene/Animation/AnimatedObject.hpp"
#include "Castor3D/Shader/Program.hpp"

#include <CastorUtils/Design/ResourceCache.hpp>
#include <CastorUtils/FileParser/ParserParameter.hpp>
#include <CastorUtils/Graphics/PixelFormat.hpp>

#include <algorithm>

namespace castor3d
{
	//*********************************************************************************************

	namespace
	{
		CU_ImplementAttributeParser( parserPassEmissive )
		{
			auto & parsingContext = static_cast< castor3d::SceneFileContext & >( context );

			if ( !parsingContext.pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				float value;
				params[0]->get( value );
				parsingContext.pass->setEmissive( value );
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( parserPassAlpha )
		{
			auto & parsingContext = static_cast< castor3d::SceneFileContext & >( context );

			if ( !parsingContext.pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				float fFloat;
				params[0]->get( fFloat );
				parsingContext.pass->setOpacity( fFloat );
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( parserPassDoubleFace )
		{
			auto & parsingContext = static_cast< castor3d::SceneFileContext & >( context );

			if ( !parsingContext.pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				bool value;
				params[0]->get( value );
				parsingContext.pass->setTwoSided( value );
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParserBlock( parserPassTextureUnit, CSCNSection::eTextureUnit )
		{
			auto & parsingContext = static_cast< castor3d::SceneFileContext & >( context );

			if ( parsingContext.pass )
			{
				parsingContext.textureTransform = TextureTransform{};

				if ( parsingContext.createPass
					|| parsingContext.pass->getTextureUnitsCount() <= parsingContext.unitIndex )
				{
					parsingContext.imageInfo->mipLevels = ashes::RemainingArrayLayers;
				}
				else
				{
					parsingContext.textureConfiguration = parsingContext.pass->getTextureUnit( parsingContext.unitIndex )->getConfiguration();
				}

				++parsingContext.unitIndex;
				sectionName = parsingContext.pass->getTextureSectionID();
			}
			else
			{
				CU_ParsingError( cuT( "Pass not initialised" ) );
			}
		}
		CU_EndAttributeBlock()

		CU_ImplementAttributeParser( parserPassShader )
		{
			auto & parsingContext = static_cast< castor3d::SceneFileContext & >( context );
			parsingContext.shaderProgram.reset();
			parsingContext.shaderStage = VkShaderStageFlagBits( 0u );

			if ( parsingContext.pass )
			{
				auto & cache = parsingContext.parser->getEngine()->getShaderProgramCache();
				parsingContext.shaderProgram = cache.getNewProgram( parsingContext.material->getName() + castor::string::toString( parsingContext.pass->getId() )
					, true );
			}
			else
			{
				CU_ParsingError( cuT( "Pass not initialised" ) );
			}
		}
		CU_EndAttributePush( CSCNSection::eShaderProgram )

		CU_ImplementAttributeParser( parserPassMixedInterpolative )
		{
			auto & parsingContext = static_cast< castor3d::SceneFileContext & >( context );

			if ( !parsingContext.pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				bool value;
				params[0]->get( value );

				if ( value )
				{
					parsingContext.pass->setTwoSided( true );
					parsingContext.pass->setAlphaValue( 0.95f );
					parsingContext.pass->setAlphaBlendMode( BlendMode::eInterpolative );
					parsingContext.pass->setAlphaFunc( VK_COMPARE_OP_GREATER );
					parsingContext.pass->setBlendAlphaFunc( VK_COMPARE_OP_LESS_OR_EQUAL );
				}

			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( parserPassAlphaBlendMode )
		{
			auto & parsingContext = static_cast< castor3d::SceneFileContext & >( context );

			if ( !parsingContext.pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				uint32_t mode = 0;
				params[0]->get( mode );
				parsingContext.pass->setAlphaBlendMode( BlendMode( mode ) );
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( parserPassColourBlendMode )
		{
			auto & parsingContext = static_cast< castor3d::SceneFileContext & >( context );

			if ( !parsingContext.pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				uint32_t mode = 0;
				params[0]->get( mode );
				parsingContext.pass->setColourBlendMode( BlendMode( mode ) );
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( parserPassAlphaFunc )
		{
			auto & parsingContext = static_cast< castor3d::SceneFileContext & >( context );

			if ( !parsingContext.pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				uint32_t uiFunc;
				float fFloat;
				params[0]->get( uiFunc );
				params[1]->get( fFloat );
				parsingContext.pass->setAlphaFunc( VkCompareOp( uiFunc ) );
				parsingContext.pass->setAlphaValue( fFloat );
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( parserPassBlendAlphaFunc )
		{
			auto & parsingContext = static_cast< castor3d::SceneFileContext & >( context );

			if ( !parsingContext.pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				uint32_t uiFunc;
				float fFloat;
				params[0]->get( uiFunc );
				params[1]->get( fFloat );
				parsingContext.pass->setBlendAlphaFunc( VkCompareOp( uiFunc ) );
				parsingContext.pass->setAlphaValue( fFloat );
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( parserPassRefractionRatio )
		{
			auto & parsingContext = static_cast< castor3d::SceneFileContext & >( context );

			if ( !parsingContext.pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				float value = 0;
				params[0]->get( value );
				parsingContext.pass->setRefractionRatio( value );
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( parserPassSubsurfaceScattering )
		{
			auto & parsingContext = static_cast< castor3d::SceneFileContext & >( context );

			if ( !parsingContext.pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else
			{
				parsingContext.subsurfaceScattering = std::make_unique< SubsurfaceScattering >();
			}
		}
		CU_EndAttributePush( CSCNSection::eSubsurfaceScattering )

		CU_ImplementAttributeParser( parserPassParallaxOcclusion )
		{
			auto & parsingContext = static_cast< castor3d::SceneFileContext & >( context );

			if ( !parsingContext.pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				auto value = uint32_t( ParallaxOcclusionMode::eNone );
				params[0]->get( value );
				parsingContext.pass->setParallaxOcclusion( ParallaxOcclusionMode( value ) );
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( parserPassBWAccumulationOperator )
		{
			auto & parsingContext = static_cast< castor3d::SceneFileContext & >( context );

			if ( !parsingContext.pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				uint32_t value = 0u;
				params[0]->get( value );
				parsingContext.pass->setBWAccumulationOperator( uint8_t( value ) );
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( parserPassReflections )
		{
			auto & parsingContext = static_cast< castor3d::SceneFileContext & >( context );

			if ( !parsingContext.pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				bool value{ false };
				params[0]->get( value );
				parsingContext.pass->enableReflections( value );
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( parserPassRefractions )
		{
			auto & parsingContext = static_cast< castor3d::SceneFileContext & >( context );

			if ( !parsingContext.pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				bool value{ false };
				params[0]->get( value );
				parsingContext.pass->enableRefractions( value );
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( parserPassTransmission )
		{
			auto & parsingContext = static_cast< castor3d::SceneFileContext & >( context );

			if ( !parsingContext.pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				castor::Point3f value{ 1.0f, 1.0f, 1.0f };
				params[0]->get( value );
				parsingContext.pass->setTransmission( value );
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( parserPassEdgeColour )
		{
			auto & parsingContext = static_cast< castor3d::SceneFileContext & >( context );

			if ( !parsingContext.pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				castor::RgbaColour value;
				params[0]->get( value );
				parsingContext.pass->setEdgeColour( value );
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( parserPassEdgeWidth )
		{
			auto & parsingContext = static_cast< castor3d::SceneFileContext & >( context );

			if ( !parsingContext.pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				float value;
				params[0]->get( value );
				parsingContext.pass->setEdgeWidth( value );
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( parserPassDepthFactor )
		{
			auto & parsingContext = static_cast< castor3d::SceneFileContext & >( context );

			if ( !parsingContext.pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				float value;
				params[0]->get( value );
				parsingContext.pass->setDepthFactor( value );
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( parserPassNormalFactor )
		{
			auto & parsingContext = static_cast< castor3d::SceneFileContext & >( context );

			if ( !parsingContext.pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				float value;
				params[0]->get( value );
				parsingContext.pass->setNormalFactor( value );
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( parserPassObjectFactor )
		{
			auto & parsingContext = static_cast< castor3d::SceneFileContext & >( context );

			if ( !parsingContext.pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				float value;
				params[0]->get( value );
				parsingContext.pass->setObjectFactor( value );
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( parserPassEnd )
		{
			auto & parsingContext = static_cast< castor3d::SceneFileContext & >( context );

			if ( !parsingContext.pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else
			{
				parsingContext.pass->prepareTextures();
				parsingContext.pass.reset();
			}
		}
		CU_EndAttributePop()

		CU_ImplementAttributeParser( parserUnitImage )
		{
			auto & parsingContext = static_cast< castor3d::SceneFileContext & >( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				castor::Path folder;
				castor::Path relative;
				params[0]->get( relative );

				if ( castor::File::fileExists( context.file.getPath() / relative ) )
				{
					folder = context.file.getPath();
				}
				else if ( !castor::File::fileExists( relative ) )
				{
					CU_ParsingError( cuT( "File [" ) + relative + cuT( "] not found, check the relativeness of the path" ) );
					relative.clear();
				}

				if ( !relative.empty() )
				{
					parsingContext.folder = folder;
					parsingContext.relative = relative;
					parsingContext.strName.clear();
				}
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( parserUnitLevelsCount )
		{
			auto & parsingContext = static_cast< castor3d::SceneFileContext & >( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( parsingContext.imageInfo->mipLevels );
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( parserUnitRenderTarget )
		{
			auto & parsingContext = static_cast< castor3d::SceneFileContext & >( context );
			parsingContext.targetType = TargetType::eTexture;
			parsingContext.size = { 1u, 1u };
			parsingContext.pixelFormat = castor::PixelFormat::eUNDEFINED;
		}
		CU_EndAttributePush( CSCNSection::eRenderTarget )

		CU_ImplementAttributeParser( parserUnitNormalMask )
		{
			auto & parsingContext = static_cast< castor3d::SceneFileContext & >( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( parsingContext.textureConfiguration.normalMask[0] );
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( parserUnitOpacityMask )
		{
			auto & parsingContext = static_cast< castor3d::SceneFileContext & >( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( parsingContext.textureConfiguration.opacityMask[0] );
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( parserUnitEmissiveMask )
		{
			auto & parsingContext = static_cast< castor3d::SceneFileContext & >( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( parsingContext.textureConfiguration.emissiveMask[0] );
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( parserUnitHeightMask )
		{
			auto & parsingContext = static_cast< castor3d::SceneFileContext & >( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( parsingContext.textureConfiguration.heightMask[0] );
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( parserUnitOcclusionMask )
		{
			auto & parsingContext = static_cast< castor3d::SceneFileContext & >( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( parsingContext.textureConfiguration.occlusionMask[0] );
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( parserUnitTransmittanceMask )
		{
			auto & parsingContext = static_cast< castor3d::SceneFileContext & >( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( parsingContext.textureConfiguration.transmittanceMask[0] );
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( parserUnitNormalFactor )
		{
			auto & parsingContext = static_cast< castor3d::SceneFileContext & >( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( parsingContext.textureConfiguration.normalFactor );
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( parserUnitHeightFactor )
		{
			auto & parsingContext = static_cast< castor3d::SceneFileContext & >( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( parsingContext.textureConfiguration.heightFactor );
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( parserUnitNormalDirectX )
		{
			auto & parsingContext = static_cast< castor3d::SceneFileContext & >( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				bool isDirectX;
				params[0]->get( isDirectX );
				parsingContext.textureConfiguration.normalGMultiplier = isDirectX
					? -1.0f
					: 1.0f;
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( parserUnitSampler )
		{
			auto & parsingContext = static_cast< castor3d::SceneFileContext & >( context );

			if ( !params.empty() )
			{
				castor::String name;
				auto sampler = parsingContext.parser->getEngine()->getSamplerCache().find( params[0]->get( name ) );

				if ( sampler.lock() )
				{
					parsingContext.sampler = sampler;
				}
				else
				{
					CU_ParsingError( cuT( "Unknown sampler : [" ) + name + cuT( "]" ) );
				}
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( parserUnitInvertY )
		{
			auto & parsingContext = static_cast< castor3d::SceneFileContext & >( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				bool value;
				params[0]->get( value );
				parsingContext.textureConfiguration.needsYInversion = value
					? 1u
					: 0u;
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( parserUnitTransform )
		{
			auto & parsingContext = static_cast< castor3d::SceneFileContext & >( context );

			if ( parsingContext.pass )
			{
			}
		}
		CU_EndAttributePush( CSCNSection::eTextureTransform )

		CU_ImplementAttributeParser( parserUnitTileSet )
		{
			auto & parsingContext = static_cast< castor3d::SceneFileContext & >( context );

			if ( parsingContext.pass )
			{
				castor::Point2i value;
				params[0]->get( value );
				parsingContext.textureConfiguration.tileSet->z = uint32_t( value->x );
				parsingContext.textureConfiguration.tileSet->w = uint32_t( value->y );
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( parserUnitTiles )
		{
			auto & parsingContext = static_cast< castor3d::SceneFileContext & >( context );

			if ( parsingContext.pass )
			{
				params[0]->get( parsingContext.textureConfiguration.tiles );
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( parserUnitAnimation )
		{
			auto & parsingContext = static_cast< castor3d::SceneFileContext & >( context );

			if ( parsingContext.pass )
			{
				parsingContext.textureAnimation = std::make_unique< TextureAnimation >( *parsingContext.parser->getEngine()
					, "Default" );
			}
		}
		CU_EndAttributePush( CSCNSection::eTextureAnimation )

		CU_ImplementAttributeParser( parserUnitEnd )
		{
			auto & parsingContext = static_cast< castor3d::SceneFileContext & >( context );

			if ( parsingContext.pass )
			{
				if ( !parsingContext.sampler.lock() )
				{
					parsingContext.sampler = parsingContext.parser->getEngine()->getDefaultSampler();
				}

				TextureSourceInfo sourceInfo = ( parsingContext.textureRenderTarget
					? TextureSourceInfo{ parsingContext.sampler.lock()
						, parsingContext.textureRenderTarget }
					: TextureSourceInfo{ parsingContext.sampler.lock()
						, parsingContext.folder
						, parsingContext.relative } );

				if ( parsingContext.textureAnimation && parsingContext.scene )
				{
					auto animated = parsingContext.scene->addAnimatedTexture( sourceInfo
						, parsingContext.textureConfiguration
						, *parsingContext.pass );
					parsingContext.textureAnimation->addPendingAnimated( *animated );
				}

				if ( parsingContext.textureRenderTarget )
				{
					parsingContext.pass->registerTexture( std::move( sourceInfo )
						, { { {} }
							, std::move( parsingContext.textureConfiguration ) }
						, std::move( parsingContext.textureAnimation ) );
					parsingContext.textureRenderTarget.reset();
				}
				else if ( parsingContext.folder.empty() && parsingContext.relative.empty() )
				{
					CU_ParsingError( cuT( "TextureUnit's image not initialised" ) );
				}
				else
				{
					if ( parsingContext.imageInfo->mipLevels == ashes::RemainingArrayLayers )
					{
						parsingContext.imageInfo->mipLevels = 20;
					}

					parsingContext.pass->registerTexture( std::move( sourceInfo )
						, { std::move( parsingContext.imageInfo )
							, std::move( parsingContext.textureConfiguration ) }
						, std::move( parsingContext.textureAnimation ) );
				}

				parsingContext.imageInfo =
				{
					0u,
					VK_IMAGE_TYPE_2D,
					VK_FORMAT_UNDEFINED,
					{ 1u, 1u, 1u },
					1u,
					1u,
					VK_SAMPLE_COUNT_1_BIT,
					VK_IMAGE_TILING_OPTIMAL,
					VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT
				};
				parsingContext.textureConfiguration = TextureConfiguration{};
			}
			else
			{
				CU_ParsingError( cuT( "Pass not initialised" ) );
			}
		}
		CU_EndAttributePop()
	}

	//*********************************************************************************************

	void Pass::parseError( castor::String const & error )
	{
		castor::StringStream stream{ castor::makeStringStream() };
		stream << cuT( "Error, : " ) << error;
		castor::Logger::logError( stream.str() );
	}

	void Pass::addParser( castor::AttributeParsers & parsers
		, uint32_t section
		, castor::String const & name
		, castor::ParserFunction function
		, castor::ParserParameterArray && array )
	{
		auto nameIt = parsers.find( name );

		if ( nameIt != parsers.end()
			&& nameIt->second.find( section ) != nameIt->second.end() )
		{
			parseError( cuT( "Parser " ) + name + cuT( " for section " ) + castor::string::toString( section ) + cuT( " already exists." ) );
		}
		else
		{
			parsers[name][section] = { function, array };
		}
	}

	void Pass::addCommonParsers( uint32_t mtlSectionID
		, uint32_t texSectionID
		, castor::AttributeParsers & result )
	{
		using namespace castor;

		static UInt32StrMap const blendModes{ getEnumMapT< BlendMode >() };
		static UInt32StrMap const comparisonFuncs{ getEnumMapT( VK_COMPARE_OP_NEVER, VK_COMPARE_OP_ALWAYS ) };
		static UInt32StrMap const parallaxOcclusionModes{ getEnumMapT< ParallaxOcclusionMode >() };

		Pass::addParser( result, mtlSectionID, cuT( "emissive" ), parserPassEmissive, { makeParameter< ParameterType::eFloat >() } );
		Pass::addParser( result, mtlSectionID, cuT( "alpha" ), parserPassAlpha, { makeParameter< ParameterType::eFloat >() } );
		Pass::addParser( result, mtlSectionID, cuT( "two_sided" ), parserPassDoubleFace, { makeParameter< ParameterType::eBool >() } );
		Pass::addParser( result, mtlSectionID, cuT( "texture_unit" ), parserPassTextureUnit );
		Pass::addParser( result, mtlSectionID, cuT( "shader_program" ), parserPassShader );
		Pass::addParser( result, mtlSectionID, cuT( "mixed_interpolation" ), parserPassMixedInterpolative, { makeParameter< ParameterType::eBool >() } );
		Pass::addParser( result, mtlSectionID, cuT( "alpha_blend_mode" ), parserPassAlphaBlendMode, { makeParameter< ParameterType::eCheckedText >( blendModes ) } );
		Pass::addParser( result, mtlSectionID, cuT( "colour_blend_mode" ), parserPassColourBlendMode, { makeParameter< ParameterType::eCheckedText >( blendModes ) } );
		Pass::addParser( result, mtlSectionID, cuT( "alpha_func" ), parserPassAlphaFunc, { makeParameter< ParameterType::eCheckedText >( comparisonFuncs ), makeParameter< ParameterType::eFloat >() } );
		Pass::addParser( result, mtlSectionID, cuT( "blend_alpha_func" ), parserPassBlendAlphaFunc, { makeParameter< ParameterType::eCheckedText >( comparisonFuncs ), makeParameter< ParameterType::eFloat >() } );
		Pass::addParser( result, mtlSectionID, cuT( "refraction_ratio" ), parserPassRefractionRatio, { makeParameter< ParameterType::eFloat >() } );
		Pass::addParser( result, mtlSectionID, cuT( "subsurface_scattering" ), parserPassSubsurfaceScattering );
		Pass::addParser( result, mtlSectionID, cuT( "parallax_occlusion" ), parserPassParallaxOcclusion, { makeParameter< ParameterType::eCheckedText >( parallaxOcclusionModes ) } );
		Pass::addParser( result, mtlSectionID, cuT( "bw_accumulation" ), parserPassBWAccumulationOperator, { makeParameter< ParameterType::eUInt32 >( makeRange( 0u, 6u ) ) } );
		Pass::addParser( result, mtlSectionID, cuT( "reflections" ), parserPassReflections, { makeParameter< ParameterType::eBool >() } );
		Pass::addParser( result, mtlSectionID, cuT( "refractions" ), parserPassRefractions, { makeParameter< ParameterType::eBool >() } );
		Pass::addParser( result, mtlSectionID, cuT( "transmission" ), parserPassTransmission, { makeParameter< ParameterType::ePoint3F >() } );
		Pass::addParser( result, mtlSectionID, cuT( "edge_colour" ), parserPassEdgeColour, { makeParameter< ParameterType::eRgbaColour >() } );
		Pass::addParser( result, mtlSectionID, cuT( "edge_width" ), parserPassEdgeWidth, { makeParameter< ParameterType::eFloat >( makeRange( MinEdgeWidth, MaxEdgeWidth ) ) } );
		Pass::addParser( result, mtlSectionID, cuT( "edge_depth_factor" ), parserPassDepthFactor, { makeParameter< ParameterType::eFloat >( makeRange( 0.0f, 1.0f ) ) } );
		Pass::addParser( result, mtlSectionID, cuT( "edge_normal_factor" ), parserPassNormalFactor, { makeParameter< ParameterType::eFloat >( makeRange( 0.0f, 1.0f ) ) } );
		Pass::addParser( result, mtlSectionID, cuT( "edge_object_factor" ), parserPassObjectFactor, { makeParameter< ParameterType::eFloat >( makeRange( 0.0f, 1.0f ) ) } );
		Pass::addParser( result, mtlSectionID, cuT( "}" ), parserPassEnd );

		Pass::addParser( result, texSectionID, cuT( "image" ), parserUnitImage, { makeParameter< ParameterType::ePath >() } );
		Pass::addParser( result, texSectionID, cuT( "levels_count" ), parserUnitLevelsCount, { makeParameter< ParameterType::eUInt32 >() } );
		Pass::addParser( result, texSectionID, cuT( "render_target" ), parserUnitRenderTarget );
		Pass::addParser( result, texSectionID, cuT( "normal_mask" ), parserUnitNormalMask, { makeParameter< ParameterType::eUInt32 >() } );
		Pass::addParser( result, texSectionID, cuT( "opacity_mask" ), parserUnitOpacityMask, { makeParameter< ParameterType::eUInt32 >() } );
		Pass::addParser( result, texSectionID, cuT( "emissive_mask" ), parserUnitEmissiveMask, { makeParameter< ParameterType::eUInt32 >() } );
		Pass::addParser( result, texSectionID, cuT( "height_mask" ), parserUnitHeightMask, { makeParameter< ParameterType::eUInt32 >() } );
		Pass::addParser( result, texSectionID, cuT( "occlusion_mask" ), parserUnitOcclusionMask, { makeParameter< ParameterType::eUInt32 >() } );
		Pass::addParser( result, texSectionID, cuT( "transmittance_mask" ), parserUnitTransmittanceMask, { makeParameter< ParameterType::eUInt32 >() } );
		Pass::addParser( result, texSectionID, cuT( "normal_factor" ), parserUnitNormalFactor, { makeParameter< ParameterType::eFloat >() } );
		Pass::addParser( result, texSectionID, cuT( "height_factor" ), parserUnitHeightFactor, { makeParameter< ParameterType::eFloat >() } );
		Pass::addParser( result, texSectionID, cuT( "normal_directx" ), parserUnitNormalDirectX, { makeParameter< ParameterType::eBool >() } );
		Pass::addParser( result, texSectionID, cuT( "sampler" ), parserUnitSampler, { makeParameter< ParameterType::eName >() } );
		Pass::addParser( result, texSectionID, cuT( "invert_y" ), parserUnitInvertY, { makeParameter< ParameterType::eBool >() } );
		Pass::addParser( result, texSectionID, cuT( "transform" ), parserUnitTransform );
		Pass::addParser( result, texSectionID, cuT( "tileset" ), parserUnitTileSet, { makeParameter< ParameterType::ePoint2I >() } );
		Pass::addParser( result, texSectionID, cuT( "tiles" ), parserUnitTiles, { makeParameter< ParameterType::eUInt32 >() } );
		Pass::addParser( result, texSectionID, cuT( "animation" ), parserUnitAnimation );
		Pass::addParser( result, texSectionID, cuT( "}" ), parserUnitEnd );
	}
}
