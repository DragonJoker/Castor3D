#include "Castor3D/Material/Texture/TextureUnit.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/UploadData.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Cache/TargetCache.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Material/Pass/Component/PassComponentRegister.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Material/Texture/Animation/TextureAnimation.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneFileParser.hpp"

#include <CastorUtils/Design/ResourceCache.hpp>
#include <CastorUtils/FileParser/FileParser.hpp>
#include <CastorUtils/FileParser/ParserParameter.hpp>

CU_ImplementSmartPtr( castor3d, TextureData )
CU_ImplementSmartPtr( castor3d, TextureUnit )
CU_ImplementSmartPtr( castor3d, TextureUnitData )

namespace castor3d
{
	namespace texunit
	{
		static TextureSourceInfoUPtr getSourceInfo( castor::FileParserContext & context
			, castor::String name
			, SceneFileContext::TextureConfig & texture )
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

		static castor::String getTextureName( SceneFileContext::TextureConfig & texture )
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

		static CU_ImplementAttributeParser( parserTexture )
		{
			auto & parsingContext = getParserContext( context );

			if ( !parsingContext.scene )
			{
				CU_ParsingError( cuT( "No scene initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( parsingContext.strName );
			}
		}
		CU_EndAttributePush( CSCNSection::eTexture )

		static CU_ImplementAttributeParser( parserImage )
		{
			auto & parsingContext = getParserContext( context );

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
					auto & engine = *parsingContext.parser->getEngine();
					parsingContext.texture.image = engine.tryFindImage( relative.getFileName() );

					if ( !parsingContext.texture.image )
					{
						auto img = engine.createImage( relative.getFileName()
							, castor::ImageCreateParams{ folder / relative
								, { false, false, false } } );
						parsingContext.texture.image = engine.addImage( relative.getFileName(), img );
					}
				}
				else if ( !castor::File::fileExists( relative ) )
				{
					CU_ParsingError( cuT( "File [" ) + relative + cuT( "] not found, check the relativeness of the path" ) );
					relative.clear();
				}

				if ( !relative.empty() )
				{
					parsingContext.texture.folder = folder;
					parsingContext.texture.relative = relative;
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserRenderTarget )
		{
			auto & parsingContext = getParserContext( context );
			parsingContext.targetType = TargetType::eTexture;
		}
		CU_EndAttributePush( CSCNSection::eRenderTarget )

		static CU_ImplementAttributeParser( parserInvertY )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				bool value;
				params[0]->get( value );
				parsingContext.texture.configuration.needsYInversion = value
					? 1u
					: 0u;
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserTileSet )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				castor::Point2i value;
				params[0]->get( value );
				parsingContext.texture.configuration.tileSet->z = uint32_t( value->x );
				parsingContext.texture.configuration.tileSet->w = uint32_t( value->y );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserTile )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				castor::Point2i value;
				params[0]->get( value );
				parsingContext.texture.configuration.tileSet->x = uint32_t( value->x );
				parsingContext.texture.configuration.tileSet->y = uint32_t( value->y );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserTiles )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				params[0]->get( parsingContext.texture.configuration.tiles );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserTextureEnd )
		{
			auto & parsingContext = getParserContext( context );
			parsingContext.texture.configuration.transform = parsingContext.textureTransform;

			if ( auto sourceInfo = getSourceInfo( context, parsingContext.strName, parsingContext.texture ) )
			{
				parsingContext.parser->getEngine()->getTextureUnitCache().getSourceData( *sourceInfo );
				parsingContext.sourceInfos.emplace( parsingContext.strName, std::move( sourceInfo ) );
			}

			parsingContext.texture = {};
			parsingContext.strName = {};
		}
		CU_EndAttributePop()

		static CU_ImplementAttributeParser( parserTextureUnit )
		{
			auto & parsingContext = getParserContext( context );

			if ( parsingContext.pass )
			{
				parsingContext.textureTransform = TextureTransform{};

				if ( parsingContext.createPass
					|| parsingContext.pass->getTextureUnitsCount() <= parsingContext.unitIndex )
				{
					parsingContext.mipLevels = ashes::RemainingArrayLayers;
				}
				else
				{
					parsingContext.texture.configuration = parsingContext.pass->getTextureUnit( parsingContext.unitIndex )->getConfiguration();
				}

				++parsingContext.unitIndex;
			}
			else
			{
				CU_ParsingError( cuT( "Pass not initialised" ) );
			}
		}
		CU_EndAttributePush( CSCNSection::eTextureUnit )

		static CU_ImplementAttributeParser( parserChannel )
		{
			auto & parsingContext = getParserContext( context );

			if ( !params.empty() )
			{
				PassComponentTextureFlag textures;
				params[0]->get( textures );
				auto & engine = *parsingContext.parser->getEngine();
				engine.getPassComponentsRegister().fillChannels( textures, parsingContext );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserLevelsCount )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( parsingContext.mipLevels );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserTexcoordSet )
		{
			auto & parsingContext = getParserContext( context );

			if ( parsingContext.pass )
			{
				params[0]->get( parsingContext.texcoordSet );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserUnitTexture )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				castor::String name;
				params[0]->get( name );
				auto it = parsingContext.sourceInfos.find( name );

				if ( it != parsingContext.sourceInfos.end() )
				{
					parsingContext.strName = name;
				}
				else
				{
					CU_ParsingError( cuT( "Unknown texture : [" ) + name + cuT( "]" ) );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserSampler )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				castor::String name;
				auto sampler = parsingContext.parser->getEngine()->findSampler( params[0]->get( name ) );

				if ( sampler )
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

		static CU_ImplementAttributeParser( parserUnitEnd )
		{
			auto & parsingContext = getParserContext( context );

			if ( parsingContext.pass )
			{
				if ( !parsingContext.sampler )
				{
					parsingContext.sampler = parsingContext.parser->getEngine()->getDefaultSampler();
				}

				TextureSourceInfo * sourceInfoPtr{};
				TextureSourceInfoUPtr ownSourceInfoPtr;

				if ( !parsingContext.strName.empty() )
				{
					auto it = parsingContext.sourceInfos.find( parsingContext.strName );

					if ( it == parsingContext.sourceInfos.end() )
					{
						CU_ParsingError( cuT( "Could not find texture [" ) + parsingContext.strName + cuT( "]" ) );
					}
					else
					{
						sourceInfoPtr = it->second.get();
					}
				}
				else
				{
					parsingContext.texture.configuration.transform = parsingContext.textureTransform;
					ownSourceInfoPtr = getSourceInfo( context, getTextureName( parsingContext.texture ), parsingContext.texture );
					sourceInfoPtr = ownSourceInfoPtr.get();
					parsingContext.texture = {};
					parsingContext.strName = {};
				}

				if ( sourceInfoPtr )
				{
					auto & sourceInfo = *sourceInfoPtr;

					if ( parsingContext.textureAnimation && parsingContext.scene )
					{
						auto animated = parsingContext.scene->addAnimatedTexture( sourceInfo
							, parsingContext.texture.configuration
							, *parsingContext.pass );
						parsingContext.textureAnimation->addPendingAnimated( *animated );
					}

					parsingContext.pass->registerTexture( sourceInfo
						, PassTextureConfig{ parsingContext.sampler, parsingContext.texcoordSet }
						, std::move( parsingContext.textureAnimation ) );
				}

				parsingContext.sampler = {};
				parsingContext.texcoordSet = {};
				parsingContext.passComponent = {};
				parsingContext.textureTransform = {};
				parsingContext.textureAnimation = {};
				parsingContext.mipLevels = ashes::RemainingArrayLayers;
			}
			else
			{
				CU_ParsingError( cuT( "Pass not initialised" ) );
			}
		}
		CU_EndAttributePop()

		static CU_ImplementAttributeParser( parserTransform )
		{
		}
		CU_EndAttributePush( CSCNSection::eTextureTransform )

		static CU_ImplementAttributeParser( parserTransformRotate )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				float value;
				params[0]->get( value );
				parsingContext.textureTransform.rotate = castor::Angle::fromDegrees( value );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserTransformTranslate )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				castor::Point2f value;
				params[0]->get( value );
				parsingContext.textureTransform.translate = castor::Point3f{ value->x, value->y, 0.0f };
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserTransformScale )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				castor::Point2f value;
				params[0]->get( value );
				parsingContext.textureTransform.scale = castor::Point3f{ value->x, value->y, 1.0f };
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserAnimation )
		{
			auto & parsingContext = getParserContext( context );

			if ( parsingContext.pass )
			{
				parsingContext.textureAnimation = castor::makeUnique< TextureAnimation >( *parsingContext.parser->getEngine()
					, "Default" );
			}
		}
		CU_EndAttributePush( CSCNSection::eTextureAnimation )

		static CU_ImplementAttributeParser( parserAnimRotate )
		{
			auto & parsingContext = getParserContext( context );

			if ( !parsingContext.textureAnimation )
			{
				CU_ParsingError( cuT( "TextureAnimation not initialised" ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				float value;
				params[0]->get( value );
				parsingContext.textureAnimation->setRotateSpeed( TextureRotateSpeed{ castor::Angle::fromDegrees( value ) } );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserAnimTranslate )
		{
			auto & parsingContext = getParserContext( context );

			if ( !parsingContext.textureAnimation )
			{
				CU_ParsingError( cuT( "TextureAnimation not initialised" ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				castor::Point2f value;
				params[0]->get( value );
				parsingContext.textureAnimation->setTranslateSpeed( TextureTranslateSpeed{ value } );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserAnimScale )
		{
			auto & parsingContext = getParserContext( context );

			if ( !parsingContext.textureAnimation )
			{
				CU_ParsingError( cuT( "TextureAnimation not initialised" ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				castor::Point2f value;
				params[0]->get( value );
				parsingContext.textureAnimation->setScaleSpeed( TextureScaleSpeed{ value } );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserAnimTile )
		{
			auto & parsingContext = getParserContext( context );

			if ( !parsingContext.textureAnimation )
			{
				CU_ParsingError( cuT( "TextureAnimation not initialised" ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				bool value;
				params[0]->get( value );

				if ( value )
				{
					parsingContext.textureAnimation->enableTileAnim();
				}
			}
		}
		CU_EndAttribute()
	}

	TextureUnit::TextureUnit( TextureUnit && rhs )
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
		, m_descriptor{ 0u, 0u, 1u, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER }
	{
		m_transformations.setIdentity();
	}

	TextureUnit::~TextureUnit()
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

	void TextureUnit::cleanup()
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
		addParser( result, uint32_t( CSCNSection::eScene ), cuT( "texture" ), texunit::parserTexture, { makeParameter< ParameterType::eName >() } );
		addParser( result, uint32_t( CSCNSection::eTexture ), cuT( "image" ), texunit::parserImage, { makeParameter< ParameterType::ePath >() } );
		addParser( result, uint32_t( CSCNSection::eTexture ), cuT( "render_target" ), texunit::parserRenderTarget );
		addParser( result, uint32_t( CSCNSection::eTexture ), cuT( "invert_y" ), texunit::parserInvertY, { makeParameter< ParameterType::eBool >() } );
		addParser( result, uint32_t( CSCNSection::eTexture ), cuT( "tileset" ), texunit::parserTileSet, { makeParameter< ParameterType::ePoint2I >() } );
		addParser( result, uint32_t( CSCNSection::eTexture ), cuT( "tiles" ), texunit::parserTiles, { makeParameter< ParameterType::eUInt32 >() } );
		addParser( result, uint32_t( CSCNSection::eTexture ), cuT( "}" ), texunit::parserTextureEnd );

		addParser( result, uint32_t( CSCNSection::ePass ), cuT( "texture_unit" ), texunit::parserTextureUnit );
		addParser( result, uint32_t( CSCNSection::eTextureUnit ), cuT( "image" ), texunit::parserImage, { makeParameter< ParameterType::ePath >() } );
		addParser( result, uint32_t( CSCNSection::eTextureUnit ), cuT( "render_target" ), texunit::parserRenderTarget );
		addParser( result, uint32_t( CSCNSection::eTextureUnit ), cuT( "invert_y" ), texunit::parserInvertY, { makeParameter< ParameterType::eBool >() } );
		addParser( result, uint32_t( CSCNSection::eTextureUnit ), cuT( "tileset" ), texunit::parserTileSet, { makeParameter< ParameterType::ePoint2I >() } );
		addParser( result, uint32_t( CSCNSection::eTextureUnit ), cuT( "tiles" ), texunit::parserTiles, { makeParameter< ParameterType::eUInt32 >() } );
		addParser( result, uint32_t( CSCNSection::eTextureUnit ), cuT( "channel" ), texunit::parserChannel, { makeParameter< ParameterType::eBitwiseOred32BitsCheckedText >( "TextureChannel", textureChannels ) } );
		addParser( result, uint32_t( CSCNSection::eTextureUnit ), cuT( "levels_count" ), texunit::parserLevelsCount, { makeParameter< ParameterType::eUInt32 >() } );
		addParser( result, uint32_t( CSCNSection::eTextureUnit ), cuT( "texcoord_set" ), texunit::parserTexcoordSet, { makeParameter< ParameterType::eUInt32 >() } );
		addParser( result, uint32_t( CSCNSection::eTextureUnit ), cuT( "texture" ), texunit::parserUnitTexture, { makeParameter< ParameterType::eName >() } );
		addParser( result, uint32_t( CSCNSection::eTextureUnit ), cuT( "sampler" ), texunit::parserSampler, { makeParameter< ParameterType::eName >() } );
		addParser( result, uint32_t( CSCNSection::eTextureUnit ), cuT( "animation" ), texunit::parserAnimation );
		addParser( result, uint32_t( CSCNSection::eTextureUnit ), cuT( "}" ), texunit::parserUnitEnd );

		addParser( result, uint32_t( CSCNSection::eTextureUnit ), cuT( "transform" ), texunit::parserTransform );
		addParser( result, uint32_t( CSCNSection::eTextureTransform ), cuT( "rotate" ), texunit::parserTransformRotate, { makeParameter< ParameterType::eFloat >() } );
		addParser( result, uint32_t( CSCNSection::eTextureTransform ), cuT( "translate" ), texunit::parserTransformTranslate, { makeParameter< ParameterType::ePoint2F >() } );
		addParser( result, uint32_t( CSCNSection::eTextureTransform ), cuT( "scale" ), texunit::parserTransformScale, { makeParameter< ParameterType::ePoint2F >() } );
		addParser( result, uint32_t( CSCNSection::eTextureTransform ), cuT( "tile" ), texunit::parserTile, { makeParameter< ParameterType::ePoint2I >() } );

		addParser( result, uint32_t( CSCNSection::eTextureAnimation ), cuT( "rotate" ), texunit::parserAnimRotate, { makeParameter< ParameterType::eFloat >() } );
		addParser( result, uint32_t( CSCNSection::eTextureAnimation ), cuT( "translate" ), texunit::parserAnimTranslate, { makeParameter< ParameterType::ePoint2F >() } );
		addParser( result, uint32_t( CSCNSection::eTextureAnimation ), cuT( "scale" ), texunit::parserAnimScale, { makeParameter< ParameterType::ePoint2F >() } );
		addParser( result, uint32_t( CSCNSection::eTextureAnimation ), cuT( "tile" ), texunit::parserAnimTile, { makeParameter< ParameterType::eBool >() } );
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
}
