#include "Castor3D/Cache/TextureCache.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Texture/TextureSourceInfo.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderTarget.hpp"

#include <CastorUtils/Miscellaneous/Hash.hpp>

namespace castor3d
{
	using ashes::operator==;
	using ashes::operator!=;

	//*********************************************************************************************

	namespace cachetex
	{
		static castor::PxBufferBaseUPtr mergeBuffers( castor::PxBufferBaseUPtr lhsBuffer
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

		static castor::PxBufferBaseUPtr adaptBuffer( castor::PxBufferBaseUPtr buffer )
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

		static castor::PxBufferBaseUPtr getImageBuffer( castor::Image & image
			, bool allowSRGB
			, uint32_t maxImageSize )
		{
			if ( !castor::isCompressed( image.getPixelFormat() ) )
			{
				auto maxDim = std::max( image.getWidth(), image.getHeight() );

				if ( maxDim == image.getWidth()
					&& image.getWidth() > maxImageSize )
				{
					auto ratio = float( maxImageSize ) / float( image.getWidth() );
					auto height = uint32_t( float( image.getHeight() ) * ratio );
					image.resample( { maxImageSize, height } );
				}
				else if ( maxDim == image.getHeight()
					&& image.getHeight() > maxImageSize )
				{
					auto ratio = float( maxImageSize ) / float( image.getHeight() );
					auto width = uint32_t( float( image.getHeight() ) * ratio );
					image.resample( { width, maxImageSize } );
				}
			}

			auto format = ( ( isSRGBFormat( image.getPixels()->getFormat() ) && allowSRGB )
				? image.getPixels()->getFormat()
				: getNonSRGBFormat( image.getPixels()->getFormat() ) );
			auto buffer = castor::PxBufferBase::createUnique( image.getPixels()->getDimensions()
				, image.getPixels()->getLayers()
				, image.getPixels()->getLevels()
				, format
				, image.getPixels()->getConstPtr()
				, format
				, image.getPixels()->getAlign() );
			buffer = adaptBuffer( std::move( buffer ) );

			if ( !buffer )
			{
				CU_LoaderError( "Couldn't adapt image buffer." );
			}

			if ( image.getPixels()->isFlipped() )
			{
				buffer->flip();
			}

			return buffer;
		}

		static castor::PxBufferBaseUPtr getBufferImage( Engine & engine
			, castor::String const & name
			, castor::String const & type
			, castor::ByteArray const & data
			, bool allowSRGB )
		{
			auto image = engine.getImageCache().tryFind( name );

			if ( !image.lock() )
			{
				image = engine.getImageCache().add( name
					, castor::ImageCreateParams{ type
						, data
						, { false, false, false } } );
			}

			auto img = image.lock();

			if ( !img )
			{
				CU_LoaderError( "Couldn't load image." );
			}

			return getImageBuffer( *img, allowSRGB, engine.getMaxImageSize() );
		}

		static castor::PxBufferBaseUPtr getFileImage( Engine & engine
			, castor::String const & name
			, castor::Path const & folder
			, castor::Path const & relative
			, bool allowSRGB )
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

			return getImageBuffer( *img, allowSRGB, engine.getMaxImageSize() );
		}

		static TextureLayoutSPtr getTextureLayout( Engine & engine
			, castor::PxBufferBaseUPtr buffer
			, castor::String const & name
			, bool allowCompression
			, bool layersToTiles
			, uint32_t & layers
			, std::atomic_bool & interrupted )
		{
			// Finish buffer initialisation.
			auto & loader = engine.getImageLoader();
			auto compressedFormat = loader.getOptions().getCompressed( buffer->getFormat() );

			if ( compressedFormat != buffer->getFormat()
				&& allowCompression )
			{
				log::debug << ( name + cuT( " - Compressing result.\n" ) );
				buffer = castor::PxBufferBase::createUnique( &loader.getOptions()
					, &interrupted
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

		static size_t makeHash( TextureSourceInfo const & sourceInfo
			, PassTextureConfig const & config )
		{
			auto result = TextureSourceInfoHasher{}( sourceInfo );
			auto flags = getFlags( config.config );

			for ( auto flag : flags )
			{
				castor::hashCombine( result, flag );
			}

			return result;
		}

		static bool findUnit( Engine & engine
			, castor::CheckedMutex & loadMtx
			, std::unordered_map< size_t, TextureUnitSPtr > & loaded
			, TextureUnitData & data
			, TextureUnitSPtr & result )
		{
			auto hash = makeHash( data.sourceInfo, data.passConfig );
			auto lock( makeUniqueLock( loadMtx ) );
			auto ires = loaded.emplace( hash, nullptr );
			auto it = ires.first;

			if ( ires.second )
			{
				it->second = std::make_shared< TextureUnit >( engine, data );
				it->second->setConfiguration( data.passConfig.config );
			}
			else
			{
				auto merged = data.passConfig.config;

				if ( it->second->getTexture() )
				{
					updateIndices( convert( it->second->getTexture()->getPixelFormat() )
						, merged );
				}
				else
				{
					updateIndices( castor::PixelFormat::eR8G8B8A8_UNORM
						, merged );
				}

				if ( merged != it->second->getConfiguration() )
				{
					mergeConfigs( it->second->getConfiguration(), merged );
					it->second->setConfiguration( merged );
				}
			}

			result = it->second;
			return !ires.second;
		}

		static bool hasElems( castor::CheckedMutex & loadMtx
			, std::vector< std::unique_ptr< TextureUnitCache::ThreadData > > & loading )
		{
			auto lock( castor::makeUniqueLock( loadMtx ) );
			return !loading.empty();
		}

		static castor::PxBufferBaseUPtr loadSource( Engine & engine
			, TextureSourceInfo const & sourceInfo
			, bool allowSRGB )
		{
			return sourceInfo.isBufferImage()
				? cachetex::getBufferImage( engine
					, sourceInfo.name()
					, sourceInfo.type()
					, sourceInfo.buffer()
					, allowSRGB )
				: cachetex::getFileImage( engine
					, sourceInfo.relative()
					, sourceInfo.folder()
					, sourceInfo.relative()
					, allowSRGB );
		}
	}

	//*********************************************************************************************

	castor::PixelFormat getSRGBFormat( castor::PixelFormat format )
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
	}

	castor::PixelFormat getNonSRGBFormat( castor::PixelFormat format )
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
	}

	//*********************************************************************************************

	TextureUnitCache::TextureUnitCache( Engine & engine )
		: OwnedBy< Engine >{ engine }
	{
	}

	TextureUnitCache::~TextureUnitCache()
	{
	}

	void TextureUnitCache::initialise( RenderDevice const & device )
	{
		if ( !device.hasBindless()
			|| m_initialised )
		{
			return;
		}

		// Bindless textures initialisation
		// Descriptor layout first.
		std::array< VkDescriptorBindingFlags, 1u > bindlessFlags{ ( VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT
				| VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT
				| VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT ) };
		auto extendedInfo = makeVkStruct< VkDescriptorSetLayoutBindingFlagsCreateInfo >( uint32_t( bindlessFlags.size() )
			, bindlessFlags.data() );
		ashes::VkDescriptorSetLayoutBindingArray layoutBindings;
		layoutBindings.emplace_back( makeDescriptorSetLayoutBinding( 0u
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VK_SHADER_STAGE_ALL
			, device.getMaxBindlessSampled() ) );
		m_bindlessTexLayout = device->createDescriptorSetLayout( "SceneRenderNodesBindless"
			, makeVkStructPNext< VkDescriptorSetLayoutCreateInfo >( &extendedInfo
				, VkDescriptorSetLayoutCreateFlags( VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT )
				, uint32_t( layoutBindings.size() )
				, layoutBindings.data() ) );

		// Then the descriptor pool.
		ashes::VkDescriptorPoolSizeArray poolSizes;
		poolSizes.push_back( { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, device.getMaxBindlessSampled() } );
		m_bindlessTexPool = device->createDescriptorPool( VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT
			, device.getMaxBindlessSampled()
			, poolSizes );

		// And the descriptor set.
		std::array< uint32_t, 1u > maxBindings{ device.getMaxBindlessSampled() - 1u };
		auto countInfo = makeVkStruct< VkDescriptorSetVariableDescriptorCountAllocateInfo >( uint32_t( maxBindings.size() )
			, maxBindings.data() );
		m_bindlessTexSet = m_bindlessTexPool->createDescriptorSet( "SceneRenderNodesTextures"
			, *m_bindlessTexLayout
			, RenderPipeline::eTextures
			, &countInfo );

		m_initialised = true;

		for ( auto unit : m_pendingUnits )
		{
			doAddWrite( *unit );
		}
	}

	void TextureUnitCache::cleanup()
	{
		if ( m_initialised.exchange( false ) )
		{
			m_bindlessTexSet.reset();
			m_bindlessTexPool.reset();
			m_bindlessTexLayout.reset();
		}
	}

	void TextureUnitCache::stopLoad()
	{
		for ( auto & data : m_loading )
		{
			data->interrupted.exchange( true );
		}
	}

	void TextureUnitCache::update( GpuUpdater & updater )
	{
		if ( !m_initialised
			|| !hasBindless() )
		{
			return;
		}

		std::vector< ashes::WriteDescriptorSet > tmp;
		{
			auto lock( castor::makeUniqueLock( m_dirtyWritesMtx ) );
			std::swap( m_dirtyWrites, tmp );
		}

		if ( !tmp.empty() )
		{
			m_bindlessTexSet->updateBindings( tmp );
		}
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
		while ( cachetex::hasElems( m_loadMtx, m_loading ) )
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

	TextureUnitSPtr TextureUnitCache::getTexture( TextureUnitData & unitData )
	{
		TextureUnitSPtr result{};

		if ( !cachetex::findUnit( *getEngine(), m_loadMtx, m_loaded, unitData, result ) )
		{
			auto & data = doCreateThreadData( unitData, *result );

			if ( data.data->sourceInfo.isRenderTarget() )
			{
				getEngine()->sendEvent( makeGpuFunctorEvent( EventType::ePreRender
					, [this, &data]( RenderDevice const & device
						, QueueData const & queueData )
						{
							if ( !data.interrupted )
							{
								data.unit->setSampler( data.data->sourceInfo.sampler() );
								data.unit->setRenderTarget( data.data->sourceInfo.renderTarget() );
								auto config = data.data->passConfig.config;
								config.tileSet->z = data.tiles;
								config.tileSet->w = 1u;
								data.unit->setConfiguration( std::move( config ) );
								data.unit->initialise( device, queueData );
								doAddWrite( *data.unit );
							}

							doDestroyThreadData( data );
						} ) );
			}
			else
			{
				getEngine()->pushCpuJob( [this, &data]()
					{
						while ( !data.interrupted
							&& data.data->buffer == nullptr )
						{
							std::this_thread::sleep_for( 1_ms );
						}

						if ( !data.interrupted )
						{
							doCreateLayout( data
								, ( data.data->sourceInfo.isBufferImage()
									? data.data->sourceInfo.name()
									: castor::String{ data.data->sourceInfo.relative() } ) );
							// On buffer load end, upload to VRAM on GPU thread.
							doUpload( data );
						}
						else
						{
							doDestroyThreadData( data );
						}
					} );
			}
		}

		return result;
	}

	TextureUnitData & TextureUnitCache::getSourceData( TextureSourceInfo const & sourceInfo
		, PassTextureConfig const & passConfig
		, AnimationUPtr animation )
	{
		auto hash = cachetex::makeHash( sourceInfo, passConfig );
		auto ires = m_datas.emplace( hash, nullptr );
		auto it = ires.first;

		if ( ires.second )
		{
			bool allowSRGB = checkFlag( passConfig.config.textureSpace, TextureSpace::eAllowSRGB );
			it->second = castor::makeUnique< TextureUnitData >( sourceInfo
				, passConfig
				, std::move( animation ) );
			auto result = it->second.get();

			if ( result->sourceInfo.isBufferImage()
				|| result->sourceInfo.isFileImage() )
			{
				getEngine()->pushCpuJob( [this, result, allowSRGB]()
					{
						result->buffer = cachetex::loadSource( *getEngine(), result->sourceInfo, allowSRGB );
					} );
			}
		}

		return *it->second;
	}
	
	TextureUnitData & TextureUnitCache::mergeSources( TextureSourceInfo const & lhsSourceInfo
		, PassTextureConfig const & lhsPassConfig
		, uint32_t lhsSrcMask
		, uint32_t lhsDstMask
		, TextureSourceInfo const & rhsSourceInfo
		, PassTextureConfig const & rhsPassConfig
		, uint32_t rhsSrcMask
		, uint32_t rhsDstMask
		, castor::String const & name
		, TextureSourceInfo const & resultSourceInfo
		, PassTextureConfig const & resultPassConfig )
	{
		auto hash = cachetex::makeHash( resultSourceInfo, resultPassConfig );
		auto ires = m_datas.emplace( hash, nullptr );
		auto it = ires.first;

		if ( ires.second )
		{
			it->second = castor::makeUnique< TextureUnitData >( resultSourceInfo
				, resultPassConfig
				, nullptr );
			auto result = it->second.get();
			bool lhsAllowSRGB = checkFlag( lhsPassConfig.config.textureSpace, TextureSpace::eAllowSRGB );
			bool rhsAllowSRGB = checkFlag( rhsPassConfig.config.textureSpace, TextureSpace::eAllowSRGB );

			getEngine()->pushCpuJob( [this, result, name, lhsAllowSRGB, lhsSrcMask, lhsDstMask, lhsSourceInfo, rhsAllowSRGB, rhsSrcMask, rhsDstMask, rhsSourceInfo]()
				{
					// Merge CPU buffers on CPU thread.
					auto lhsImage = cachetex::loadSource( *getEngine(), lhsSourceInfo, lhsAllowSRGB );
					auto rhsImage = cachetex::loadSource( *getEngine(), rhsSourceInfo, rhsAllowSRGB );
					result->buffer = cachetex::mergeBuffers( std::move( lhsImage )
						, lhsSrcMask
						, lhsDstMask
						, std::move( rhsImage )
						, rhsSrcMask
						, rhsDstMask
						, name );
				} );
		}

		return *it->second;
	}

	void TextureUnitCache::doCreateLayout( ThreadData & data
		, castor::String const & name )
	{
		data.layout = cachetex::getTextureLayout( *getEngine()
			, std::move( data.data->buffer )
			, name
			, data.data->sourceInfo.allowCompression() && !checkFlag( data.data->passConfig.config.textureSpace, TextureSpace::eTangentSpace )
			, data.data->sourceInfo.layersToTiles()
			, data.tiles
			, data.interrupted );
	}

	TextureUnitCache::ThreadData & TextureUnitCache::doCreateThreadData( TextureUnitData & data
		, TextureUnit & unit )
	{
		auto lock( castor::makeUniqueLock( m_loadMtx ) );
		m_loading.emplace_back( std::make_unique< ThreadData >( data, unit ) );
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
		getEngine()->sendEvent( makeGpuFunctorEvent( EventType::ePreRender
			, [this, &data]( RenderDevice const & device
				, QueueData const & queueData )
				{
					if ( !data.interrupted )
					{
						data.unit->setSampler( data.data->sourceInfo.sampler() );
						data.unit->setTexture( data.layout );
						auto config = data.data->passConfig.config;
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
						doAddWrite( *data.unit );
					}

					doDestroyThreadData( data );
				} ) );
	}

	void TextureUnitCache::doAddWrite( TextureUnit & unit )
	{
		if ( !m_initialised )
		{
			m_pendingUnits.push_back( &unit );
			return;
		}

		if ( !hasBindless() )
		{
			return;
		}

		auto ires = m_units.emplace( &unit, OnTextureUnitChangedConnection{} );

		if ( ires.second )
		{
			ires.first->second = unit.onIdChanged.connect( [this]( TextureUnit const & tex )
				{
					if ( tex.getId() )
					{
						doUpdateWrite( tex );
					}
				} );
		}

		if ( unit.getId() != 0u )
		{
			doUpdateWrite( unit );
		}
	}

	void TextureUnitCache::doUpdateWrite( TextureUnit const & unit )
	{
		auto write = unit.getDescriptor();
		write->dstArrayElement = unit.getId() - 1u;
		write->dstSet = *m_bindlessTexSet;
		write->dstBinding = 0u;
		{
			auto lock( castor::makeUniqueLock( m_dirtyWritesMtx ) );
			m_dirtyWrites.emplace_back( write );
		}
	}

	//*********************************************************************************************
}
