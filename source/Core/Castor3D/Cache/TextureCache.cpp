#include "Castor3D/Cache/TextureCache.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/UploadData.hpp"
#include "Castor3D/Event/Frame/CpuFunctorEvent.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Texture/TextureSourceInfo.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderTarget.hpp"

#include <CastorUtils/Miscellaneous/Hash.hpp>

CU_ImplementSmartPtr( castor3d, TextureUnitCache )

namespace castor3d
{
	using ashes::operator==;
	using ashes::operator!=;

	//*********************************************************************************************

	namespace cachetex
	{
		static castor::PixelFormat normaliseFormat( castor::PixelFormat format )
		{
			switch ( format )
			{
			case castor::PixelFormat::eR8G8B8_UNORM:
				return castor::PixelFormat::eR8G8B8A8_UNORM;
			case castor::PixelFormat::eB8G8R8_UNORM:
				return castor::PixelFormat::eA8B8G8R8_UNORM;
			case castor::PixelFormat::eR8G8_SRGB:
			case castor::PixelFormat::eR8G8B8_SRGB:
				return castor::PixelFormat::eR8G8B8A8_SRGB;
			case castor::PixelFormat::eB8G8R8_SRGB:
				return castor::PixelFormat::eA8B8G8R8_SRGB;
			case castor::PixelFormat::eR16G16B16_SFLOAT:
				return castor::PixelFormat::eR16G16B16A16_SFLOAT;
			case castor::PixelFormat::eR32G32B32_SFLOAT:
				return castor::PixelFormat::eR32G32B32A32_SFLOAT;
			default:
				return format;
			}
		}

		static castor::ImageRes adaptToTextureImage( Engine & engine
			, std::atomic_bool const & interrupted
			, castor::Image & image
			, TextureSourceInfo const & sourceInfo
			, uint32_t maxImageSize
			, bool generateMips )
		{
			auto name = image.getName();
			auto imagePixels = image.getPixels();

			// Normalise format, regarding SRGB/Linear space
			auto format = ( ( isSRGBFormat( imagePixels->getFormat() ) && sourceInfo.allowSRGB() )
				? imagePixels->getFormat()
				: getNonSRGBFormat( imagePixels->getFormat() ) );
			auto buffer = castor::PxBufferBase::create( imagePixels->getDimensions()
				, imagePixels->getLayers()
				, imagePixels->getLevels()
				, format
				, imagePixels->getConstPtr()
				, format
				, imagePixels->getAlign() );

			// Account for image size limit set in the engine (if possible).
			if ( !castor::isCompressed( image.getPixelFormat() ) )
			{
				auto maxDim = std::max( image.getWidth(), image.getHeight() );

				if ( maxDim == image.getWidth()
					&& image.getWidth() > maxImageSize )
				{
					auto ratio = float( maxImageSize ) / float( image.getWidth() );
					auto height = uint32_t( float( image.getHeight() ) * ratio );
					buffer = castor::Image::resample( { maxImageSize, height }, castor::move( buffer ) );
					name += cuT( "/WResampled" );
				}
				else if ( maxDim == image.getHeight()
					&& image.getHeight() > maxImageSize )
				{
					auto ratio = float( maxImageSize ) / float( image.getHeight() );
					auto width = uint32_t( float( image.getHeight() ) * ratio );
					buffer = castor::Image::resample( { width, maxImageSize }, castor::move( buffer ) );
					name += cuT( "/HResampled" );
				}
			}

			// Expand RGB to RGBA format
			if ( auto normalisedFormat = normaliseFormat( buffer->getFormat() );
				buffer->getFormat() != normalisedFormat )
			{
				log::debug << name << cuT( " - Converting RGB to RGBA.\n" );
				buffer = castor::PxBufferBase::create( buffer->getDimensions()
					, buffer->getLayers()
					, buffer->getLevels()
					, normalisedFormat
					, buffer->getConstPtr()
					, buffer->getFormat()
					, buffer->getAlign() );
				name += cuT( "/RGBA" );
			}

			// Convert from layered image to tiled image
			if ( sourceInfo.layersToTiles()
				&& buffer->getLayers() > 1u )
			{
				log::debug << name << cuT( " - Converting layers to tiles.\n" );
				buffer->convertToTiles( engine.getRenderSystem()->getProperties().limits.maxImageDimension2D );
				name += cuT( "/Tiled" );
			}

			// Generate mipmaps, if possible.
			if ( !castor::isCompressed( buffer->getFormat() )
				&& buffer->getWidth() > 1u
				&& buffer->getHeight() > 1u
				&& generateMips )
			{
				log::debug << name << cuT( " - Generating mipmaps.\n" );
				buffer->generateMips();
				name += cuT( "/Mipped" );
			}

			// Compress result.
			auto const & loader = engine.getImageLoader();
			auto compressedFormat = loader.getOptions().getCompressed( buffer->getFormat() );

			if ( auto compressedMinExtent = ashes::getMinimalExtent2D( convert( compressedFormat ) );
				compressedFormat != buffer->getFormat()
					&& buffer->getWidth() >= compressedMinExtent.width
					&& buffer->getHeight() >= compressedMinExtent.height
					&& sourceInfo.allowCompression() )
			{
				log::debug << name << cuT( " - Compressing.\n" );
				buffer = castor::PxBufferBase::create( &loader.getOptions()
					, &interrupted
					, buffer->getDimensions()
					, compressedFormat
					, buffer->getConstPtr()
					, buffer->getFormat() );
				name += cuT( "/Compressed" );
			}

			// Apply original buffer flip
			if ( imagePixels->isFlipped() )
			{
				buffer->flip();
			}

			castor::ImageLayout layout{ ( ( buffer->getLayers() == 1u && image.getLayout().type == castor::ImageLayout::e2DArray )
					? castor::ImageLayout::e2D
					: image.getLayout().type )
				, *buffer };
			return engine.createImage( name
				, image.getPath()
				, castor::move( layout )
				, castor::move( buffer ) );
		}

		static castor::ImageRes loadSource( Engine & engine
			, std::atomic_bool const & interrupted
			, TextureSourceInfo const & sourceInfo
			, bool generateMips )
		{
			return adaptToTextureImage( engine
				, interrupted
				, ( sourceInfo.isBufferImage()
					? getBufferImage( engine
						, sourceInfo.name()
						, sourceInfo.type()
						, sourceInfo.buffer() )
					: getFileImage( engine
						, sourceInfo.name()
						, sourceInfo.folder()
						, sourceInfo.relative() ) )
				, sourceInfo
				, engine.getMaxImageSize()
				, generateMips );
		}

		static size_t makeHash( TextureSourceInfo const & sourceInfo )
		{
			return TextureSourceInfoHasher{}( sourceInfo );
		}

		static size_t makeHash( TextureSourceInfo const & sourceInfo
			, PassTextureConfig const & passConfig )
		{
			auto result = TextureSourceInfoHasher{}( sourceInfo );
			return castor::hashCombine( result, PassTextureConfigHasher{}( passConfig ) );
		}

		static bool findUnit( Engine & engine
			, castor::CheckedMutex & loadMtx
			, castor::UnorderedMap< size_t, TextureUnitUPtr > & loaded
			, TextureUnitData & data
			, TextureUnitRPtr & result )
		{
			auto hash = makeHash( data.base->sourceInfo, data.passConfig );
			auto lock( makeUniqueLock( loadMtx ) );
			auto [it, res] = loaded.emplace( hash, nullptr );

			if ( res )
			{
				it->second = castor::makeUnique< TextureUnit >( engine, data );
				it->second->setConfiguration( data.base->sourceInfo.textureConfig() );
			}
			else
			{
				auto merged = data.base->sourceInfo.textureConfig();

				if ( it->second->isTextured() )
				{
					updateIndices( convert( it->second->getTexturePixelFormat() )
						, merged );
				}
				else
				{
					updateIndices( castor::PixelFormat::eR8G8B8A8_UNORM
						, merged );
				}

				if ( !shallowEqual( merged, it->second->getConfiguration() ) )
				{
					mergeConfigs( it->second->getConfiguration(), merged );
					it->second->setConfiguration( merged );
				}
			}

			result = it->second.get();
			return !res;
		}

		static bool hasElems( castor::CheckedMutex & loadMtx
			, castor::Vector< castor::RawUniquePtr< TextureUnitCache::ThreadData > > const & loading )
		{
			auto lock( castor::makeUniqueLock( loadMtx ) );
			return !loading.empty();
		}
	}

	//*********************************************************************************************

	TextureUnitCache::TextureUnitCache( Engine & engine
		, crg::ResourcesCache & resources )
		: OwnedBy< Engine >{ engine }
		, m_resources{ resources }
	{
		TextureCombine dummy{};
		registerTextureCombine( dummy );
	}

	TextureCombine TextureUnitCache::registerTextureCombine( Pass const & pass )
	{
		auto loadLock( castor::makeUniqueLock( m_loadMtx ) );
		TextureCombine result;
		result.configCount = pass.getTextureUnitsCount();

		for ( auto & unit : pass.getTextureUnits() )
		{
			for ( auto flag : getFlags( unit->getConfiguration() ) )
			{
				result.flags.emplace( flag );
			}
		}

		registerTextureCombine( result );
		return result;
	}

	TextureCombineID TextureUnitCache::registerTextureCombine( TextureCombine & combine )
	{
		if ( auto it = std::find( m_texturesCombines.begin()
				, m_texturesCombines.end()
				, combine );
			it != m_texturesCombines.end() )
		{
			combine.baseId = it->baseId;
			return it->baseId;
		}

		if ( m_texturesCombines.size() >= MaxTextureCombines )
		{
			CU_Failure( "Overflown texture combines count." );
			CU_Exception( "Overflown texture combines count." );
		}

		m_texturesCombines.push_back( combine );
		auto & result = m_texturesCombines.back();
		result.baseId = TextureCombineID( m_texturesCombines.size() );
		combine.baseId = result.baseId;
		return result.baseId;
	}

	TextureCombineID TextureUnitCache::getTextureCombineID( TextureCombine const & combine )const
	{
		if ( combine.baseId == 0u )
		{
			auto it = std::find( m_texturesCombines.begin()
				, m_texturesCombines.end()
				, combine );

			if ( it == m_texturesCombines.end() )
			{
				CU_Failure( "Textures combination was not registered." );
				CU_Exception( "Textures combination was not registered." );
			}

			return TextureCombineID( std::distance( m_texturesCombines.begin(), it ) + 1 );
		}

		CU_Require( combine.baseId <= m_texturesCombines.size() );
		return combine.baseId;
	}

	TextureCombine TextureUnitCache::getTextureCombine( TextureCombineID id )const
	{
		CU_Require( id <= m_texturesCombines.size() );

		if ( id == 0 )
		{
			CU_Failure( "Unexpected invalid texture combine type." );
			return TextureCombine{};
		}

		return m_texturesCombines[id - 1u];
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
		castor::Array< VkDescriptorBindingFlags, 1u > bindlessFlags{ ( VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT
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
		castor::Array< uint32_t, 1u > maxBindings{ device.getMaxBindlessSampled() - 1u };
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
			for ( auto const & [id, loaded] : m_loadedUnits )
			{
				loaded->cleanup();
			}

			for ( auto const & [id, loaded] : m_loaded )
			{
				loaded->destroy();
			}

			m_bindlessTexSet.reset();
			m_bindlessTexPool.reset();
			m_bindlessTexLayout.reset();
		}
	}

	void TextureUnitCache::stopLoad()
	{
		auto it = m_loading.begin();

		while ( it != m_loading.end() )
		{
			if ( auto const & data = *it;
				data->expected )
			{
				data->interrupted.exchange( true );
				++it;
			}
			else
			{
				it = m_loading.erase( it );
			}
		}
	}

	void TextureUnitCache::update( GpuUpdater & updater )
	{
		if ( !m_initialised
			|| !hasBindless() )
		{
			return;
		}

		castor::Vector< ashes::WriteDescriptorSet > tmp;
		{
			auto lock( castor::makeUniqueLock( m_dirtyWritesMtx ) );
			castor::swap( m_dirtyWrites, tmp );
		}

		if ( !tmp.empty() )
		{
			m_bindlessTexSet->updateBindings( tmp );
		}
	}

	void TextureUnitCache::upload( UploadData & uploader )
	{
		castor::Map< TextureData *, Texture * > toUpload;
		{
			auto lock( castor::makeUniqueLock( m_uploadMtx ) );
			toUpload = castor::move( m_toUpload );
		}

		for ( auto [data, texture] : toUpload )
		{
			uploader.pushUpload( data->image->getBuffer().data()
				, data->image->getBuffer().size()
				, *texture->image
				, data->image->getLayout()
				, texture->sampledViewId.data->info.subresourceRange
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
				, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT );

			for ( auto unit : doListTextureUnits( texture ) )
			{
				doAddWrite( *unit );
			}
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

		if ( auto it = m_dirty.find( &pass );
			it != m_dirty.end() )
		{
			pass.prepareTextures();
		}
	}

	void TextureUnitCache::clear()
	{
		for ( auto const & loading : m_loading )
		{
			loading->interrupted.exchange( true );
		}

		while ( cachetex::hasElems( m_loadMtx, m_loading ) )
		{
			std::this_thread::sleep_for( 1_ms );
		}

		auto dirtyLock( castor::makeUniqueLock( m_dirtyMtx ) );
		auto loadLock( castor::makeUniqueLock( m_loadMtx ) );
		m_dirty.clear();

		for ( auto const & [id, loaded] : m_loadedUnits )
		{
			loaded->cleanup();
		}

		for ( auto const & [id, loaded] : m_loaded )
		{
			loaded->destroy();
		}

		m_loadedUnits.clear();
		m_loaded.clear();
		m_loading.clear();
		m_dirtyWrites.clear();
		m_units.clear();
		m_unitDatas.clear();
		m_datas.clear();
		m_pendingUnits.clear();
		m_texturesCombines.clear();
	}

	TextureData & TextureUnitCache::getSourceData( TextureSourceInfo const & sourceInfo )
	{
		auto realSource = sourceInfo;

		if ( !realSource.isRenderTarget() )
		{
			realSource.allowSRGB( realSource.allowSRGB()
				&& checkFlag( sourceInfo.textureConfig().textureSpace, TextureSpace::eAllowSRGB ) );
			realSource.allowCompression( realSource.allowCompression()
				&& !checkFlag( sourceInfo.textureConfig().textureSpace, TextureSpace::eTangentSpace ) );
		}

		auto hash = cachetex::makeHash( realSource );
		auto [it, res] = m_datas.emplace( hash, nullptr );

		if ( res )
		{
			it->second = castor::makeUnique< TextureData >( realSource );

			if ( auto result = it->second.get();
				!result->sourceInfo.isRenderTarget() )
			{
				auto & data = doCreateThreadData( *result );
				getEngine()->pushCpuJob( [this, &data]()
					{
						auto image = cachetex::loadSource( *getEngine()
							, data.interrupted
							, data.data->sourceInfo
							, true );
						auto name = image->getName();
						log::info << "Loaded image [" << name << "] (" << *image << ")" << std::endl;
						data.data->image = getEngine()->tryFindImage( name );

						if ( !data.data->image )
						{
							data.data->image = getEngine()->addImage( name, image );
						}
					} );
			}
		}

		return *it->second;
	}

	TextureUnitRPtr TextureUnitCache::getTextureUnit( TextureUnitData & unitData )
	{
		TextureUnitRPtr result{};

		if ( !cachetex::findUnit( *getEngine(), m_loadMtx, m_loadedUnits, unitData, result ) )
		{
			doGetTexture( *unitData.base
				, [this, &unitData, result]( TextureData const & data
					, Texture const * texture
					, bool isCreated )
				{
					if ( !isCreated )
					{
						result->reportFailure();
						return;
					}

					auto config = unitData.base->sourceInfo.textureConfig();

					if ( data.sourceInfo.isRenderTarget() )
					{
						config.tileSet->z = 1u;
						config.tileSet->w = 1u;
						texture = &data.sourceInfo.renderTarget()->getTexture();
						std::atomic_bool isInitialised = false;
						auto renderTarget = unitData.base->sourceInfo.renderTarget();
						renderTarget->initialise( [&isInitialised]( RenderTarget const &, QueueData const & )
							{
								isInitialised = true;
							} );

						while ( !isInitialised )
						{
							std::this_thread::sleep_for( 5_ms );
						}
					}
					else
					{
						auto tiles = data.image->getPixels()->getTiles();

						if ( config.tileSet->z <= 1 && tiles->x >= 1 )
						{
							config.tileSet->z = tiles->x;
						}

						if ( config.tileSet->w <= 1 && tiles->y >= 1 )
						{
							config.tileSet->w = tiles->y;
						}
					}

					result->setSampler( unitData.passConfig.sampler );
					result->setTexture( texture );
					result->setConfiguration( castor::move( config ) );

					result->initialise();

					if ( data.sourceInfo.isRenderTarget() )
					{
						doAddWrite( *result );
					}
					else
					{
						auto & array = m_unitsToAdd.try_emplace( texture ).first->second;
						array.push_back( result );
					}
				} );
		}
		else
		{
			log::debug << "TextureCache: Reusing existing TextureUnitData for [" << unitData.base->sourceInfo.name() << "]" << std::endl;
		}

		return result;
	}

	TextureUnitData & TextureUnitCache::getSourceData( TextureSourceInfo const & sourceInfo
		, PassTextureConfig const & passConfig
		, TextureAnimationUPtr animation )
	{
		auto hash = cachetex::makeHash( sourceInfo, passConfig );
		auto [it, res] = m_unitDatas.emplace( hash, nullptr );

		if ( res )
		{
			auto & sourceData = getSourceData( sourceInfo );
			it->second = castor::makeUnique< TextureUnitData >( &sourceData
				, passConfig
				, castor::move( animation ) );
		}

		return *it->second;
	}

	Texture const * TextureUnitCache::doGetTexture( TextureData & data
		, castor::Function< void( TextureData const &, Texture const *, bool ) > const & onEndCpuLoad )
	{
		Texture * result{};
		bool wasFound{};
		{
			auto hash = cachetex::makeHash( data.sourceInfo );
			auto lock( castor::makeUniqueLock( m_loadMtx ) );
			auto [it, res] = m_loaded.try_emplace( hash );
			wasFound = !res;

			if ( !wasFound )
			{
				if ( data.sourceInfo.isRenderTarget() )
				{
					auto texture = &data.sourceInfo.renderTarget()->getTexture();

					if ( onEndCpuLoad )
					{
						onEndCpuLoad( data, texture, true );
					}

					return texture;
				}

				it->second = castor::makeUnique< Texture >();
			}

			result = it->second.get();
		}

		if ( !wasFound )
		{
			auto & threadData = doFindThreadData( data );
			threadData.texture = result;
			threadData.expected = true;
			getEngine()->pushCpuJob( [this, onEndCpuLoad, &threadData]()
				{
					// Wait for interruption or image CPU loading end
					while ( !threadData.interrupted
						&& threadData.data->image == nullptr )
					{
						std::this_thread::sleep_for( 1_ms );
					}

					if ( !threadData.interrupted )
					{
						try
						{
							auto lock( castor::makeUniqueLock( m_loadMtx ) );
							doInitTexture( threadData );

							if ( onEndCpuLoad )
							{
								onEndCpuLoad( *threadData.data, threadData.texture, true );
							}
						}
						catch ( std::exception & exc )
						{
							log::error << "Exception encountered while creating a texture image: " << exc.what() << std::endl;
							onEndCpuLoad( *threadData.data, threadData.texture, false );
						}
					}

					doDestroyThreadData( threadData );
				} );
		}
		else if ( onEndCpuLoad )
		{
			getEngine()->pushCpuJob( [this, result, &data, onEndCpuLoad]()
				{
					// Wait for full loading end
					while ( data.image == nullptr || !*result )
					{
						std::this_thread::sleep_for( 1_ms );
					}
					{
						auto lock( castor::makeUniqueLock( m_loadMtx ) );

						if ( onEndCpuLoad )
						{
							onEndCpuLoad( data, result, true );
						}
					}
				} );
		}

		return result;
	}

	void TextureUnitCache::doInitTexture( ThreadData & data )
	{
		if ( !data.interrupted )
		{
			auto const & device = *getEngine()->getRenderDevice();
			auto & layout = data.data->image->getLayout();
			*data.texture = Texture{ device
				, m_resources
				, data.data->image->getName()
				, 0u
				, VkExtent3D{ layout.extent->x, layout.extent->y, layout.extent->z }
				, layout.layers
				, layout.levels
				, convert( layout.format )
				, data.data->usage
				, nullptr };
			data.texture->create();
			{
				auto lock( castor::makeUniqueLock( m_uploadMtx ) );
				m_toUpload.emplace( data.data, data.texture );
			}
		}
	}

	TextureUnitCache::ThreadData & TextureUnitCache::doCreateThreadData( TextureData & data )
	{
		auto lock( castor::makeUniqueLock( m_loadMtx ) );
		m_loading.emplace_back( castor::make_unique< ThreadData >( data ) );
		return *m_loading.back();
	}

	TextureUnitCache::ThreadData & TextureUnitCache::doFindThreadData( TextureData & data )
	{
		auto lock( castor::makeUniqueLock( m_loadMtx ) );
		auto it = std::find_if( m_loading.begin()
			, m_loading.end()
			, [&data]( auto & lookup )
			{
				return lookup->data == ( &data );
			} );
		CU_Require( it != m_loading.end() );
		return **it;
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

		auto [it, res] = m_units.try_emplace( &unit );

		if ( res )
		{
			it->second = unit.onIdChanged.connect( [this]( TextureUnit const & tex )
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

	castor::Vector< TextureUnit * > TextureUnitCache::doListTextureUnits( Texture const * texture )
	{
		auto lock( castor::makeUniqueLock( m_loadMtx ) );
		auto it = m_unitsToAdd.find( texture );

		if ( it == m_unitsToAdd.end() )
		{
			return {};
		}

		auto result = castor::move( it->second );
		m_unitsToAdd.erase( it );
		return result;
	}

	//*********************************************************************************************
}
