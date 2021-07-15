#include "Castor3D/Material/Pass/Pass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/SamplerCache.hpp"
#include "Castor3D/Cache/ShaderCache.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureConfiguration.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Material/Texture/Animation/TextureAnimation.hpp"
#include "Castor3D/Render/Node/PassRenderNode.hpp"
#include "Castor3D/Scene/SceneFileParser.hpp"
#include "Castor3D/Shader/Program.hpp"

#include <CastorUtils/Graphics/PixelFormat.hpp>

#include <algorithm>

namespace castor3d
{
	//*********************************************************************************************

	namespace
	{
		using ashes::getName;

		template< typename EnumT >
		castor::UInt32StrMap getEnumMapT( EnumT min, EnumT max )
		{
			castor::UInt32StrMap result;

			for ( uint32_t i = uint32_t( min ); i <= uint32_t( max ); ++i )
			{
				result[getName( EnumT( i ) )] = i;
			}

			return result;
		}

		template< typename EnumT >
		castor::UInt32StrMap getEnumMapT()
		{
			return getEnumMapT( EnumT::eMin, EnumT::eMax );
		}

		CU_ImplementAttributeParser( parserPassEmissive )
		{
			SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

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
			SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

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
			SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

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

		CU_ImplementAttributeParser( parserPassTextureUnit )
		{
			SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );
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
			}
			else
			{
				CU_ParsingError( cuT( "Pass not initialised" ) );
			}
		}
		CU_EndAttributePush( CSCNSection::eTextureUnit )

		CU_ImplementAttributeParser( parserPassShader )
		{
			SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );
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
			SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

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
			SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

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
			SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

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
			SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

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
			SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

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
			SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

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
			SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

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
			SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

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
			SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

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
			SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

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
			SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

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
			SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

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

		CU_ImplementAttributeParser( parserPassEnd )
		{
			SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

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

	Pass::Pass( Material & parent )
		: OwnedBy< Material >{ parent }
		, m_flags{ PassFlag::ePickable
			| PassFlag::eLighting
			| ( ( getType() == MaterialType::eMetallicRoughness
				? PassFlag::eMetallicRoughness 
				: ( getType() == MaterialType::eSpecularGlossiness
					? PassFlag::eSpecularGlossiness
					: PassFlag::eNone ) ) ) }
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

	MaterialType Pass::getType()const
	{
		return getOwner()->getType();
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
		onChanged( *this );
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
		onChanged( *this );
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

	void Pass::addCommonParsers( uint32_t sectionID
		, castor::AttributeParsersBySection & result )
	{
		using namespace castor;

		static UInt32StrMap const blendModes{ getEnumMapT< BlendMode >() };
		static UInt32StrMap const comparisonFuncs{ getEnumMapT( VK_COMPARE_OP_NEVER, VK_COMPARE_OP_ALWAYS ) };
		static UInt32StrMap const parallaxOcclusionModes{ getEnumMapT< ParallaxOcclusionMode >() };

		addParser( result, sectionID, cuT( "emissive" ), parserPassEmissive, { makeParameter< ParameterType::eFloat >() } );
		addParser( result, sectionID, cuT( "alpha" ), parserPassAlpha, { makeParameter< ParameterType::eFloat >() } );
		addParser( result, sectionID, cuT( "two_sided" ), parserPassDoubleFace, { makeParameter< ParameterType::eBool >() } );
		addParser( result, sectionID, cuT( "texture_unit" ), parserPassTextureUnit );
		addParser( result, sectionID, cuT( "shader_program" ), parserPassShader );
		addParser( result, sectionID, cuT( "mixed_interpolation" ), parserPassMixedInterpolative, { makeParameter< ParameterType::eBool >() } );
		addParser( result, sectionID, cuT( "alpha_blend_mode" ), parserPassAlphaBlendMode, { makeParameter< ParameterType::eCheckedText >( blendModes ) } );
		addParser( result, sectionID, cuT( "colour_blend_mode" ), parserPassColourBlendMode, { makeParameter< ParameterType::eCheckedText >( blendModes ) } );
		addParser( result, sectionID, cuT( "alpha_func" ), parserPassAlphaFunc, { makeParameter< ParameterType::eCheckedText >( comparisonFuncs ), makeParameter< ParameterType::eFloat >() } );
		addParser( result, sectionID, cuT( "blend_alpha_func" ), parserPassBlendAlphaFunc, { makeParameter< ParameterType::eCheckedText >( comparisonFuncs ), makeParameter< ParameterType::eFloat >() } );
		addParser( result, sectionID, cuT( "refraction_ratio" ), parserPassRefractionRatio, { makeParameter< ParameterType::eFloat >() } );
		addParser( result, sectionID, cuT( "subsurface_scattering" ), parserPassSubsurfaceScattering );
		addParser( result, sectionID, cuT( "parallax_occlusion" ), parserPassParallaxOcclusion, { makeParameter< ParameterType::eCheckedText >( parallaxOcclusionModes ) } );
		addParser( result, sectionID, cuT( "bw_accumulation" ), parserPassBWAccumulationOperator, { makeParameter< ParameterType::eUInt32 >( makeRange( 0u, 6u ) ) } );
		addParser( result, sectionID, cuT( "reflections" ), parserPassReflections, { makeParameter< ParameterType::eBool >() } );
		addParser( result, sectionID, cuT( "refractions" ), parserPassRefractions, { makeParameter< ParameterType::eBool >() } );
		addParser( result, sectionID, cuT( "transmission" ), parserPassTransmission, { makeParameter< ParameterType::ePoint3F >() } );
		addParser( result, sectionID, cuT( "}" ), parserPassEnd );
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
		onChanged( *this );
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
