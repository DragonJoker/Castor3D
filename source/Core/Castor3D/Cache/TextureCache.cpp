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

		TextureSourceInfo mergeSourceInfos( TextureSourceInfo const & lhs
			, TextureSourceInfo const & rhs )
		{
			return TextureSourceInfo{ lhs.sampler()
				, lhs.folder()
				, castor::Path{ lhs.relative() + rhs.relative() }
				, lhs.allowCompression()
				, lhs.generateMips() };
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

			if ( pixelFormat != rhsBuffer->getFormat() )
			{
				if ( getBytesPerPixel( pixelFormat ) < getBytesPerPixel( rhsBuffer->getFormat() )
					|| ( !isFloatingPoint( pixelFormat )
						&& isFloatingPoint( rhsBuffer->getFormat() ) )
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

		castor::PxBufferBaseUPtr getFileImage( Engine & engine
			, castor::String const & name
			, castor::Path const & folder
			, castor::Path const & relative )
		{
			auto image = engine.getImageCache().tryFind( name );

			if ( !image.lock() )
			{
				image = engine.getImageCache().add( name
					, castor::ImageCreateParams{ folder / relative
						, false
						, false } );
			}

			auto img = image.lock();

			if ( !img )
			{
				CU_LoaderError( "Couldn't load image." );
			}

			auto buffer = castor::PxBufferBase::createUnique( img->getPixels()->getDimensions()
				, img->getPixels()->getLayers()
				, img->getPixels()->getLevels()
				, img->getPixels()->getFormat()
				, img->getPixels()->getConstPtr()
				, img->getPixels()->getFormat()
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
				it->second = std::make_shared< TextureUnit >( engine );
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
		mergeMasks( lhs.needsGammaCorrection, rhs.needsGammaCorrection );
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
		, TextureConfiguration resultConfig )
	{
		// Prepare the resulting texture configuration.
		auto resultInfo = mergeSourceInfos( lhsSourceInfo, rhsSourceInfo );
		mergeConfigsBase( lhsConfig, resultConfig );
		mergeConfigsBase( rhsConfig, resultConfig );

		PassTextureConfig passConfig{ { {} }, resultConfig, {} };
		TextureUnitSPtr result{};

		if ( findUnit( *getEngine(), m_loadMtx, m_loaded, resultInfo, passConfig, result ) )
		{
			doMergeSources( lhsSourceInfo
				, lhsConfig
				, lhsSrcMask
				, lhsDstMask
				, rhsSourceInfo
				, rhsConfig
				, rhsSrcMask
				, rhsDstMask
				, resultInfo
				, passConfig
				, name
				, *result );
		}

		return result;
	}

	void TextureUnitCache::doCreateLayout( ThreadData & data
		, castor::String const & name )
	{
		auto & configuration = data.config.config;
		// GTH - On upload end, set registered unit content.
		if ( data.sourceInfo.renderTarget() )
		{
			configuration.needsGammaCorrection = 0u;
		}
		else if ( configuration.colourMask[0]
			|| configuration.emissiveMask[0] )
		{
			auto format = data.buffer->getFormat();
			configuration.needsGammaCorrection = !isFloatingPoint( format );
		}

		data.layout = getTextureLayout( *getEngine()
			, std::move( data.buffer )
			, name
			, data.sourceInfo.allowCompression() && ( data.config.config.normalMask[0] == 0 ) );
	}

	void TextureUnitCache::doLoadSource( TextureSourceInfo const & sourceInfo
		, PassTextureConfig const & config
		, TextureUnit & unit )
	{
		auto & data = doCreateThreadData( sourceInfo, config, unit );

		if ( sourceInfo.renderTarget() )
		{
			getEngine()->pushGpuJob( [this, &data]( RenderDevice const & device
				, QueueData const & queueData )
				{
					data.unit->setSampler( data.sourceInfo.sampler() );
					data.unit->setRenderTarget( data.sourceInfo.renderTarget() );
					data.unit->setConfiguration( data.config.config );
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
						, data.sourceInfo.relative() );
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
		getEngine()->pushCpuJob( [this, &data, name, lhsSrcMask, lhsDstMask, lhsSourceInfo, rhsSrcMask, rhsDstMask, rhsSourceInfo]()
			{
				// Merge CPU buffers on CPU thread.
				auto lhsImage = getFileImage( *getEngine()
					, lhsSourceInfo.relative()
					, lhsSourceInfo.folder()
					, lhsSourceInfo.relative() );
				auto rhsImage = getFileImage( *getEngine()
					, rhsSourceInfo.relative()
					, rhsSourceInfo.folder()
					, rhsSourceInfo.relative() );
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
				data.unit->setConfiguration( data.config.config );
				data.unit->initialise( device, queueData );
				doDestroyThreadData( data );
			} );
	}

	//*********************************************************************************************
}
