#include "Castor3D/Material/Texture/TextureUnit.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/UploadData.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Cache/TargetCache.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Pass/Component/PassComponentRegister.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Material/Texture/Animation/TextureAnimation.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneFileParserData.hpp"

#include <CastorUtils/Design/ResourceCache.hpp>
#include <CastorUtils/FileParser/FileParser.hpp>
#include <CastorUtils/FileParser/FileParser.hpp>

CU_ImplementSmartPtr( castor3d, TextureData )
CU_ImplementSmartPtr( castor3d, TextureUnit )
CU_ImplementSmartPtr( castor3d, TextureUnitData )

namespace castor3d
{
	namespace texunit
	{
		static TextureSourceInfoUPtr getSourceInfo( castor::FileParserContext & context
			, castor::String name
			, TextureContext & texture )
		{
			TextureSourceInfoUPtr result;

			if ( texture.renderTarget )
			{
				result = castor::makeUnique< TextureSourceInfo >( std::move( name )
					, std::move( texture.configuration )
					, texture.renderTarget );
			}
			else if ( texture.folder.empty() && texture.relative.empty() )
			{
				CU_ParsingError( cuT( "Texture unit's image not initialised" ) );
			}
			else
			{
				result = castor::makeUnique< TextureSourceInfo >( std::move( name )
					, std::move( texture.configuration )
					, texture.folder
					, texture.relative );
			}

			return result;
		}

		static castor::String getTextureName( TextureContext & texture )
		{
			TextureSourceInfoUPtr result;

			if ( texture.renderTarget )
			{
				return texture.renderTarget->getName();
			}

			if ( !texture.folder.empty() || !texture.relative.empty() )
			{
				return ( texture.folder / texture.relative ).getFileName();
			}

			return castor::cuEmptyString;
		}

		static CU_ImplementAttributeParserNewBlock( parserRootTexture, RootContext, TextureContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( newBlockContext->name );
				newBlockContext->root = blockContext;
			}
		}
		CU_EndAttributePushNewBlock( CSCNSection::eTexture )

		static CU_ImplementAttributeParserNewBlock( parserSceneTexture, SceneContext, TextureContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( newBlockContext->name );
				newBlockContext->root = blockContext->root;
				newBlockContext->scene = blockContext;
			}
		}
		CU_EndAttributePushNewBlock( CSCNSection::eTexture )

		static CU_ImplementAttributeParserBlock( parserImage, TextureContext )
		{
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
					auto & engine = *getEngine( *blockContext );
					blockContext->image = engine.tryFindImage( relative.getFileName() );

					if ( !blockContext->image )
					{
						auto img = engine.createImage( relative.getFileName()
							, castor::ImageCreateParams{ folder / relative
								, { false, false, false } } );
						blockContext->image = engine.addImage( relative.getFileName(), img );
					}
				}
				else if ( !castor::File::fileExists( relative ) )
				{
					CU_ParsingError( cuT( "File [" ) + relative + cuT( "] not found, check the relativeness of the path" ) );
					relative.clear();
				}

				if ( !relative.empty() )
				{
					blockContext->folder = folder;
					blockContext->relative = relative;
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserNewBlock( parserRenderTarget, TextureContext, TargetContext )
		{
			newBlockContext->targetType = TargetType::eTexture;
			newBlockContext->texture = blockContext;
		}
		CU_EndAttributePushNewBlock( CSCNSection::eRenderTarget )

		static CU_ImplementAttributeParserBlock( parserInvertY, TextureContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				bool value;
				params[0]->get( value );
				blockContext->configuration.needsYInversion = value
					? 1u
					: 0u;
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserTileSet, TextureContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				castor::Point2i value;
				params[0]->get( value );
				blockContext->configuration.tileSet->z = uint32_t( value->x );
				blockContext->configuration.tileSet->w = uint32_t( value->y );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserTile, TextureContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				castor::Point2i value;
				params[0]->get( value );
				blockContext->configuration.tileSet->x = uint32_t( value->x );
				blockContext->configuration.tileSet->y = uint32_t( value->y );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserTiles, TextureContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				params[0]->get( blockContext->configuration.tiles );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserTextureEnd, TextureContext )
		{
			if ( auto sourceInfo = getSourceInfo( context, blockContext->name, *blockContext ) )
			{
				getEngine( *blockContext )->getTextureUnitCache().getSourceData( *sourceInfo );
				blockContext->root->sourceInfos.emplace( blockContext->name, std::move( sourceInfo ) );
			}
		}
		CU_EndAttributePop()

		static CU_ImplementAttributeParserNewBlock( parserTextureUnit, PassContext, TextureContext )
		{
			if ( blockContext->pass )
			{
				newBlockContext->pass = blockContext;
				newBlockContext->root = blockContext->material->root;
				newBlockContext->scene = blockContext->material->scene;

				if ( blockContext->createPass
					|| blockContext->pass->getTextureUnitsCount() <= blockContext->unitIndex )
				{
					newBlockContext->mipLevels = ashes::RemainingArrayLayers;
				}
				else
				{
					newBlockContext->configuration = blockContext->pass->getTextureUnit( blockContext->unitIndex )->getConfiguration();
				}

				++blockContext->unitIndex;
			}
			else
			{
				CU_ParsingError( cuT( "Pass not initialised" ) );
			}
		}
		CU_EndAttributePushNewBlock( CSCNSection::eTextureUnit )

		static CU_ImplementAttributeParserBlock( parserChannel, TextureContext )
		{
			if ( !params.empty() )
			{
				PassComponentTextureFlag textures;
				params[0]->get( textures );
				auto & engine = *getEngine( *blockContext );
				engine.getPassComponentsRegister().fillChannels( textures, *blockContext );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserLevelsCount, TextureContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( blockContext->mipLevels );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserTexcoordSet, TextureContext )
		{
			if ( blockContext->pass )
			{
				params[0]->get( blockContext->texcoordSet );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserUnitTexture, TextureContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				castor::String name;
				params[0]->get( name );
				auto it = blockContext->root->sourceInfos.find( name );

				if ( it != blockContext->root->sourceInfos.end() )
				{
					blockContext->name = name;
				}
				else
				{
					CU_ParsingError( cuT( "Unknown texture : [" ) + name + cuT( "]" ) );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserSampler, TextureContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				castor::String name;
				auto sampler = getEngine( *blockContext )->findSampler( params[0]->get( name ) );

				if ( sampler )
				{
					blockContext->sampler = sampler;
				}
				else
				{
					CU_ParsingError( cuT( "Unknown sampler : [" ) + name + cuT( "]" ) );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserTransform, TextureContext )
		{
		}
		CU_EndAttributePushBlock( CSCNSection::eTextureTransform, blockContext )

		static CU_ImplementAttributeParserBlock( parserUnitEnd, TextureContext )
		{
			if ( blockContext->pass )
			{
				if ( !blockContext->sampler )
				{
					blockContext->sampler = getEngine( *blockContext )->getDefaultSampler();
				}

				TextureSourceInfo * sourceInfoPtr{};
				TextureSourceInfoUPtr ownSourceInfoPtr;

				if ( !blockContext->name.empty() )
				{
					auto it = blockContext->root->sourceInfos.find( blockContext->name );

					if ( it == blockContext->root->sourceInfos.end() )
					{
						CU_ParsingError( cuT( "Could not find texture [" ) + blockContext->name + cuT( "]" ) );
					}
					else
					{
						sourceInfoPtr = it->second.get();
					}
				}
				else
				{
					blockContext->configuration.transform = blockContext->textureTransform;
					ownSourceInfoPtr = getSourceInfo( context, getTextureName( *blockContext ), *blockContext );
					sourceInfoPtr = ownSourceInfoPtr.get();
				}

				if ( sourceInfoPtr )
				{
					auto & sourceInfo = *sourceInfoPtr;

					if ( blockContext->textureAnimation && blockContext->scene )
					{
						auto animated = blockContext->scene->scene->addAnimatedTexture( sourceInfo
							, blockContext->configuration
							, *blockContext->pass->pass );
						blockContext->textureAnimation->addPendingAnimated( *animated );
					}

					blockContext->pass->pass->registerTexture( sourceInfo
						, PassTextureConfig{ blockContext->sampler, blockContext->texcoordSet }
						, std::move( blockContext->textureAnimation ) );
				}
			}
			else
			{
				CU_ParsingError( cuT( "Pass not initialised" ) );
			}
		}
		CU_EndAttributePop()

		static CU_ImplementAttributeParserBlock( parserTransformRotate, TextureContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				blockContext->textureTransform.rotate = castor::Angle::fromDegrees( params[0]->get< float >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserTransformTranslate, TextureContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				castor::Point2f value;
				params[0]->get( value );
				blockContext->textureTransform.translate = castor::Point3f{ value->x, value->y, 0.0f };
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserTransformScale, TextureContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				castor::Point2f value;
				params[0]->get( value );
				blockContext->textureTransform.scale = castor::Point3f{ value->x, value->y, 1.0f };
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserAnimation, TextureContext )
		{
			if ( blockContext->pass )
			{
				blockContext->textureAnimation = castor::makeUnique< TextureAnimation >( *getEngine( *blockContext )
					, "Default" );
			}
		}
		CU_EndAttributePushBlock( CSCNSection::eTextureAnimation, blockContext )

		static CU_ImplementAttributeParserBlock( parserAnimRotate, TextureContext )
		{
			if ( !blockContext->textureAnimation )
			{
				CU_ParsingError( cuT( "TextureAnimation not initialised" ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				blockContext->textureAnimation->setRotateSpeed( TextureRotateSpeed{ castor::Angle::fromDegrees( params[0]->get< float >() ) } );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserAnimTranslate, TextureContext )
		{
			if ( !blockContext->textureAnimation )
			{
				CU_ParsingError( cuT( "TextureAnimation not initialised" ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				blockContext->textureAnimation->setTranslateSpeed( TextureTranslateSpeed{ params[0]->get< castor::Point2f >() } );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserAnimScale, TextureContext )
		{
			if ( !blockContext->textureAnimation )
			{
				CU_ParsingError( cuT( "TextureAnimation not initialised" ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				blockContext->textureAnimation->setScaleSpeed( TextureScaleSpeed{ params[0]->get< castor::Point2f >() } );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserAnimTile, TextureContext )
		{
			if ( !blockContext->textureAnimation )
			{
				CU_ParsingError( cuT( "TextureAnimation not initialised" ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				if ( params[0]->get< bool >() )
				{
					blockContext->textureAnimation->enableTileAnim();
				}
			}
		}
		CU_EndAttribute()
	}

	TextureUnit::TextureUnit( TextureUnit && rhs )noexcept
		: AnimableT< Engine >{ std::move( rhs ) }
		, m_data{ rhs.m_data }
		, m_device{ std::move( rhs.m_device ) }
		, m_configuration{ std::move( rhs.m_configuration ) }
		, m_transform{ std::move( rhs.m_transform ) }
		, m_transformations{ std::move( rhs.m_transformations ) }
		, m_texture{ std::move( rhs.m_texture ) }
		, m_descriptor{ std::move( rhs.m_descriptor ) }
		, m_id{ std::move( rhs.m_id ) }
		, m_name{ std::move( rhs.m_name ) }
		, m_initialised{ std::move( rhs.m_initialised ) }
		, m_animated{ std::move( rhs.m_animated ) }
		, m_setIndex{ std::move( rhs.m_setIndex ) }
	{
		getOwner()->getMaterialCache().unregisterUnit( rhs );
		rhs.m_id = 0u;
		rhs.m_initialised = false;
		rhs.m_animated = false;

		if ( m_id )
		{
			m_id = 0u;
			getOwner()->getMaterialCache().registerUnit( *this );
		}
	}

	TextureUnit::TextureUnit( Engine & engine
		, TextureUnitData & data )
		: AnimableT< Engine >{ engine }
		, m_data{ data }
	{
		m_transformations.setIdentity();
	}

	TextureUnit::~TextureUnit()noexcept
	{
		if ( getId() != 0u )
		{
			cleanup();
		}

		if ( m_data.base->sourceInfo.isRenderTarget() )
		{
			auto renderTarget = m_data.base->sourceInfo.renderTarget();
			getEngine()->getRenderTargetCache().remove( renderTarget );
		}
	}

	bool TextureUnit::initialise()
	{
		if ( m_initialised && isInitialised() )
		{
			getOwner()->getMaterialCache().registerUnit( *this );
			return m_initialised;
		}

		m_device = getEngine()->getRenderDevice();
		m_name = m_data.base->sourceInfo.name();
		m_gpuImage = m_texture->image.get();
		m_sampler->initialise( *m_device );
		m_descriptor = ashes::WriteDescriptorSet{ 0u
			, 0u
			, VkDescriptorType( VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER )
			, ashes::VkDescriptorImageInfoArray{ VkDescriptorImageInfo{ getSampler()
			, m_texture->sampledView
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL } } };

		if ( hasAnimation() )
		{
			getAnimation().initialiseTiles( *this );
		}

		m_initialised = true;
		getOwner()->getMaterialCache().registerUnit( *this );
		return true;
	}

	void TextureUnit::cleanup()noexcept
	{
		getOwner()->getMaterialCache().unregisterUnit( *this );
		m_id = 0u;

		if ( m_device )
		{
			auto & device = *m_device;
			m_device = nullptr;

			if ( m_data.base->sourceInfo.isRenderTarget() )
			{
				auto renderTarget = m_data.base->sourceInfo.renderTarget();
				renderTarget->cleanup( device );
			}
		}
	}

	void TextureUnit::setSampler( SamplerObs sampler )
	{
		m_sampler = sampler;
	}

	void TextureUnit::setTexture( Texture const * texture )
	{
		m_texture = texture;
	}

	TextureAnimation & TextureUnit::createAnimation()
	{
		if ( !hasAnimation() )
		{
			addAnimation( castor::makeUniqueDerived< Animation, TextureAnimation >( *getEngine(), "Default" ) );
			m_animated = true;
		}

		return doGetAnimation< TextureAnimation >( "Default" );
	}

	void TextureUnit::removeAnimation()
	{
		if ( hasAnimation() )
		{
			doRemoveAnimation( "Default" );
		}
	}

	TextureAnimation & TextureUnit::getAnimation()
	{
		CU_Require( hasAnimation() );
		return doGetAnimation< TextureAnimation >( "Default" );
	}

	TextureAnimation const & TextureUnit::getAnimation()const
	{
		CU_Require( hasAnimation() );
		return doGetAnimation< TextureAnimation >( "Default" );
	}

	void TextureUnit::addParsers( castor::AttributeParsers & result
		, castor::UInt32StrMap const & textureChannels )
	{
		using namespace castor;

		BlockParserContextT< RootContext > rootContext{ result, CSCNSection::eRoot };
		BlockParserContextT< SceneContext > sceneContext{ result, CSCNSection::eScene, CSCNSection::eRoot };
		BlockParserContextT< PassContext > passContext{ result, CSCNSection::ePass, CSCNSection::eMaterial };
		BlockParserContextT< TextureContext > textureContext{ result, CSCNSection::eTexture };
		BlockParserContextT< TextureContext > unitContext{ result, CSCNSection::eTextureUnit, CSCNSection::ePass };
		BlockParserContextT< TextureContext > transformContext{ result, CSCNSection::eTextureTransform, CSCNSection::eTextureUnit };
		BlockParserContextT< TextureContext > animContext{ result, CSCNSection::eTextureAnimation, CSCNSection::eTextureUnit };

		rootContext.addPushParser( cuT( "texture" ), CSCNSection::eTexture, texunit::parserRootTexture, { makeParameter< ParameterType::eName >() } );
		sceneContext.addPushParser( cuT( "texture" ), CSCNSection::eTexture, texunit::parserSceneTexture, { makeParameter< ParameterType::eName >() } );

		textureContext.addParser( cuT( "image" ), texunit::parserImage, { makeParameter< ParameterType::ePath >() } );
		textureContext.addParser( cuT( "invert_y" ), texunit::parserInvertY, { makeParameter< ParameterType::eBool >() } );
		textureContext.addParser( cuT( "tileset" ), texunit::parserTileSet, { makeParameter< ParameterType::ePoint2I >() } );
		textureContext.addParser( cuT( "tiles" ), texunit::parserTiles, { makeParameter< ParameterType::eUInt32 >() } );
		textureContext.addPushParser( cuT( "render_target" ), CSCNSection::eRenderTarget, texunit::parserRenderTarget );
		textureContext.addPopParser( cuT( "}" ), texunit::parserTextureEnd );

		passContext.addPushParser( cuT( "texture_unit" ), CSCNSection::eTextureUnit, texunit::parserTextureUnit );
		unitContext.addParser( cuT( "image" ), texunit::parserImage, { makeParameter< ParameterType::ePath >() } );
		unitContext.addParser( cuT( "invert_y" ), texunit::parserInvertY, { makeParameter< ParameterType::eBool >() } );
		unitContext.addParser( cuT( "tileset" ), texunit::parserTileSet, { makeParameter< ParameterType::ePoint2I >() } );
		unitContext.addParser( cuT( "tiles" ), texunit::parserTiles, { makeParameter< ParameterType::eUInt32 >() } );
		unitContext.addParser( cuT( "channel" ), texunit::parserChannel, { makeParameter< ParameterType::eBitwiseOred32BitsCheckedText >( "TextureChannel", textureChannels ) } );
		unitContext.addParser( cuT( "levels_count" ), texunit::parserLevelsCount, { makeParameter< ParameterType::eUInt32 >() } );
		unitContext.addParser( cuT( "texcoord_set" ), texunit::parserTexcoordSet, { makeParameter< ParameterType::eUInt32 >() } );
		unitContext.addParser( cuT( "texture" ), texunit::parserUnitTexture, { makeParameter< ParameterType::eName >() } );
		unitContext.addParser( cuT( "sampler" ), texunit::parserSampler, { makeParameter< ParameterType::eName >() } );
		unitContext.addPushParser( cuT( "render_target" ), CSCNSection::eRenderTarget, texunit::parserRenderTarget );
		unitContext.addPushParser( cuT( "animation" ), CSCNSection::eTextureAnimation, texunit::parserAnimation );
		unitContext.addPushParser( cuT( "transform" ), CSCNSection::eTextureTransform, texunit::parserTransform );
		unitContext.addPopParser( cuT( "}" ), texunit::parserUnitEnd );

		transformContext.addParser( cuT( "rotate" ), texunit::parserTransformRotate, { makeParameter< ParameterType::eFloat >() } );
		transformContext.addParser( cuT( "translate" ), texunit::parserTransformTranslate, { makeParameter< ParameterType::ePoint2F >() } );
		transformContext.addParser( cuT( "scale" ), texunit::parserTransformScale, { makeParameter< ParameterType::ePoint2F >() } );
		transformContext.addParser( cuT( "tile" ), texunit::parserTile, { makeParameter< ParameterType::ePoint2I >() } );
		transformContext.addDefaultPopParser();

		animContext.addParser( cuT( "rotate" ), texunit::parserAnimRotate, { makeParameter< ParameterType::eFloat >() } );
		animContext.addParser( cuT( "translate" ), texunit::parserAnimTranslate, { makeParameter< ParameterType::ePoint2F >() } );
		animContext.addParser( cuT( "scale" ), texunit::parserAnimScale, { makeParameter< ParameterType::ePoint2F >() } );
		animContext.addParser( cuT( "tile" ), texunit::parserAnimTile, { makeParameter< ParameterType::eBool >() } );
		animContext.addDefaultPopParser();
	}

	VkImageType TextureUnit::getType()const
	{
		CU_Require( isTextured() );
		return m_texture->imageId.data->info.imageType;
	}

	castor::String TextureUnit::toString()const
	{
		return m_name;
	}

	TextureFlagsSet TextureUnit::getFlags()const
	{
		return castor3d::getFlags( m_configuration );
	}

	bool TextureUnit::isInitialised()const
	{
		return m_texture && *m_texture;
	}

	bool TextureUnit::isTransformAnimated()const
	{
		return hasAnimation()
			&& getAnimation().isTransformAnimated();
	}

	bool TextureUnit::isTileAnimated()const
	{
		return hasAnimation()
			&& getAnimation().isTileAnimated();
	}

	ashes::Sampler const & TextureUnit::getSampler()const
	{
		CU_Require( isTextured() );
		return m_sampler->getSampler();
	}

	RenderTargetRPtr TextureUnit::getRenderTarget()const
	{
		CU_Require( isRenderTarget() );
		return m_data.base->sourceInfo.renderTarget();
	}

	castor::String TextureUnit::getTextureName()const
	{
		CU_Require( isTextured() );
		return m_name;
	}

	castor::Path TextureUnit::getTexturePath()const
	{
		CU_Require( isTextured() );
		return getCPUImage().getPath();
	}

	bool TextureUnit::isTextureStatic()const
	{
		return m_data.base->sourceInfo.isBufferImage()
			|| m_data.base->sourceInfo.isFileImage();
	}

	VkFormat TextureUnit::getTexturePixelFormat()const
	{
		CU_Require( isTextured() );
		return m_texture->imageId.data->info.format;
	}

	castor::Point3ui TextureUnit::getTextureImageTiles()const
	{
		return getCPUImage().getPxBuffer().getTiles();
	}

	bool TextureUnit::hasTextureImageBuffer()const
	{
		return getCPUImage().hasBuffer();
	}

	castor::PxBufferBase const & TextureUnit::getTextureImageBuffer()const
	{
		return getCPUImage().getPxBuffer();
	}

	VkExtent3D TextureUnit::getTextureDimensions()const
	{
		CU_Require( isTextured() );
		return m_texture->imageId.data->info.extent;
	}

	uint32_t TextureUnit::getTextureMipmapCount()const
	{
		CU_Require( isTextured() );
		return m_texture->imageId.data->info.mipLevels;
	}

	void TextureUnit::setConfiguration( TextureConfiguration value )
	{
		auto format = castor::PixelFormat::eR8G8B8A8_UNORM;

		if ( m_data.base->sourceInfo.isRenderTarget() )
		{
			auto renderTarget = m_data.base->sourceInfo.renderTarget();
			format = castor::PixelFormat( renderTarget->getPixelFormat() );
		}
		else if ( m_texture )
		{
			format = convert( getTexturePixelFormat() );
		}

		auto needsYInversion = value.needsYInversion;
		auto flippedPixels = m_data.base->image
			? m_data.base->image->getPixels()->isFlipped()
			: needsYInversion;
		m_configuration = std::move( value );
		m_configuration.needsYInversion = ( ( flippedPixels && needsYInversion )
			? 0u
			: ( ( flippedPixels || needsYInversion ) ? 1u : 0u ) );
		updateIndices( format, m_configuration );
		setTransform( m_configuration.transform );
		onChanged( *this );
	}

	void TextureUnit::setTransform( castor::Point3f const & translate
		, castor::Angle const & rotate
		, castor::Point3f const & scale )
	{
		m_transform.translate->x = translate->x;
		m_transform.translate->y = translate->y;
		m_transform.translate->z = translate->z;

		m_transform.rotate = rotate;

		m_transform.scale->x = scale->x;
		m_transform.scale->y = scale->y;
		m_transform.scale->z = scale->z;

		doUpdateTransform( translate
			, rotate
			, scale );
	}

	void TextureUnit::setTransform( TextureTransform const & transform )
	{
		m_transform = transform;
		doUpdateTransform( castor::Point3f{ m_transform.translate }
			, m_transform.rotate
			, castor::Point3f{ m_transform.scale } );
	}

	void TextureUnit::setAnimationTransform( castor::Point3f const & translate
		, castor::Angle const & rotate
		, castor::Point3f const & scale )
	{
		CU_Require( hasAnimation() );
		doUpdateTransform( castor::Point3f{ m_transform.translate } + translate
			, m_transform.rotate + rotate
			, castor::Point3f{ m_transform.scale } * scale );
	}

	void TextureUnit::setTexcoordSet( uint32_t value )
	{
		m_setIndex = value;
		onChanged( *this );
	}

	void TextureUnit::doUpdateTransform( castor::Point3f const & translate
		, castor::Angle const & rotate
		, castor::Point3f const & scale )
	{
		m_configuration.transform.translate->x = translate->x;
		m_configuration.transform.translate->y = translate->y;
		m_configuration.transform.translate->z = translate->z;

		m_configuration.transform.rotate = rotate;

		m_configuration.transform.scale->x = scale->x;
		m_configuration.transform.scale->y = scale->y;
		m_configuration.transform.scale->z = scale->z;

		onChanged( *this );
	}

	Engine * getEngine( TextureContext const & context )
	{
		return getEngine( *context.root );
	}
}
