#include "Castor3D/Material/Pass/Pass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/SamplerCache.hpp"
#include "Castor3D/Cache/ShaderCache.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Pass/PassVisitor.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureConfiguration.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Material/Texture/Animation/TextureAnimation.hpp"
#include "Castor3D/Render/Node/PassRenderNode.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneFileParser.hpp"
#include "Castor3D/Scene/Animation/AnimatedObject.hpp"
#include "Castor3D/Shader/Program.hpp"

#include <CastorUtils/FileParser/ParserParameter.hpp>
#include <CastorUtils/Graphics/PixelFormat.hpp>

#include <algorithm>

namespace castor3d
{
	//*********************************************************************************************

	namespace
	{
		TextureFlags getUsedImageComponents( TextureConfiguration const & config )
		{
			auto mergeMasks = []( uint32_t toMerge
				, TextureFlag flag
				, TextureFlags & result )
			{
				result |= toMerge
					? flag
					: TextureFlag::eNone;
			};

			TextureFlags result = TextureFlag::eNone;
			mergeMasks( config.colourMask[0], TextureFlag::eDiffuse, result );
			mergeMasks( config.specularMask[0], TextureFlag::eSpecular, result );
			mergeMasks( config.metalnessMask[0], TextureFlag::eMetalness, result );
			mergeMasks( config.glossinessMask[0], TextureFlag::eGlossiness, result );
			mergeMasks( config.roughnessMask[0], TextureFlag::eRoughness, result );
			mergeMasks( config.opacityMask[0], TextureFlag::eOpacity, result );
			mergeMasks( config.emissiveMask[0], TextureFlag::eEmissive, result );
			mergeMasks( config.normalMask[0], TextureFlag::eNormal, result );
			mergeMasks( config.heightMask[0], TextureFlag::eHeight, result );
			mergeMasks( config.occlusionMask[0], TextureFlag::eOcclusion, result );
			mergeMasks( config.transmittanceMask[0], TextureFlag::eTransmittance, result );
			return result;
		}

		CU_ImplementAttributeParser( parserPassEmissive )
		{
			auto parsingContext = std::static_pointer_cast< castor3d::SceneFileContext >( context );

			if ( !parsingContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				float value;
				params[0]->get( value );
				parsingContext->pass->setEmissive( value );
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( parserPassAlpha )
		{
			auto parsingContext = std::static_pointer_cast< castor3d::SceneFileContext >( context );

			if ( !parsingContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				float fFloat;
				params[0]->get( fFloat );
				parsingContext->pass->setOpacity( fFloat );
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( parserPassDoubleFace )
		{
			auto parsingContext = std::static_pointer_cast< castor3d::SceneFileContext >( context );

			if ( !parsingContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				bool value;
				params[0]->get( value );
				parsingContext->pass->setTwoSided( value );
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParserBlock( parserPassTextureUnit, CSCNSection::eTextureUnit )
		{
			auto parsingContext = std::static_pointer_cast< castor3d::SceneFileContext >( context );
			parsingContext->textureUnit.reset();

			if ( parsingContext->pass )
			{
				parsingContext->textureTransform = TextureTransform{};

				if ( parsingContext->createPass
					|| parsingContext->pass->getTextureUnitsCount() < parsingContext->unitIndex )
				{
					parsingContext->textureUnit = std::make_shared< TextureUnit >( *parsingContext->parser->getEngine() );
					parsingContext->createUnit = true;
					parsingContext->imageInfo->mipLevels = ashes::RemainingArrayLayers;
				}
				else
				{
					parsingContext->createUnit = false;
					parsingContext->textureUnit = parsingContext->pass->getTextureUnit( parsingContext->unitIndex );
				}

				++parsingContext->unitIndex;
				sectionName = parsingContext->pass->getTextureSectionID();
			}
			else
			{
				CU_ParsingError( cuT( "Pass not initialised" ) );
			}
		}
		CU_EndAttributeBlock()

		CU_ImplementAttributeParser( parserPassShader )
		{
			auto parsingContext = std::static_pointer_cast< castor3d::SceneFileContext >( context );
			parsingContext->shaderProgram.reset();
			parsingContext->shaderStage = VkShaderStageFlagBits( 0u );

			if ( parsingContext->pass )
			{
				parsingContext->shaderProgram = parsingContext->parser->getEngine()->getShaderProgramCache().getNewProgram( parsingContext->material->getName() + castor::string::toString( parsingContext->pass->getId() )
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
			auto parsingContext = std::static_pointer_cast< castor3d::SceneFileContext >( context );

			if ( !parsingContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				bool value;
				params[0]->get( value );

				if ( value )
				{
					parsingContext->pass->setTwoSided( true );
					parsingContext->pass->setAlphaBlendMode( BlendMode::eInterpolative );
					parsingContext->pass->setAlphaFunc( VK_COMPARE_OP_GREATER );
					parsingContext->pass->setBlendAlphaFunc( VK_COMPARE_OP_LESS_OR_EQUAL );
					parsingContext->pass->setAlphaValue( 0.95f );
				}

			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( parserPassAlphaBlendMode )
		{
			auto parsingContext = std::static_pointer_cast< castor3d::SceneFileContext >( context );

			if ( !parsingContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				uint32_t mode = 0;
				params[0]->get( mode );
				parsingContext->pass->setAlphaBlendMode( BlendMode( mode ) );
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( parserPassColourBlendMode )
		{
			auto parsingContext = std::static_pointer_cast< castor3d::SceneFileContext >( context );

			if ( !parsingContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				uint32_t mode = 0;
				params[0]->get( mode );
				parsingContext->pass->setColourBlendMode( BlendMode( mode ) );
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( parserPassAlphaFunc )
		{
			auto parsingContext = std::static_pointer_cast< castor3d::SceneFileContext >( context );

			if ( !parsingContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				uint32_t uiFunc;
				float fFloat;
				params[0]->get( uiFunc );
				params[1]->get( fFloat );
				parsingContext->pass->setAlphaFunc( VkCompareOp( uiFunc ) );
				parsingContext->pass->setAlphaValue( fFloat );
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( parserPassBlendAlphaFunc )
		{
			auto parsingContext = std::static_pointer_cast< castor3d::SceneFileContext >( context );

			if ( !parsingContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				uint32_t uiFunc;
				float fFloat;
				params[0]->get( uiFunc );
				params[1]->get( fFloat );
				parsingContext->pass->setBlendAlphaFunc( VkCompareOp( uiFunc ) );
				parsingContext->pass->setAlphaValue( fFloat );
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( parserPassRefractionRatio )
		{
			auto parsingContext = std::static_pointer_cast< castor3d::SceneFileContext >( context );

			if ( !parsingContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				float value = 0;
				params[0]->get( value );
				parsingContext->pass->setRefractionRatio( value );
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( parserPassSubsurfaceScattering )
		{
			auto parsingContext = std::static_pointer_cast< castor3d::SceneFileContext >( context );

			if ( !parsingContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else
			{
				parsingContext->subsurfaceScattering = std::make_unique< SubsurfaceScattering >();
			}
		}
		CU_EndAttributePush( CSCNSection::eSubsurfaceScattering )

		CU_ImplementAttributeParser( parserPassParallaxOcclusion )
		{
			auto parsingContext = std::static_pointer_cast< castor3d::SceneFileContext >( context );

			if ( !parsingContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				auto value = uint32_t( ParallaxOcclusionMode::eNone );
				params[0]->get( value );
				parsingContext->pass->setParallaxOcclusion( ParallaxOcclusionMode( value ) );
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( parserPassBWAccumulationOperator )
		{
			auto parsingContext = std::static_pointer_cast< castor3d::SceneFileContext >( context );

			if ( !parsingContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				uint32_t value = 0u;
				params[0]->get( value );
				parsingContext->pass->setBWAccumulationOperator( uint8_t( value ) );
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( parserPassReflections )
		{
			auto parsingContext = std::static_pointer_cast< castor3d::SceneFileContext >( context );

			if ( !parsingContext->pass )
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
				parsingContext->pass->enableReflections( value );
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( parserPassRefractions )
		{
			auto parsingContext = std::static_pointer_cast< castor3d::SceneFileContext >( context );

			if ( !parsingContext->pass )
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
				parsingContext->pass->enableRefractions( value );
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( parserPassTransmission )
		{
			auto parsingContext = std::static_pointer_cast< castor3d::SceneFileContext >( context );

			if ( !parsingContext->pass )
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
				parsingContext->pass->setTransmission( value );
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( parserPassEdgeColour )
		{
			auto parsingContext = std::static_pointer_cast< castor3d::SceneFileContext >( context );

			if ( !parsingContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				castor::RgbaColour value;
				params[0]->get( value );
				parsingContext->pass->setEdgeColour( value );
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( parserPassEdgeWidth )
		{
			auto parsingContext = std::static_pointer_cast< castor3d::SceneFileContext >( context );

			if ( !parsingContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				float value;
				params[0]->get( value );
				parsingContext->pass->setEdgeWidth( value );
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( parserPassDepthFactor )
		{
			auto parsingContext = std::static_pointer_cast< castor3d::SceneFileContext >( context );

			if ( !parsingContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				float value;
				params[0]->get( value );
				parsingContext->pass->setDepthFactor( value );
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( parserPassNormalFactor )
		{
			auto parsingContext = std::static_pointer_cast< castor3d::SceneFileContext >( context );

			if ( !parsingContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				float value;
				params[0]->get( value );
				parsingContext->pass->setNormalFactor( value );
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( parserPassObjectFactor )
		{
			auto parsingContext = std::static_pointer_cast< castor3d::SceneFileContext >( context );

			if ( !parsingContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				float value;
				params[0]->get( value );
				parsingContext->pass->setObjectFactor( value );
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( parserPassEnd )
		{
			auto parsingContext = std::static_pointer_cast< castor3d::SceneFileContext >( context );

			if ( !parsingContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else
			{
				parsingContext->parser->getEngine()->prepareTextures( *parsingContext->pass );
				parsingContext->pass.reset();
			}
		}
		CU_EndAttributePop()

		CU_ImplementAttributeParser( parserUnitImage )
		{
			auto parsingContext = std::static_pointer_cast< castor3d::SceneFileContext >( context );

			if ( !parsingContext->textureUnit )
			{
				CU_ParsingError( cuT( "No TextureUnit initialised." ) );
			}
			else if ( !params.empty() )
			{
				castor::Path folder;
				castor::Path relative;
				params[0]->get( relative );

				if ( castor::File::fileExists( context->m_file.getPath() / relative ) )
				{
					folder = context->m_file.getPath();
				}
				else if ( !castor::File::fileExists( relative ) )
				{
					CU_ParsingError( cuT( "File [" ) + relative + cuT( "] not found, check the relativeness of the path" ) );
					relative.clear();
				}

				if ( !relative.empty() )
				{
					parsingContext->folder = folder;
					parsingContext->relative = relative;
					parsingContext->strName.clear();
				}
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( parserUnitLevelsCount )
		{
			auto parsingContext = std::static_pointer_cast< castor3d::SceneFileContext >( context );

			if ( !parsingContext->textureUnit )
			{
				CU_ParsingError( cuT( "No TextureUnit initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( parsingContext->imageInfo->mipLevels );
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( parserUnitRenderTarget )
		{
			auto parsingContext = std::static_pointer_cast< castor3d::SceneFileContext >( context );

			if ( !parsingContext->textureUnit )
			{
				CU_ParsingError( cuT( "No TextureUnit initialised." ) );
			}
			else
			{
				parsingContext->targetType = TargetType::eTexture;
				parsingContext->size = { 1u, 1u };
				parsingContext->pixelFormat = castor::PixelFormat::eUNDEFINED;
			}
		}
		CU_EndAttributePush( CSCNSection::eRenderTarget )

		CU_ImplementAttributeParser( parserUnitNormalMask )
		{
			auto parsingContext = std::static_pointer_cast< castor3d::SceneFileContext >( context );

			if ( !parsingContext->textureUnit )
			{
				CU_ParsingError( cuT( "No TextureUnit initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( parsingContext->textureConfiguration.normalMask[0] );
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( parserUnitOpacityMask )
		{
			auto parsingContext = std::static_pointer_cast< castor3d::SceneFileContext >( context );

			if ( !parsingContext->textureUnit )
			{
				CU_ParsingError( cuT( "No TextureUnit initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( parsingContext->textureConfiguration.opacityMask[0] );
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( parserUnitEmissiveMask )
		{
			auto parsingContext = std::static_pointer_cast< castor3d::SceneFileContext >( context );

			if ( !parsingContext->textureUnit )
			{
				CU_ParsingError( cuT( "No TextureUnit initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( parsingContext->textureConfiguration.emissiveMask[0] );
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( parserUnitHeightMask )
		{
			auto parsingContext = std::static_pointer_cast< castor3d::SceneFileContext >( context );

			if ( !parsingContext->textureUnit )
			{
				CU_ParsingError( cuT( "No TextureUnit initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( parsingContext->textureConfiguration.heightMask[0] );
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( parserUnitOcclusionMask )
		{
			auto parsingContext = std::static_pointer_cast< castor3d::SceneFileContext >( context );

			if ( !parsingContext->textureUnit )
			{
				CU_ParsingError( cuT( "No TextureUnit initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( parsingContext->textureConfiguration.occlusionMask[0] );
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( parserUnitTransmittanceMask )
		{
			auto parsingContext = std::static_pointer_cast< castor3d::SceneFileContext >( context );

			if ( !parsingContext->textureUnit )
			{
				CU_ParsingError( cuT( "No TextureUnit initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( parsingContext->textureConfiguration.transmittanceMask[0] );
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( parserUnitNormalFactor )
		{
			auto parsingContext = std::static_pointer_cast< castor3d::SceneFileContext >( context );

			if ( !parsingContext->textureUnit )
			{
				CU_ParsingError( cuT( "No TextureUnit initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( parsingContext->textureConfiguration.normalFactor );
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( parserUnitHeightFactor )
		{
			auto parsingContext = std::static_pointer_cast< castor3d::SceneFileContext >( context );

			if ( !parsingContext->textureUnit )
			{
				CU_ParsingError( cuT( "No TextureUnit initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( parsingContext->textureConfiguration.heightFactor );
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( parserUnitNormalDirectX )
		{
			auto parsingContext = std::static_pointer_cast< castor3d::SceneFileContext >( context );

			if ( !parsingContext->textureUnit )
			{
				CU_ParsingError( cuT( "No TextureUnit initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				bool isDirectX;
				params[0]->get( isDirectX );
				parsingContext->textureConfiguration.normalGMultiplier = isDirectX
					? -1.0f
					: 1.0f;
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( parserUnitSampler )
		{
			auto parsingContext = std::static_pointer_cast< castor3d::SceneFileContext >( context );

			if ( !parsingContext->textureUnit )
			{
				CU_ParsingError( cuT( "No TextureUnit initialised." ) );
			}
			else if ( !params.empty() )
			{
				castor::String name;
				SamplerSPtr sampler = parsingContext->parser->getEngine()->getSamplerCache().find( params[0]->get( name ) );

				if ( sampler )
				{
					parsingContext->textureUnit->setSampler( sampler );
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
			auto parsingContext = std::static_pointer_cast< castor3d::SceneFileContext >( context );

			if ( !parsingContext->textureUnit )
			{
				CU_ParsingError( cuT( "No TextureUnit initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				bool value;
				params[0]->get( value );
				parsingContext->textureConfiguration.needsYInversion = value
					? 1u
					: 0u;
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( parserUnitTransform )
		{
			auto parsingContext = std::static_pointer_cast< castor3d::SceneFileContext >( context );

			if ( parsingContext->pass )
			{
				if ( !parsingContext->textureUnit )
				{
					CU_ParsingError( cuT( "TextureUnit not initialised" ) );
				}
			}
		}
		CU_EndAttributePush( CSCNSection::eTextureTransform )

		CU_ImplementAttributeParser( parserUnitAnimation )
		{
			auto parsingContext = std::static_pointer_cast< castor3d::SceneFileContext >( context );

			if ( parsingContext->pass )
			{
				if ( !parsingContext->textureUnit )
				{
					CU_ParsingError( cuT( "TextureUnit not initialised" ) );
				}
				else
				{
					parsingContext->textureAnimation = &parsingContext->textureUnit->createAnimation();
					auto animated = parsingContext->scene->addAnimatedTexture( *parsingContext->textureUnit
						, *parsingContext->pass );
					animated->addAnimation( parsingContext->textureAnimation->getName() );
				}
			}
		}
		CU_EndAttributePush( CSCNSection::eTextureAnimation )

		CU_ImplementAttributeParser( parserUnitEnd )
		{
			auto parsingContext = std::static_pointer_cast< castor3d::SceneFileContext >( context );

			if ( parsingContext->pass )
			{
				if ( !parsingContext->textureUnit )
				{
					CU_ParsingError( cuT( "TextureUnit not initialised" ) );
				}
				else
				{
					if ( parsingContext->textureUnit->getRenderTarget() )
					{
						parsingContext->textureUnit->setConfiguration( parsingContext->textureConfiguration );
						parsingContext->pass->addTextureUnit( std::move( parsingContext->textureUnit ) );
						parsingContext->renderTarget = nullptr;
					}
					else if ( parsingContext->folder.empty() && parsingContext->relative.empty() )
					{
						CU_ParsingError( cuT( "TextureUnit's image not initialised" ) );
						parsingContext->textureUnit.reset();
					}
					else
					{
						if ( parsingContext->imageInfo->mipLevels == ~uint32_t( 0u ) )
						{
							parsingContext->imageInfo->mipLevels = 20;
						}

						if ( parsingContext->createUnit
							&& getUsedImageComponents( parsingContext->textureConfiguration ) != TextureFlag::eNone )
						{
							auto texture = std::make_shared< TextureLayout >( *parsingContext->parser->getEngine()->getRenderSystem()
								, parsingContext->imageInfo
								, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
								, parsingContext->relative );
							texture->setSource( parsingContext->folder
								, parsingContext->relative
								, false
								, false );
							parsingContext->textureUnit->setTexture( texture );
							parsingContext->textureUnit->setConfiguration( parsingContext->textureConfiguration );
							parsingContext->textureUnit->setTransform( parsingContext->textureTransform );
							parsingContext->pass->addTextureUnit( std::move( parsingContext->textureUnit ) );
						}
						else
						{
							parsingContext->textureUnit.reset();
						}
					}

					parsingContext->imageInfo =
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
					parsingContext->textureConfiguration = TextureConfiguration{};
				}
			}
			else
			{
				CU_ParsingError( cuT( "Pass not initialised" ) );
			}
		}
		CU_EndAttributePop()

		void mergeMasks( uint32_t lhs
			, uint32_t & rhs )
		{
			rhs |= lhs;
		}

		void mergeFactors( float lhs
			, float & rhs
			, float ref )
		{
			if ( lhs != rhs )
			{
				rhs = ( lhs == ref
					? rhs
					: lhs );
			}
		}

		void mergeConfigs( TextureConfiguration const & lhs
			, TextureConfiguration & rhs )
		{
			mergeMasks( lhs.colourMask[0], rhs.colourMask[0] );
			mergeMasks( lhs.specularMask[0], rhs.specularMask[0] );
			mergeMasks( lhs.metalnessMask[0], rhs.metalnessMask[0] );
			mergeMasks( lhs.glossinessMask[0], rhs.glossinessMask[0] );
			mergeMasks( lhs.roughnessMask[0], rhs.roughnessMask[0] );
			mergeMasks( lhs.opacityMask[0], rhs.opacityMask[0] );
			mergeMasks( lhs.emissiveMask[0], rhs.emissiveMask[0] );
			mergeMasks( lhs.normalMask[0], rhs.normalMask[0] );
			mergeMasks( lhs.heightMask[0], rhs.heightMask[0] );
			mergeMasks( lhs.occlusionMask[0], rhs.occlusionMask[0] );
			mergeMasks( lhs.transmittanceMask[0], rhs.transmittanceMask[0] );
			mergeMasks( lhs.needsGammaCorrection, rhs.needsGammaCorrection );
			mergeMasks( lhs.needsYInversion, rhs.needsYInversion );
			mergeFactors( lhs.heightFactor, rhs.heightFactor, 0.1f );
			mergeFactors( lhs.normalFactor, rhs.normalFactor, 1.0f );
			mergeFactors( lhs.normalGMultiplier, rhs.normalGMultiplier, 1.0f );
		}

		uint32_t & getMask( TextureConfiguration & config
			, uint32_t offset )
		{
			return ( *reinterpret_cast< castor::Point2ui * >( reinterpret_cast< uint8_t * >( &config ) + offset ) )[0];
		}

		uint32_t const & getMask( TextureConfiguration const & config
			, uint32_t offset )
		{
			return ( *reinterpret_cast< castor::Point2ui const * >( reinterpret_cast< uint8_t const * >( &config ) + offset ) )[0];
		}

		SamplerSPtr mergeSamplers( SamplerSPtr lhs
			, SamplerSPtr rhs
			, castor::String const & name )
		{
			if ( lhs == rhs )
			{
				return lhs;
			}

			//log::debug << name + cuT( " - Merging samplers." ) << std::endl;
			auto sampler = std::make_shared< Sampler >( *lhs->getEngine(), name );
			sampler->setBorderColour( lhs->getBorderColour() );
			sampler->setCompareOp( lhs->getCompareOp() );
			sampler->setMagFilter( lhs->getMagFilter() == VkFilter::VK_FILTER_NEAREST
				? rhs->getMagFilter()
				: lhs->getMagFilter() );
			sampler->setMinFilter( lhs->getMinFilter() == VkFilter::VK_FILTER_NEAREST
				? rhs->getMinFilter()
				: lhs->getMinFilter() );

			if ( lhs->isCompareEnabled() || rhs->isCompareEnabled() )
			{
				sampler->setCompareOp( std::max( lhs->getCompareOp(), rhs->getCompareOp() ) );
			}

			if ( lhs->isMipmapSet() || rhs->isMipmapSet() )
			{
				sampler->setMipFilter( lhs->getMipFilter() == VkSamplerMipmapMode::VK_SAMPLER_MIPMAP_MODE_NEAREST
					? rhs->getMipFilter()
					: lhs->getMipFilter() );
			}

			sampler->setWrapS( std::max( lhs->getWrapS(), rhs->getWrapS() ) );
			sampler->setWrapT( std::max( lhs->getWrapT(), rhs->getWrapT() ) );
			sampler->setWrapR( std::max( lhs->getWrapR(), rhs->getWrapR() ) );
			sampler->setMinLod( std::min( lhs->getMinLod(), rhs->getMinLod() ) );
			sampler->setMaxLod( std::max( lhs->getMaxLod(), rhs->getMaxLod() ) );
			sampler->setLodBias( std::max( lhs->getLodBias(), rhs->getLodBias() ) );
			sampler->enableAnisotropicFiltering( lhs->isAnisotropicFilteringEnabled() || rhs->isAnisotropicFilteringEnabled() );
			sampler->setMaxAnisotropy( std::max( lhs->getMaxAnisotropy(), rhs->getMaxAnisotropy() ) );
			return sampler;
		}

		castor::PxBufferBaseUPtr mergeBuffers( castor::Image const & lhs
			, uint32_t const & lhsSrcMask
			, uint32_t const & lhsDstMask
			, castor::Image const & rhs
			, uint32_t const & rhsSrcMask
			, uint32_t const & rhsDstMask
			, castor::String const & name )
		{
			// Resize images to max images dimensions, if needed.
			auto dimensions = lhs.getDimensions();
			auto lhsBuffer = lhs.getPixels();
			auto rhsBuffer = rhs.getPixels();

			if ( dimensions != rhs.getDimensions() )
			{
				dimensions.set( std::max( dimensions.getWidth(), rhs.getDimensions().getWidth() )
					, std::max( dimensions.getHeight(), rhs.getDimensions().getHeight() ) );

				if ( dimensions != lhs.getDimensions() )
				{
					//log::debug << name << cuT( " - Resizing LHS image." ) << std::endl;
					lhsBuffer = lhs.getResampled( dimensions ).getPixels();
				}

				if ( dimensions != rhs.getDimensions() )
				{
					//log::debug << name << cuT( " - Resizing RHS image." ) << std::endl;
					rhsBuffer = rhs.getResampled( dimensions ).getPixels();
				}
			}

			// Adjust the pixel formats to the most precise one
			auto pixelFormat = lhs.getPixelFormat();

			if ( pixelFormat != rhs.getPixelFormat() )
			{
				if ( getBytesPerPixel( pixelFormat ) < getBytesPerPixel( rhs.getPixelFormat() )
					|| ( !isFloatingPoint( pixelFormat )
						&& isFloatingPoint( rhs.getPixelFormat() ) )
					)
				{
					pixelFormat = rhs.getPixelFormat();
				}
			}

			// Merge the two buffers into one
			auto lhsComponents = getPixelComponents( lhsSrcMask );
			auto rhsComponents = getPixelComponents( rhsSrcMask );
			auto result = castor::PxBufferBase::createUnique( dimensions
				, getPixelFormat( pixelFormat, getPixelComponents( lhsDstMask | rhsDstMask ) ) );
			//log::debug << name << cuT( " - Copying LHS image components to result." ) << std::endl;
			copyBufferComponents( lhsComponents
				, getPixelComponents( lhsDstMask )
				, *lhsBuffer
				, *result );
			//log::debug << name << cuT( " - Copying RHS image components to result." ) << std::endl;
			copyBufferComponents( rhsComponents
				, getPixelComponents( rhsDstMask )
				, *rhsBuffer
				, *result );
			return result;
		}

		TextureLayoutSPtr getTextureLayout( Engine & engine
			, castor::PxBufferBaseUPtr buffer
			, castor::String const & name
			, bool allowCompression )
		{
#if !defined( NDEBUG )
			allowCompression = false;
#endif
			// Finish buffer initialisation.
			auto & loader = engine.getImageLoader();
			auto compressedFormat = loader.getOptions().getCompressed( buffer->getFormat() );

			if ( compressedFormat != buffer->getFormat()
				&& allowCompression )
			{
				//log::debug << name << cuT( " - Compressing result." ) << std::endl;
				buffer = castor::PxBufferBase::createUnique( &loader.getOptions()
					, buffer->getDimensions()
					, compressedFormat
					, buffer->getConstPtr()
					, buffer->getFormat() );
			}
			else if ( !castor::isCompressed( buffer->getFormat() ) )
			{
				//log::debug << name << cuT( " - Generating result mipmaps." ) << std::endl;
				buffer->generateMips();
			}

			// Create the resulting texture.
			return createTextureLayout( engine
				, name
				, std::move( buffer )
				, true );
		}

		TextureUnitSPtr prepareTexture( Engine & engine
			, castor::PxBufferBaseUPtr buffer
			, castor::String const & name
			, TextureConfiguration resultConfig )
		{
			auto unit = std::make_shared< TextureUnit >( engine );
			unit->setConfiguration( resultConfig );
			unit->setTexture( getTextureLayout( engine
				, std::move( buffer )
				, name
				, resultConfig.normalMask[0] == 0 ) );
			return unit;
		}

		bool isPreparable( TextureUnit const & unit )
		{
			return ( !unit.getRenderTarget() )
				&& unit.getTexture()
				&& unit.getTexture()->isStatic()
				&& !ashes::isCompressedFormat( unit.getTexture()->getPixelFormat() );
		}

		bool isMergeable( TextureUnit const & unit )
		{
			return isPreparable( unit )
				&& !unit.hasAnimation();
		}

		TextureUnitSPtr getPreparedTexture( Engine & engine
			, castor::String const & parentName
			, castor::String const & name
			, TextureUnitSPtr unit )
		{
			if ( isPreparable( *unit ) )
			{
				//log::debug << parentName << name << cuT( " - Preparing texture for upload." ) << std::endl;
				unit->setTexture( getTextureLayout( engine
					, std::make_unique< castor::PxBufferBase >( unit->getTexture()->getImage().getPxBuffer() )
					, unit->getTexture()->getName()
					, unit->getConfiguration().normalMask[0] == 0 ) );
			}

			return unit;
		}
	}

	//*********************************************************************************************

	Pass::Pass( Material & parent
		, PassTypeID typeID
		, PassFlags initialFlags )
		: OwnedBy< Material >{ parent }
		, m_typeID{ typeID }
		, m_flags{ PassFlag::ePickable
			| PassFlag::eLighting
			| initialFlags }
		, m_opacity{ m_dirty, 1.0f }
		, m_bwAccumulationOperator{ m_dirty, castor::makeRangedValue( 1u, 0u, 8u ) }
		, m_emissive{ m_dirty, 0.0f }
		, m_refractionRatio{ m_dirty, 0.0f }
		, m_twoSided{ m_dirty, false }
		, m_alphaBlendMode{ m_dirty, BlendMode::eNoBlend }
		, m_colourBlendMode{ m_dirty, BlendMode::eNoBlend }
		, m_alphaValue{ m_dirty, 0.0f }
		, m_transmission{ m_dirty, { 1.0f, 1.0f, 1.0f } }
		, m_alphaFunc{ m_dirty, VK_COMPARE_OP_ALWAYS }
		, m_blendAlphaFunc{ m_dirty, VK_COMPARE_OP_ALWAYS }
		, m_parallaxOcclusionMode{ m_dirty, ParallaxOcclusionMode::eNone }
		, m_edgeColour{ m_dirty, castor::RgbaColour::fromComponents( 0.0f, 0.0f, 0.0f, 1.0f ) }
		, m_edgeWidth{ m_dirty, { 1.0f, castor::makeRange( MinEdgeWidth, MaxEdgeWidth ) } }
		, m_depthFactor{ m_dirty, { 1.0f, castor::makeRange( 0.0f, 1.0f ) } }
		, m_normalFactor{ m_dirty, { 1.0f, castor::makeRange( 0.0f, 1.0f ) } }
		, m_objectFactor{ m_dirty, { 1.0f, castor::makeRange( 0.0f, 1.0f ) } }
	{
	}

	Pass::~Pass()
	{
		m_textureUnits.clear();
	}

	void Pass::initialise( RenderDevice const & device )
	{
		while ( !m_texturesReduced
			&& !m_textureUnits.empty() )
		{
			std::this_thread::sleep_for( 1_ms );
		}

		for ( auto unit : m_textureUnits )
		{
			unit->initialise( device );
		}

		doInitialise();
	}

	void Pass::cleanup()
	{
		doCleanup();

		for ( auto unit : m_textureUnits )
		{
			unit->cleanup();
		}
	}

	void Pass::update()
	{
		if ( m_dirty )
		{
			onChanged( *this );
			m_dirty = false;
		}
	}

	void Pass::accept( PassVisitorBase & vis )
	{
		doAccept( vis );
		vis.visit( cuT( "Emissive" )
			, m_emissive );
		vis.visit( cuT( "Opacity" )
			, m_opacity );
		vis.visit( cuT( "Two sided" )
			, m_twoSided );
		vis.visit( cuT( "Colour blend mode" )
			, m_colourBlendMode );
		vis.visit( cuT( "Alpha blend mode" )
			, m_alphaBlendMode );
		vis.visit( cuT( "Pass flags" )
			, m_flags );
		vis.visit( cuT( "Alpha func" )
			, m_alphaFunc );
		vis.visit( cuT( "Blend alpha func" )
			, m_blendAlphaFunc );
		vis.visit( cuT( "Alpha ref. value" )
			, m_alphaValue );
		vis.visit( cuT( "Transmission" )
			, m_transmission );
		vis.visit( cuT( "Blended weighted accumulator" )
			, m_bwAccumulationOperator );
		vis.visit( cuT( "Parallax occlusion mode" )
			, m_parallaxOcclusionMode );
		vis.visit( cuT( "Edge colour" )
			, m_edgeColour );
		vis.visit( cuT( "Edge width" )
			, m_edgeWidth );
		vis.visit( cuT( "Depth factor" )
			, m_depthFactor );
		vis.visit( cuT( "Normal factor" )
			, m_normalFactor );
		vis.visit( cuT( "Object factor" )
			, m_objectFactor );

		if ( hasRefraction() )
		{
			vis.visit( cuT( "Refraction ratio" )
				, m_refractionRatio );
		}
	}

	void Pass::accept( TextureConfiguration & configuration
		, PassVisitorBase & vis )
	{
		doAccept( configuration, vis );
		vis.visit( cuT( "Emissive" ), TextureFlag::eEmissive, configuration.emissiveMask, 3u );
		vis.visit( cuT( "Opacity" ), TextureFlag::eOpacity, configuration.opacityMask, 1u );
		vis.visit( cuT( "Occlusion" ), TextureFlag::eOcclusion, configuration.occlusionMask, 1u );
		vis.visit( cuT( "Transmittance" ), TextureFlag::eTransmittance, configuration.transmittanceMask, 1u );
		vis.visit( cuT( "Normal" ), TextureFlag::eNormal, configuration.normalMask, 3u );
		vis.visit( cuT( "Normal factor" ), configuration.normalFactor );
		vis.visit( cuT( "Normal DirectX" ), configuration.normalGMultiplier );
		vis.visit( cuT( "Height" ), TextureFlag::eHeight, configuration.heightMask, 1u );
		vis.visit( cuT( "Height factor" ), configuration.heightFactor );
	}

	void Pass::addTextureUnit( TextureUnitSPtr unit )
	{
		m_textures |= unit->getFlags();
		auto image = unit->toString();
		auto it = std::find_if( m_textureUnits.begin()
			, m_textureUnits.end()
			, [&image]( TextureUnitSPtr lookup )
			{
				return lookup->toString() == image;
			} );

		if ( it == m_textureUnits.end() )
		{
			if ( unit->getConfiguration().heightMask[0] )
			{
				m_heightTextureIndex = uint32_t( m_textureUnits.size() );
			}

			m_textureUnits.push_back( std::move( unit ) );
		}
		else
		{
			auto lhsConfig = unit->getConfiguration();
			unit = *it;
			auto rhsConfig = unit->getConfiguration();
			mergeConfigs( std::move( lhsConfig ), rhsConfig );
			unit->setConfiguration( std::move( rhsConfig ) );

			if ( unit->getConfiguration().heightMask[0] )
			{
				m_heightTextureIndex = uint32_t( std::distance( m_textureUnits.begin(), it ) );
			}
		}

		updateFlag( PassFlag::eAlphaBlending, hasAlphaBlending() );
		updateFlag( PassFlag::eAlphaTest, hasAlphaTest() );
		updateFlag( PassFlag::eBlendAlphaTest, hasBlendAlphaTest() );

		if ( m_texturesReduced.exchange( false ) )
		{
			getOwner()->getEngine()->prepareTextures( *this );
		}
	}

	void Pass::removeTextureUnit( uint32_t index )
	{
		CU_Require( index < m_textureUnits.size() );
		log::info << cuT( "Destroying TextureUnit " ) << index << std::endl;
		auto it = m_textureUnits.begin() + index;
		auto config = ( *it )->getConfiguration();
		m_textureUnits.erase( it );
		remFlag( m_textures, TextureFlag( uint16_t( getFlags( config ) ) ) );
		updateFlag( PassFlag::eAlphaBlending, hasAlphaBlending() );
		updateFlag( PassFlag::eAlphaTest, hasAlphaTest() );
		updateFlag( PassFlag::eBlendAlphaTest, hasBlendAlphaTest() );

		if ( m_texturesReduced.exchange( false ) )
		{
			getOwner()->getEngine()->prepareTextures( *this );
		}
	}

	TextureUnitSPtr Pass::getTextureUnit( uint32_t index )const
	{
		CU_Require( index < m_textureUnits.size() );
		return m_textureUnits[index];
	}

	bool Pass::needsAlphaProcessing()const
	{
		return checkFlag( m_textures, TextureFlag::eOpacity ) || m_opacity < 1.0f;
	}

	bool Pass::hasAlphaBlending()const
	{
		return needsAlphaProcessing()
			&& getAlphaBlendMode() != BlendMode::eNoBlend;
	}

	bool Pass::hasOnlyAlphaBlending()const
	{
		return needsAlphaProcessing()
			&& getAlphaBlendMode() != BlendMode::eNoBlend
			&& getAlphaFunc() == VK_COMPARE_OP_ALWAYS;
	}

	bool Pass::hasAlphaTest()const
	{
		return needsAlphaProcessing()
			&& getAlphaFunc() != VK_COMPARE_OP_ALWAYS;
	}

	bool Pass::hasBlendAlphaTest()const
	{
		return needsAlphaProcessing()
			&& getBlendAlphaFunc() != VK_COMPARE_OP_ALWAYS;
	}

	void Pass::prepareTextures()
	{
		if ( !m_texturesReduced )
		{
			for ( auto & unit : m_textureUnits )
			{
				auto configuration = unit->getConfiguration();

				if ( unit->getRenderTarget() )
				{
					configuration.needsGammaCorrection = 0u;
				}
				else if ( configuration.colourMask[0]
					|| configuration.emissiveMask[0] )
				{
					auto format = unit->getTexture()->getPixelFormat();
					configuration.needsGammaCorrection = !isFloatingPoint( convert( format ) );
				}

				unit->setConfiguration( configuration );
			}

			TextureUnitPtrArray newUnits;
			doJoinNmlHgt( newUnits );
			doJoinEmsOcc( newUnits );
			doPrepareTextures( newUnits );
			m_textureUnits = newUnits;

			std::sort( m_textureUnits.begin()
				, m_textureUnits.end()
				, []( TextureUnitSPtr const & lhs, TextureUnitSPtr const & rhs )
				{
					return lhs->getFlags() < rhs->getFlags();
				} );

			m_texturesReduced = true;
		}
	}

	void Pass::setOpacity( float value )
	{
		m_opacity = value;

		if ( needsAlphaProcessing() )
		{
			if ( m_alphaBlendMode == BlendMode::eNoBlend )
			{
				m_alphaBlendMode = BlendMode::eInterpolative;
			}
		}
		else
		{
			m_alphaBlendMode = BlendMode::eNoBlend;
		}

		updateFlag( PassFlag::eAlphaBlending, hasAlphaBlending() );
		updateFlag( PassFlag::eAlphaTest, hasAlphaTest() );
		updateFlag( PassFlag::eBlendAlphaTest, hasBlendAlphaTest() );
		doSetOpacity( value );
		m_dirty = true;
	}

	PassFlags Pass::getPassFlags()const
	{
		return m_flags;
	}

	void Pass::setSubsurfaceScattering( SubsurfaceScatteringUPtr value )
	{
		m_subsurfaceScattering = std::move( value );
		updateFlag( PassFlag::eSubsurfaceScattering, m_subsurfaceScattering != nullptr );
		m_sssConnection = m_subsurfaceScattering->onChanged.connect( [this]( SubsurfaceScattering const & sss )
			{
				onSssChanged( sss );
			} );
		m_dirty = true;
	}

	bool Pass::needsGammaCorrection()const
	{
		return m_textureUnits.end() != std::find_if( m_textureUnits.begin()
			, m_textureUnits.end()
			, []( TextureUnitSPtr unit )
			{
				return unit->getConfiguration().needsGammaCorrection != 0u;
			} );
	}

	TextureUnitPtrArray Pass::getTextureUnits( TextureFlags mask )const
	{
		TextureUnitPtrArray result;

		for ( auto & unit : m_textureUnits )
		{
			auto & config = unit->getConfiguration();

			if ( ( checkFlag( mask, TextureFlag::eAlbedo ) && config.colourMask[0] )
				|| ( checkFlag( mask, TextureFlag::eSpecular ) && config.specularMask[0] )
				|| ( checkFlag( mask, TextureFlag::eMetalness ) && config.metalnessMask[0] )
				|| ( checkFlag( mask, TextureFlag::eGlossiness ) && config.glossinessMask[0] )
				|| ( checkFlag( mask, TextureFlag::eRoughness ) && config.roughnessMask[0] )
				|| ( checkFlag( mask, TextureFlag::eOpacity ) && config.opacityMask[0] )
				|| ( checkFlag( mask, TextureFlag::eEmissive ) && config.emissiveMask[0] )
				|| ( checkFlag( mask, TextureFlag::eNormal ) && config.normalMask[0] )
				|| ( checkFlag( mask, TextureFlag::eHeight ) && config.heightMask[0] )
				|| ( checkFlag( mask, TextureFlag::eOcclusion ) && config.occlusionMask[0] ) 
				|| ( checkFlag( mask, TextureFlag::eTransmittance ) && config.transmittanceMask[0] ) )
			{
				result.push_back( unit );
			}
		}

		return result;
	}

	TextureFlagsArray Pass::getTexturesMask( TextureFlags mask )const
	{
		auto units = getTextureUnits( mask );
		TextureFlagsArray result;

		for ( auto & unit : units )
		{
			result.push_back( { unit->getFlags(), unit->getId() } );
		}

		return result;
	}

	uint32_t Pass::getTextureUnitsCount( TextureFlags mask )const
	{
		return uint32_t( getTextureUnits( mask ).size() );
	}

	void Pass::doFillData( PassBuffer::PassDataPtr & data )const
	{
		data.edgeFactors->r = getEdgeWidth();
		data.edgeFactors->g = getDepthFactor();
		data.edgeFactors->b = getNormalFactor();
		data.edgeFactors->a = getObjectFactor();
		data.edgeColour->r = getEdgeColour().red();
		data.edgeColour->g = getEdgeColour().green();
		data.edgeColour->b = getEdgeColour().blue();
		data.edgeColour->a = ( checkFlag( m_flags, PassFlag::eDrawEdge )
			? getEdgeColour().alpha()
			: 0.0f );
		data.common->r = getOpacity();
		data.common->g = getEmissive();
		data.common->b = getAlphaValue();
		data.common->a = needsGammaCorrection() ? 2.2f : 1.0f;
		data.opacity->r = getTransmission()->x;
		data.opacity->g = getTransmission()->y;
		data.opacity->b = getTransmission()->z;
		data.opacity->a = getOpacity();
		data.reflRefr->r = getRefractionRatio();
		data.reflRefr->g = hasRefraction() ? 1.0f : 0.0f;
		data.reflRefr->b = hasReflections() ? 1.0f : 0.0f;
		data.reflRefr->a = float( getBWAccumulationOperator() );

		if ( hasSubsurfaceScattering() )
		{
			auto & subsurfaceScattering = getSubsurfaceScattering();
			data.extended.sssInfo->r = 1.0f;
			data.extended.sssInfo->g = subsurfaceScattering.getGaussianWidth();
			data.extended.sssInfo->b = subsurfaceScattering.getStrength();
			data.extended.sssInfo->a = float( subsurfaceScattering.getProfileSize() );

			auto i = 0u;
			auto & transmittanceProfile = *data.extended.transmittanceProfile;

			for ( auto & factor : subsurfaceScattering )
			{
				transmittanceProfile[i].r = factor[0];
				transmittanceProfile[i].g = factor[1];
				transmittanceProfile[i].b = factor[2];
				transmittanceProfile[i].a = factor[3];
				++i;
			}
		}
		else
		{
			data.extended.sssInfo->r = 0.0f;
			data.extended.sssInfo->g = 0.0f;
			data.extended.sssInfo->b = 0.0f;
			data.extended.sssInfo->a = 0.0f;
		}
	}

	void Pass::parseError( castor::String const & error )
	{
		castor::StringStream stream{ castor::makeStringStream() };
		stream << cuT( "Error, : " ) << error;
		castor::Logger::logError( stream.str() );
	}

	void Pass::addParser( castor::AttributeParsersBySection & parsers
		, uint32_t section
		, castor::String const & name
		, castor::ParserFunction function
		, castor::ParserParameterArray && array )
	{
		auto sectionIt = parsers.find( section );

		if ( sectionIt != parsers.end()
			&& sectionIt->second.find( name ) != sectionIt->second.end() )
		{
			parseError( cuT( "Parser " ) + name + cuT( " for section " ) + castor::string::toString( section ) + cuT( " already exists." ) );
		}
		else
		{
			parsers[section][name] = { function, array };
		}
	}

	void Pass::addCommonParsers( uint32_t mtlSectionID
		, uint32_t texSectionID
		, castor::AttributeParsersBySection & result )
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
		Pass::addParser( result, texSectionID, cuT( "animation" ), parserUnitAnimation );
		Pass::addParser( result, texSectionID, cuT( "}" ), parserUnitEnd );
	}

	void Pass::doMergeImages( TextureFlag lhsFlag
		, uint32_t lhsMaskOffset
		, uint32_t lhsDstMask
		, TextureFlag rhsFlag
		, uint32_t rhsMaskOffset
		, uint32_t rhsDstMask
		, castor::String const & name
		, TextureUnitPtrArray & result )
	{
		auto texturesLhs = getTextureUnits( lhsFlag );
		auto texturesRhs = getTextureUnits( rhsFlag );

		if ( !texturesLhs.empty()
			&& !texturesRhs.empty()
			&& texturesLhs[0] != texturesRhs[0] )
		{
			auto & lhsUnit = texturesLhs[0];
			auto & rhsUnit = texturesRhs[0];

			if ( isMergeable( *lhsUnit ) && isMergeable( *rhsUnit ) )
			{
				//log::debug << getOwner()->getName() << name << cuT( " - Merging textures." ) << std::endl;
				TextureConfiguration resultConfig;
				getMask( resultConfig, lhsMaskOffset ) = lhsDstMask;
				getMask( resultConfig, rhsMaskOffset ) = rhsDstMask;
				auto newUnit = doMergeImages( lhsUnit->getTexture()->getImage()
					, lhsUnit->getConfiguration()
					, getMask( lhsUnit->getConfiguration(), lhsMaskOffset )
					, lhsDstMask
					, rhsUnit->getTexture()->getImage()
					, rhsUnit->getConfiguration()
					, getMask( rhsUnit->getConfiguration(), rhsMaskOffset )
					, rhsDstMask
					, name
					, resultConfig );
				newUnit->setOwnSampler( mergeSamplers( lhsUnit->getSampler()
					, rhsUnit->getSampler()
					, getOwner()->getName() + name ) );
				result.push_back( newUnit );
			}
			else
			{
				result.push_back( getPreparedTexture( *getOwner()->getEngine()
					, getOwner()->getName()
					, lhsUnit->getTexture()->getName()
					, lhsUnit ) );
				result.push_back( getPreparedTexture( *getOwner()->getEngine()
					, getOwner()->getName()
					, lhsUnit->getTexture()->getName()
					, rhsUnit ) );
			}
		}
		else
		{
			TextureUnitSPtr unit;

			if ( !texturesLhs.empty() )
			{
				unit = std::move( texturesLhs[0] );
			}
			else if ( !texturesRhs.empty() )
			{
				unit = std::move( texturesRhs[0] );
			}

			if ( unit )
			{
				result.push_back( getPreparedTexture( *getOwner()->getEngine()
					, getOwner()->getName()
					, name
					, unit ) );
			}
		}
	}

	void Pass::doJoinDifOpa( TextureUnitPtrArray & result
		, castor::String const & name )
	{
		doMergeImages( TextureFlag::eDiffuse
			, offsetof( TextureConfiguration, colourMask )
			, 0x00FFFFFF
			, TextureFlag::eOpacity
			, offsetof( TextureConfiguration, opacityMask )
			, 0xFF000000
			, name
			, result );
	}

	void Pass::onSssChanged( SubsurfaceScattering const & sss )
	{
		m_dirty = true;
	}

	TextureUnitSPtr Pass::doMergeImages( castor::Image const & lhs
		, TextureConfiguration const & lhsConfig
		, uint32_t lhsSrcMask
		, uint32_t lhsDstMask
		, castor::Image const & rhs
		, TextureConfiguration const & rhsConfig
		, uint32_t rhsSrcMask
		, uint32_t rhsDstMask
		, castor::String const & name
		, TextureConfiguration resultConfig )
	{
		auto merged = mergeBuffers( lhs
			, lhsSrcMask
			, lhsDstMask
			, rhs
			, rhsSrcMask
			, rhsDstMask
			, getOwner()->getName() + name );

		// Prepare the resulting texture configuration.
		resultConfig.needsGammaCorrection = !isFloatingPoint( merged->getFormat() );

		mergeMasks( lhsConfig.needsYInversion, resultConfig.needsYInversion );
		mergeFactors( lhsConfig.heightFactor, resultConfig.heightFactor, 0.1f );
		mergeFactors( lhsConfig.normalFactor, resultConfig.normalFactor, 1.0f );
		mergeFactors( lhsConfig.normalGMultiplier, resultConfig.normalGMultiplier, 1.0f );

		mergeMasks( rhsConfig.needsYInversion, resultConfig.needsYInversion );
		mergeFactors( rhsConfig.heightFactor, resultConfig.heightFactor, 0.1f );
		mergeFactors( rhsConfig.normalFactor, resultConfig.normalFactor, 1.0f );
		mergeFactors( rhsConfig.normalGMultiplier, resultConfig.normalGMultiplier, 1.0f );

		return prepareTexture( *getOwner()->getEngine()
			, std::move( merged )
			, getOwner()->getName() + name
			, resultConfig );
	}

	void Pass::doJoinNmlHgt( TextureUnitPtrArray & result )
	{
		doMergeImages( TextureFlag::eNormal
			, offsetof( TextureConfiguration, normalMask )
			, 0x00FFFFFF
			, TextureFlag::eHeight
			, offsetof( TextureConfiguration, heightMask )
			, 0xFF000000
			, cuT( "NmlHgt" )
			, result );
	}

	void Pass::doJoinEmsOcc( TextureUnitPtrArray & result )
	{
		doMergeImages( TextureFlag::eEmissive
			, offsetof( TextureConfiguration, emissiveMask )
			, 0x00FFFFFF
			, TextureFlag::eOcclusion
			, offsetof( TextureConfiguration, occlusionMask )
			, 0xFF000000
			, cuT( "EmsOcc" )
			, result );
	}
}
