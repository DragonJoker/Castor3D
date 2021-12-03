#include "Castor3D/Cache/TextureCache.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Texture/TextureSourceInfo.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderTarget.hpp"

namespace castor3d
{
	using ashes::operator==;
	using ashes::operator!=;

	//*********************************************************************************************

	namespace
	{
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

		void mergeConfigsBase( TextureConfiguration const & lhs
			, TextureConfiguration & rhs )
		{
			mergeMasks( lhs.needsYInversion, rhs.needsYInversion );
			mergeFactors( lhs.heightFactor, rhs.heightFactor, 0.1f );
			mergeFactors( lhs.normalFactor, rhs.normalFactor, 1.0f );
			mergeFactors( lhs.normalGMultiplier, rhs.normalGMultiplier, 1.0f );
		}

		castor::PixelFormat getSRGBFormat( castor::PixelFormat pformat )
		{
			auto get = []( castor::PixelFormat format )
			{
				switch ( format )
				{
				case castor::PixelFormat::eR8_UNORM:
					return castor::PixelFormat::eR8_SRGB;
				case castor::PixelFormat::eR8G8_UNORM:
					return castor::PixelFormat::eR8G8_SRGB;
				case castor::PixelFormat::eR8G8B8_UNORM:
					return castor::PixelFormat::eR8G8B8_SRGB;
				case castor::PixelFormat::eB8G8R8_UNORM:
					return castor::PixelFormat::eB8G8R8_SRGB;
				case castor::PixelFormat::eR8G8B8A8_UNORM:
					return castor::PixelFormat::eR8G8B8A8_SRGB;
				case castor::PixelFormat::eB8G8R8A8_UNORM:
					return castor::PixelFormat::eB8G8R8A8_SRGB;
				case castor::PixelFormat::eA8B8G8R8_UNORM:
					return castor::PixelFormat::eA8B8G8R8_SRGB;
				case castor::PixelFormat::eBC1_RGB_UNORM_BLOCK:
					return castor::PixelFormat::eBC1_RGB_SRGB_BLOCK;
				case castor::PixelFormat::eBC1_RGBA_UNORM_BLOCK:
					return castor::PixelFormat::eBC1_RGBA_SRGB_BLOCK;
				case castor::PixelFormat::eBC2_UNORM_BLOCK:
					return castor::PixelFormat::eBC2_SRGB_BLOCK;
				case castor::PixelFormat::eBC3_UNORM_BLOCK:
					return castor::PixelFormat::eBC3_SRGB_BLOCK;
				case castor::PixelFormat::eBC7_UNORM_BLOCK:
					return castor::PixelFormat::eBC7_SRGB_BLOCK;
				case castor::PixelFormat::eETC2_R8G8B8_UNORM_BLOCK:
					return castor::PixelFormat::eETC2_R8G8B8_SRGB_BLOCK;
				case castor::PixelFormat::eETC2_R8G8B8A1_UNORM_BLOCK:
					return castor::PixelFormat::eETC2_R8G8B8A1_SRGB_BLOCK;
				case castor::PixelFormat::eETC2_R8G8B8A8_UNORM_BLOCK:
					return castor::PixelFormat::eETC2_R8G8B8A8_SRGB_BLOCK;
				case castor::PixelFormat::eASTC_4x4_UNORM_BLOCK:
					return castor::PixelFormat::eASTC_4x4_SRGB_BLOCK;
				case castor::PixelFormat::eASTC_5x4_UNORM_BLOCK:
					return castor::PixelFormat::eASTC_5x4_SRGB_BLOCK;
				case castor::PixelFormat::eASTC_5x5_UNORM_BLOCK:
					return castor::PixelFormat::eASTC_5x5_SRGB_BLOCK;
				case castor::PixelFormat::eASTC_6x5_UNORM_BLOCK:
					return castor::PixelFormat::eASTC_6x5_SRGB_BLOCK;
				case castor::PixelFormat::eASTC_6x6_UNORM_BLOCK:
					return castor::PixelFormat::eASTC_6x6_SRGB_BLOCK;
				case castor::PixelFormat::eASTC_8x5_UNORM_BLOCK:
					return castor::PixelFormat::eASTC_8x5_SRGB_BLOCK;
				case castor::PixelFormat::eASTC_8x6_SRGB_BLOCK:
					return castor::PixelFormat::eASTC_8x6_SRGB_BLOCK;
				case castor::PixelFormat::eASTC_8x8_UNORM_BLOCK:
					return castor::PixelFormat::eASTC_8x8_SRGB_BLOCK;
				case castor::PixelFormat::eASTC_10x5_UNORM_BLOCK:
					return castor::PixelFormat::eASTC_10x5_SRGB_BLOCK;
				case castor::PixelFormat::eASTC_10x6_UNORM_BLOCK:
					return castor::PixelFormat::eASTC_10x6_SRGB_BLOCK;
				case castor::PixelFormat::eASTC_10x8_UNORM_BLOCK:
					return castor::PixelFormat::eASTC_10x8_SRGB_BLOCK;
				case castor::PixelFormat::eASTC_10x10_UNORM_BLOCK:
					return castor::PixelFormat::eASTC_10x10_SRGB_BLOCK;
				case castor::PixelFormat::eASTC_12x10_UNORM_BLOCK:
					return castor::PixelFormat::eASTC_12x10_SRGB_BLOCK;
				case castor::PixelFormat::eASTC_12x12_UNORM_BLOCK:
					return castor::PixelFormat::eASTC_12x12_SRGB_BLOCK;
				default:
					return format;
				}
			};
			pformat = get( pformat );
			CU_Require( isSRGBFormat( pformat ) );
			return pformat;
		}

		castor::PixelFormat getNonSRGBFormat( castor::PixelFormat pformat )
		{
			auto get = []( castor::PixelFormat format )
			{
				switch ( format )
				{
				case castor::PixelFormat::eR8_SRGB:
					return castor::PixelFormat::eR8_UNORM;
				case castor::PixelFormat::eR8G8_SRGB:
					return castor::PixelFormat::eR8G8_UNORM;
				case castor::PixelFormat::eR8G8B8_SRGB:
					return castor::PixelFormat::eR8G8B8_UNORM;
				case castor::PixelFormat::eB8G8R8_SRGB:
					return castor::PixelFormat::eB8G8R8_UNORM;
				case castor::PixelFormat::eR8G8B8A8_SRGB:
					return castor::PixelFormat::eR8G8B8A8_UNORM;
				case castor::PixelFormat::eB8G8R8A8_SRGB:
					return castor::PixelFormat::eB8G8R8A8_UNORM;
				case castor::PixelFormat::eA8B8G8R8_SRGB:
					return castor::PixelFormat::eA8B8G8R8_UNORM;
				case castor::PixelFormat::eBC1_RGB_SRGB_BLOCK:
					return castor::PixelFormat::eBC1_RGB_UNORM_BLOCK;
				case castor::PixelFormat::eBC1_RGBA_SRGB_BLOCK:
					return castor::PixelFormat::eBC1_RGBA_UNORM_BLOCK;
				case castor::PixelFormat::eBC2_SRGB_BLOCK:
					return castor::PixelFormat::eBC2_UNORM_BLOCK;
				case castor::PixelFormat::eBC3_SRGB_BLOCK:
					return castor::PixelFormat::eBC3_UNORM_BLOCK;
				case castor::PixelFormat::eBC7_SRGB_BLOCK:
					return castor::PixelFormat::eBC7_UNORM_BLOCK;
				case castor::PixelFormat::eETC2_R8G8B8_SRGB_BLOCK:
					return castor::PixelFormat::eETC2_R8G8B8_UNORM_BLOCK;
				case castor::PixelFormat::eETC2_R8G8B8A1_SRGB_BLOCK:
					return castor::PixelFormat::eETC2_R8G8B8A1_UNORM_BLOCK;
				case castor::PixelFormat::eETC2_R8G8B8A8_SRGB_BLOCK:
					return castor::PixelFormat::eETC2_R8G8B8A8_UNORM_BLOCK;
				case castor::PixelFormat::eASTC_4x4_SRGB_BLOCK:
					return castor::PixelFormat::eASTC_4x4_UNORM_BLOCK;
				case castor::PixelFormat::eASTC_5x4_SRGB_BLOCK:
					return castor::PixelFormat::eASTC_5x4_UNORM_BLOCK;
				case castor::PixelFormat::eASTC_5x5_SRGB_BLOCK:
					return castor::PixelFormat::eASTC_5x5_UNORM_BLOCK;
				case castor::PixelFormat::eASTC_6x5_SRGB_BLOCK:
					return castor::PixelFormat::eASTC_6x5_UNORM_BLOCK;
				case castor::PixelFormat::eASTC_6x6_SRGB_BLOCK:
					return castor::PixelFormat::eASTC_6x6_UNORM_BLOCK;
				case castor::PixelFormat::eASTC_8x5_SRGB_BLOCK:
					return castor::PixelFormat::eASTC_8x5_UNORM_BLOCK;
				case castor::PixelFormat::eASTC_8x6_SRGB_BLOCK:
					return castor::PixelFormat::eASTC_8x6_SRGB_BLOCK;
				case castor::PixelFormat::eASTC_8x8_SRGB_BLOCK:
					return castor::PixelFormat::eASTC_8x8_UNORM_BLOCK;
				case castor::PixelFormat::eASTC_10x5_SRGB_BLOCK:
					return castor::PixelFormat::eASTC_10x5_UNORM_BLOCK;
				case castor::PixelFormat::eASTC_10x6_SRGB_BLOCK:
					return castor::PixelFormat::eASTC_10x6_UNORM_BLOCK;
				case castor::PixelFormat::eASTC_10x8_SRGB_BLOCK:
					return castor::PixelFormat::eASTC_10x8_UNORM_BLOCK;
				case castor::PixelFormat::eASTC_10x10_SRGB_BLOCK:
					return castor::PixelFormat::eASTC_10x10_UNORM_BLOCK;
				case castor::PixelFormat::eASTC_12x10_SRGB_BLOCK:
					return castor::PixelFormat::eASTC_12x10_UNORM_BLOCK;
				case castor::PixelFormat::eASTC_12x12_SRGB_BLOCK:
					return castor::PixelFormat::eASTC_12x12_UNORM_BLOCK;
				default:
					return format;
				}
			};
			pformat = get( pformat );
			CU_Require( !isSRGBFormat( pformat ) );
			return pformat;
		}

		castor::PxBufferBaseUPtr mergeBuffers( castor::PxBufferBaseUPtr lhsBuffer
			, uint32_t const & lhsSrcMask
			, uint32_t const & lhsDstMask
			, castor::PxBufferBaseUPtr rhsBuffer
			, uint32_t const & rhsSrcMask
			, uint32_t const & rhsDstMask
			, castor::String const & name )
		{
			// Resize images to max images dimensions, if needed.
			auto dimensions = lhsBuffer->getDimensions();

			if ( dimensions != rhsBuffer->getDimensions() )
			{
				dimensions.set( std::max( dimensions.getWidth(), rhsBuffer->getDimensions().getWidth() )
					, std::max( dimensions.getHeight(), rhsBuffer->getDimensions().getHeight() ) );

				if ( dimensions != lhsBuffer->getDimensions() )
				{
					log::debug << ( cuT( "Resizing LHS image.\n" ) );
					lhsBuffer = castor::Image::resample( dimensions
						, std::move( lhsBuffer ) );
				}

				if ( dimensions != rhsBuffer->getDimensions() )
				{
					log::debug << ( cuT( "Resizing RHS image.\n" ) );
					rhsBuffer = castor::Image::resample( dimensions
						, std::move( rhsBuffer ) );
				}
			}

			// Adjust the pixel formats to the most precise one
			auto pixelFormat = lhsBuffer->getFormat();
			auto rhsPixelFormat = rhsBuffer->getFormat();

			if ( isSRGBFormat( rhsPixelFormat ) )
			{
				pixelFormat = getSRGBFormat( pixelFormat );
			}
			else if ( isSRGBFormat( pixelFormat ) )
			{
				rhsPixelFormat = getSRGBFormat( rhsPixelFormat );
			}

			if ( pixelFormat != rhsBuffer->getFormat() )
			{
				if ( getBytesPerPixel( pixelFormat ) < getBytesPerPixel( rhsBuffer->getFormat() )
					|| ( !isFloatingPoint( pixelFormat ) && isFloatingPoint( rhsBuffer->getFormat() ) )
					)
				{
					pixelFormat = rhsBuffer->getFormat();
				}
			}

			// Merge the two buffers into one
			auto lhsComponents = getPixelComponents( lhsSrcMask );
			auto rhsComponents = getPixelComponents( rhsSrcMask );
			auto result = castor::PxBufferBase::createUnique( dimensions
				, getPixelFormat( pixelFormat, getPixelComponents( lhsDstMask | rhsDstMask ) ) );
			log::debug << cuT( "Copying LHS image components to result.\n" );
			copyBufferComponents( lhsComponents
				, getPixelComponents( lhsDstMask )
				, *lhsBuffer
				, *result );
			log::debug << cuT( "Copying RHS image components to result.\n" );
			copyBufferComponents( rhsComponents
				, getPixelComponents( rhsDstMask )
				, *rhsBuffer
				, *result );
			return result;
		}

		castor::PxBufferBaseUPtr adaptBuffer( castor::PxBufferBaseUPtr buffer )
		{
			auto dstFormat = buffer->getFormat();

			switch ( dstFormat )
			{
			case castor::PixelFormat::eR8G8B8_UNORM:
				dstFormat = castor::PixelFormat::eR8G8B8A8_UNORM;
				break;
			case castor::PixelFormat::eB8G8R8_UNORM:
				dstFormat = castor::PixelFormat::eA8B8G8R8_UNORM;
				break;
			case castor::PixelFormat::eR8G8_SRGB:
			case castor::PixelFormat::eR8G8B8_SRGB:
				dstFormat = castor::PixelFormat::eR8G8B8A8_SRGB;
				break;
			case castor::PixelFormat::eB8G8R8_SRGB:
				dstFormat = castor::PixelFormat::eA8B8G8R8_SRGB;
				break;
			case castor::PixelFormat::eR16G16B16_SFLOAT:
				dstFormat = castor::PixelFormat::eR16G16B16A16_SFLOAT;
				break;
			case castor::PixelFormat::eR32G32B32_SFLOAT:
				dstFormat = castor::PixelFormat::eR32G32B32A32_SFLOAT;
				break;
			default:
				// No conversion
				break;
			}

			if ( buffer->getFormat() != dstFormat )
			{
				auto flipped = buffer->isFlipped();
				buffer = castor::PxBufferBase::createUnique( buffer->getDimensions()
					, buffer->getLayers()
					, buffer->getLevels()
					, dstFormat
					, buffer->getConstPtr()
					, buffer->getFormat()
					, buffer->getAlign() );

				if ( flipped )
				{
					buffer->flip();
				}
			}

			return buffer;
		}

		bool allowSRGB( TextureConfiguration const & config )
		{
			auto flags = getFlags( config );
			return ( checkFlag( flags, TextureFlag::eColour )
					|| checkFlag( flags, TextureFlag::eEmissive ) );
		}

		castor::PxBufferBaseUPtr getFileImage( Engine & engine
			, castor::String const & name
			, castor::Path const & folder
			, castor::Path const & relative
			, TextureConfiguration const & config )
		{
			auto image = engine.getImageCache().tryFind( name );

			if ( !image.lock() )
			{
				image = engine.getImageCache().add( name
					, castor::ImageCreateParams{ folder / relative
						, { false, false, false } } );
			}

			auto img = image.lock();

			if ( !img )
			{
				CU_LoaderError( "Couldn't load image." );
			}

			auto format = ( ( isSRGBFormat( img->getPixels()->getFormat() ) && allowSRGB( config ) )
				? img->getPixels()->getFormat()
				: getNonSRGBFormat( img->getPixels()->getFormat() ) );
			auto buffer = castor::PxBufferBase::createUnique( img->getPixels()->getDimensions()
				, img->getPixels()->getLayers()
				, img->getPixels()->getLevels()
				, format
				, img->getPixels()->getConstPtr()
				, format
				, img->getPixels()->getAlign() );
			buffer = adaptBuffer( std::move( buffer ) );

			if ( !buffer )
			{
				CU_LoaderError( "Couldn't adapt image buffer." );
			}

			if ( img->getPixels()->isFlipped() )
			{
				buffer->flip();
			}

			return buffer;
		}

		TextureLayoutSPtr getTextureLayout( Engine & engine
			, castor::PxBufferBaseUPtr buffer
			, castor::String const & name
			, bool allowCompression
			, bool layersToTiles
			, uint32_t & layers )
		{
			// Finish buffer initialisation.
			auto & loader = engine.getImageLoader();
			auto compressedFormat = loader.getOptions().getCompressed( buffer->getFormat() );

			if ( compressedFormat != buffer->getFormat()
				&& allowCompression )
			{
				log::debug << ( name + cuT( " - Compressing result.\n" ) );
				buffer = castor::PxBufferBase::createUnique( &loader.getOptions()
					, buffer->getDimensions()
					, compressedFormat
					, buffer->getConstPtr()
					, buffer->getFormat() );
			}
			else if ( !castor::isCompressed( buffer->getFormat() ) )
			{
				log::debug << ( name + cuT( " - Generating result mipmaps.\n" ) );
				buffer->generateMips();
			}

			if ( layersToTiles )
			{
				layers = buffer->getLayers();
				buffer->convertToTiles( engine.getRenderSystem()->getProperties().limits.maxImageDimension2D );
			}

			// Create the resulting texture.
			return createTextureLayout( engine
				, name
				, std::move( buffer )
				, true );
		}

		size_t makeHash( TextureSourceInfo const & sourceInfo
			, PassTextureConfig const & config )
		{
			auto result = TextureSourceInfoHasher{}( sourceInfo );
			return result;
		}

		bool findUnit( Engine & engine
			, castor::CheckedMutex & loadMtx
			, std::unordered_map< size_t, TextureUnitSPtr > & loaded
			, TextureSourceInfo const & sourceInfo
			, PassTextureConfig const & config
			, TextureUnitSPtr & result )
		{
			auto hash = makeHash( sourceInfo, config );
			auto lock( makeUniqueLock( loadMtx ) );
			auto ires = loaded.emplace( hash, nullptr );
			auto it = ires.first;

			if ( ires.second )
			{
				it->second = std::make_shared< TextureUnit >( engine, sourceInfo );
				it->second->setConfiguration( config.config );
			}

			result = it->second;
			return ires.second;
		}

		bool hasElems( castor::CheckedMutex & loadMtx
			, std::vector< std::unique_ptr< TextureUnitCache::ThreadData > > & loading )
		{
			auto lock( castor::makeUniqueLock( loadMtx ) );
			return !loading.empty();
		}
	}

	//*********************************************************************************************

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
		mergeConfigsBase( lhs, rhs );
	}

	//*********************************************************************************************

	TextureUnitCache::TextureUnitCache( Engine & engine )
		: OwnedBy< Engine >{ engine }
	{
	}

	TextureUnitCache::~TextureUnitCache()
	{
	}

	void TextureUnitCache::notifyPassChange( Pass & pass )
	{
		auto lock( castor::makeUniqueLock( m_dirtyMtx ) );
		m_dirty.insert( &pass );
	}

	void TextureUnitCache::preparePass( Pass & pass )
	{
		auto lock( castor::makeUniqueLock( m_dirtyMtx ) );
		auto it = m_dirty.find( &pass );

		if ( it != m_dirty.end() )
		{
			pass.prepareTextures();
		}
	}

	void TextureUnitCache::clear()
	{
		while ( hasElems( m_loadMtx, m_loading ) )
		{
			std::this_thread::sleep_for( 1_ms );
		}

		auto dirtyLock( castor::makeUniqueLock( m_dirtyMtx ) );
		auto loadLock( castor::makeUniqueLock( m_loadMtx ) );
		m_dirty.clear();

		for ( auto & loaded : m_loaded )
		{
			loaded.second->cleanup();
		}

		m_loaded.clear();
		m_loading.clear();
	}

	TextureUnitSPtr TextureUnitCache::getTexture( TextureSourceInfo const & sourceInfo
		, PassTextureConfig const & config )
	{
		TextureUnitSPtr result{};

		if ( findUnit( *getEngine(), m_loadMtx, m_loaded, sourceInfo, config, result ) )
		{
			doLoadSource( sourceInfo, config, *result );
		}

		return result;
	}
	
	TextureUnitSPtr TextureUnitCache::mergeImages( TextureSourceInfo const & lhsSourceInfo
		, TextureConfiguration const & lhsConfig
		, uint32_t lhsSrcMask
		, uint32_t lhsDstMask
		, TextureSourceInfo const & rhsSourceInfo
		, TextureConfiguration const & rhsConfig
		, uint32_t rhsSrcMask
		, uint32_t rhsDstMask
		, castor::String const & name
		, TextureSourceInfo resultSourceInfo
		, TextureConfiguration resultConfig )
	{
		// Prepare the resulting texture configuration.
		mergeConfigsBase( lhsConfig, resultConfig );
		mergeConfigsBase( rhsConfig, resultConfig );

		PassTextureConfig passConfig{ { {} }, resultConfig };
		TextureUnitSPtr result{};

		if ( findUnit( *getEngine(), m_loadMtx, m_loaded, resultSourceInfo, passConfig, result ) )
		{
			doMergeSources( lhsSourceInfo
				, lhsConfig
				, lhsSrcMask
				, lhsDstMask
				, rhsSourceInfo
				, rhsConfig
				, rhsSrcMask
				, rhsDstMask
				, resultSourceInfo
				, passConfig
				, name
				, *result );
		}

		return result;
	}

	void TextureUnitCache::doCreateLayout( ThreadData & data
		, castor::String const & name )
	{
		data.layout = getTextureLayout( *getEngine()
			, std::move( data.buffer )
			, name
			, data.sourceInfo.allowCompression() && ( data.config.config.normalMask[0] == 0 )
			, data.sourceInfo.layersToTiles()
			, data.tiles );
	}

	void TextureUnitCache::doLoadSource( TextureSourceInfo const & sourceInfo
		, PassTextureConfig const & config
		, TextureUnit & unit )
	{
		auto & data = doCreateThreadData( sourceInfo, config, unit );

		if ( sourceInfo.isRenderTarget() )
		{
			getEngine()->pushGpuJob( [this, &data]( RenderDevice const & device
				, QueueData const & queueData )
				{
					data.unit->setSampler( data.sourceInfo.sampler() );
					data.unit->setRenderTarget( data.sourceInfo.renderTarget() );
					auto config = data.config.config;
					config.tileSet->z = data.tiles;
					config.tileSet->w = 1u;
					data.unit->setConfiguration( std::move( config ) );
					data.unit->initialise( device, queueData );
					doDestroyThreadData( data );
				} );
		}
		else
		{
			getEngine()->pushCpuJob( [this, &data]()
				{
					// Load CPU buffer on CPU thread
					data.buffer = getFileImage( *getEngine()
						, data.sourceInfo.relative()
						, data.sourceInfo.folder()
						, data.sourceInfo.relative()
						, data.config.config );
					doCreateLayout( data, data.sourceInfo.relative() );
					// On buffer load end, upload to VRAM on GPU thread.
					doUpload( data );
				} );
		}
	}

	void TextureUnitCache::doMergeSources( TextureSourceInfo const & lhsSourceInfo
		, TextureConfiguration const & lhsConfig
		, uint32_t lhsSrcMask
		, uint32_t lhsDstMask
		, TextureSourceInfo const & rhsSourceInfo
		, TextureConfiguration const & rhsConfig
		, uint32_t rhsSrcMask
		, uint32_t rhsDstMask
		, TextureSourceInfo const & resultSourceInfo
		, PassTextureConfig const & resultConfig
		, castor::String const & name
		, TextureUnit & unit )
	{
		auto & data = doCreateThreadData( resultSourceInfo, resultConfig, unit );
		getEngine()->pushCpuJob( [this, &data, name, lhsConfig, lhsSrcMask, lhsDstMask, lhsSourceInfo, rhsConfig, rhsSrcMask, rhsDstMask, rhsSourceInfo]()
			{
				// Merge CPU buffers on CPU thread.
				auto lhsImage = getFileImage( *getEngine()
					, lhsSourceInfo.relative()
					, lhsSourceInfo.folder()
					, lhsSourceInfo.relative()
					, lhsConfig );
				auto rhsImage = getFileImage( *getEngine()
					, rhsSourceInfo.relative()
					, rhsSourceInfo.folder()
					, rhsSourceInfo.relative()
					, rhsConfig );
				data.buffer = mergeBuffers( std::move( lhsImage )
					, lhsSrcMask
					, lhsDstMask
					, std::move( rhsImage )
					, rhsSrcMask
					, rhsDstMask
					, name );
				doCreateLayout( data, name );
				// On buffer load end, upload to VRAM on GPU thread.
				doUpload( data );
			} );
	}

	TextureUnitCache::ThreadData & TextureUnitCache::doCreateThreadData( TextureSourceInfo const & sourceInfo
		, PassTextureConfig const & config
		, TextureUnit & unit )
	{
		auto lock( castor::makeUniqueLock( m_loadMtx ) );
		m_loading.emplace_back( std::make_unique< ThreadData >( sourceInfo
			, config
			, unit ) );
		return *m_loading.back();
	}

	void TextureUnitCache::doDestroyThreadData( TextureUnitCache::ThreadData & data )
	{
		auto lock( castor::makeUniqueLock( m_loadMtx ) );
		auto it = std::find_if( m_loading.begin()
			, m_loading.end()
			, [&data]( auto & lookup )
			{
				return lookup.get() == ( &data );
			} );
		CU_Require( it != m_loading.end() );
		m_loading.erase( it );
	}

	void TextureUnitCache::doUpload( ThreadData & data )
	{
		getEngine()->pushGpuJob( [this, &data]( RenderDevice const & device
			, QueueData const & queueData )
			{
				data.unit->setSampler( data.sourceInfo.sampler() );
				data.unit->setTexture( data.layout );
				auto config = data.config.config;
				auto tiles = data.layout->getImage().getPixels()->getTiles();

				if ( config.tileSet->z <= 1 && tiles->x >= 1 )
				{
					config.tileSet->z = tiles->x;
				}

				if ( config.tileSet->w <= 1 && tiles->y >= 1 )
				{
					config.tileSet->w = tiles->y;
				}

				data.unit->setConfiguration( std::move( config ) );
				data.unit->initialise( device, queueData );
				doDestroyThreadData( data );
			} );
	}

	//*********************************************************************************************
}
