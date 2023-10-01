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
		static bool isMergeable( TextureSourceInfo const & source
			, VkFormat format
			, bool hasAnimation )
		{
			return !source.isRenderTarget()
				&& !ashes::isCompressedFormat( format )
				&& !hasAnimation;
		}

		static bool areFormatsCompatible( VkFormat lhs, VkFormat rhs )
		{
			if ( ashes::isCompressedFormat( lhs )
				|| ashes::isCompressedFormat( rhs ) )
			{
				return false;
			}

			if ( isSRGBFormat( convert( rhs ) ) )
			{
				return isSRGBFormat( getSRGBFormat( convert( lhs ) ) );
			}

			if ( isSRGBFormat( convert( lhs ) ) )
			{
				return isSRGBFormat( getSRGBFormat( convert( rhs ) ) );
			}

			return true;
		}

		static bool areSpacesCompatible( TextureSpaces lhs, TextureSpaces rhs )
		{
			return checkFlag( lhs, TextureSpace::eAllowSRGB ) == checkFlag( rhs, TextureSpace::eAllowSRGB )
				&& checkFlag( lhs, TextureSpace::eNormalised ) == checkFlag( rhs, TextureSpace::eNormalised );
		}

		static castor::ImageRPtr mergeBuffers( Engine & engine
			, std::atomic_bool & interrupted
			, castor::Image const & lhsImage
			, uint32_t const & lhsSrcMask
			, uint32_t const & lhsDstMask
			, castor::Image const & rhsImage
			, uint32_t const & rhsSrcMask
			, uint32_t const & rhsDstMask
			, castor::String const & name )
		{
			auto lhsBuffer = lhsImage.getPixels()->clone();
			auto rhsBuffer = rhsImage.getPixels()->clone();

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

			auto pixelFormat = lhsBuffer->getFormat();
			auto rhsPixelFormat = rhsBuffer->getFormat();

			// Favor SRGB format against linear (@TODO: change that ?)
			if ( isSRGBFormat( rhsPixelFormat ) )
			{
				pixelFormat = getSRGBFormat( pixelFormat );
			}
			else if ( isSRGBFormat( pixelFormat ) )
			{
				rhsPixelFormat = getSRGBFormat( rhsPixelFormat );
			}

			// Adjust the pixel formats to the most precise one
			auto lhsComponentFormat = getSingleComponent( pixelFormat );
			auto rhsComponentFormat = getSingleComponent( rhsPixelFormat );

			if ( lhsComponentFormat != rhsComponentFormat )
			{
				if ( getBytesPerPixel( lhsComponentFormat ) > getBytesPerPixel( rhsComponentFormat )
					|| ( !isFloatingPoint( pixelFormat ) && isFloatingPoint( rhsPixelFormat ) ) )
				{
					pixelFormat = getPixelFormat( rhsComponentFormat
						, getPixelComponents( lhsDstMask | rhsDstMask ) );
					lhsBuffer = castor::PxBufferBase::create( dimensions
						, getPixelFormat( rhsComponentFormat, getComponents( lhsBuffer->getFormat() ) )
						, lhsBuffer->getConstPtr()
						, lhsBuffer->getFormat()
						, lhsBuffer->getAlign() );
				}
				else
				{
					pixelFormat = getPixelFormat( lhsComponentFormat
						, getPixelComponents( lhsDstMask | rhsDstMask ) );
					rhsBuffer = castor::PxBufferBase::create( dimensions
						, getPixelFormat( lhsComponentFormat, getComponents( rhsBuffer->getFormat() ) )
						, rhsBuffer->getConstPtr()
						, rhsBuffer->getFormat()
						, rhsBuffer->getAlign() );
				}
			}
			else
			{
				pixelFormat = getPixelFormat( lhsComponentFormat
					, getPixelComponents( lhsDstMask | rhsDstMask ) );
			}

			// Merge the two buffers into one
			auto lhsComponents = getPixelComponents( lhsSrcMask );
			auto rhsComponents = getPixelComponents( rhsSrcMask );
			auto result = castor::PxBufferBase::create( dimensions
				, pixelFormat );
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
			result->generateMips();
			castor::ImageLayout layout{ lhsImage.getLayout().type, *result };
			log::info << "Loaded image [" << name << "] (" << layout << ")" << std::endl;
			return engine.addNewImage( name
				, castor::Path{}
				, std::move( layout )
				, std::move( result ) );
		}

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
			, std::atomic_bool & interrupted
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
					buffer = castor::Image::resample( { maxImageSize, height }, std::move( buffer ) );
					name += "/WResampled";
				}
				else if ( maxDim == image.getHeight()
					&& image.getHeight() > maxImageSize )
				{
					auto ratio = float( maxImageSize ) / float( image.getHeight() );
					auto width = uint32_t( float( image.getHeight() ) * ratio );
					buffer = castor::Image::resample( { width, maxImageSize }, std::move( buffer ) );
					name += "/HResampled";
				}
			}

			// Expand RGB to RGBA format
			auto normalisedFormat = normaliseFormat( buffer->getFormat() );

			if ( buffer->getFormat() != normalisedFormat )
			{
				log::debug << name << cuT( " - Converting RGB to RGBA.\n" );
				buffer = castor::PxBufferBase::create( buffer->getDimensions()
					, buffer->getLayers()
					, buffer->getLevels()
					, normalisedFormat
					, buffer->getConstPtr()
					, buffer->getFormat()
					, buffer->getAlign() );
				name += "/RGBA";
			}

			// Convert from layered image to tiled image
			if ( sourceInfo.layersToTiles()
				&& buffer->getLayers() > 1u )
			{
				log::debug << name << cuT( " - Converting layers to tiles.\n" );
				buffer->convertToTiles( engine.getRenderSystem()->getProperties().limits.maxImageDimension2D );
				name += "/Tiled";
			}

			// Generate mipmaps, if possible.
			if ( !castor::isCompressed( buffer->getFormat() )
				&& buffer->getWidth() > 1u
				&& buffer->getHeight() > 1u
				&& generateMips )
			{
				log::debug << name << cuT( " - Generating mipmaps.\n" );
				buffer->generateMips();
				name += "/Mipped";
			}

			// Compress result.
			auto & loader = engine.getImageLoader();
			auto compressedFormat = loader.getOptions().getCompressed( buffer->getFormat() );
			auto compressedMinExtent = ashes::getMinimalExtent2D( convert( compressedFormat ) );

			if ( compressedFormat != buffer->getFormat()
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
				name += "/Compressed";
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
				, std::move( layout )
				, std::move( buffer ) );
		}

		static castor::ImageRes loadSource( Engine & engine
			, std::atomic_bool & interrupted
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

		static bool findUnit( Engine & engine
			, castor::CheckedMutex & loadMtx
			, std::unordered_map< size_t, TextureUnitUPtr > & loaded
			, TextureUnitData & data
			, TextureUnitRPtr & result )
		{
			auto hash = makeHash( data.base->sourceInfo );
			auto lock( makeUniqueLock( loadMtx ) );
			auto ires = loaded.emplace( hash, nullptr );
			auto it = ires.first;

			if ( ires.second )
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
			return !ires.second;
		}

		static bool hasElems( castor::CheckedMutex & loadMtx
			, std::vector< std::unique_ptr< TextureUnitCache::ThreadData > > & loading )
		{
			auto lock( castor::makeUniqueLock( loadMtx ) );
			return !loading.empty();
		}

		static TextureCombineID addCombine( TextureCombine combine
			, std::vector< TextureCombine > & result )
		{
			result.push_back( std::move( combine ) );
			return TextureCombineID( result.size() );
		}
	}

	//*********************************************************************************************

	TextureUnitCache::TextureUnitCache( Engine & engine
		, crg::ResourcesCache & resources )
		: OwnedBy< Engine >{ engine }
		, m_resources{ resources }
	{
	}

	TextureUnitCache::~TextureUnitCache()
	{
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

	TextureCombineID TextureUnitCache::registerTextureCombine( TextureCombine combine )
	{
		auto it = std::find( m_texturesCombines.begin()
			, m_texturesCombines.end()
			, combine );

		if ( it == m_texturesCombines.end() )
		{
			auto idx = cachetex::addCombine( std::move( combine ), m_texturesCombines );
			it = std::next( m_texturesCombines.begin(), idx );
		}

		return TextureCombineID( std::distance( m_texturesCombines.begin(), it ) + 1 );
	}

	TextureCombineID TextureUnitCache::getTextureCombineID( TextureCombine const & combine )const
	{
		auto it = std::find( m_texturesCombines.begin()
			, m_texturesCombines.end()
			, combine );
		return it == m_texturesCombines.end()
			? cachetex::addCombine( combine, m_texturesCombines )
			: TextureCombineID( std::distance( m_texturesCombines.begin(), it ) + 1 );
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
			for ( auto & loaded : m_loadedUnits )
			{
				loaded.second->cleanup();
			}

			for ( auto & loaded : m_loaded )
			{
				loaded.second->destroy();
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
			auto & data = *it;

			if ( data->expected )
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

	void TextureUnitCache::upload( UploadData & uploader )
	{
		std::map< TextureData *, Texture * > toUpload;
		{
			auto lock( castor::makeUniqueLock( m_uploadMtx ) );
			toUpload = std::move( m_toUpload );
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
		auto it = m_dirty.find( &pass );

		if ( it != m_dirty.end() )
		{
			pass.prepareTextures();
		}
	}

	void TextureUnitCache::clear()
	{
		for ( auto & loading : m_loading )
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

		for ( auto & loaded : m_loadedUnits )
		{
			loaded.second->cleanup();
		}

		for ( auto & loaded : m_loaded )
		{
			loaded.second->destroy();
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
		realSource.allowSRGB( realSource.allowSRGB()
			&& checkFlag( sourceInfo.textureConfig().textureSpace, TextureSpace::eAllowSRGB ) );
		realSource.allowCompression( realSource.allowCompression()
			&& !checkFlag( sourceInfo.textureConfig().textureSpace, TextureSpace::eTangentSpace ) );
		auto hash = cachetex::makeHash( realSource );
		auto ires = m_datas.emplace( hash, nullptr );

		if ( ires.second )
		{
			ires.first->second = castor::makeUnique< TextureData >( realSource );
			auto result = ires.first->second.get();

			if ( !result->sourceInfo.isRenderTarget() )
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

		return *ires.first->second;
	}

	TextureUnitRPtr TextureUnitCache::getTextureUnit( TextureUnitData & unitData )
	{
		TextureUnitRPtr result{};

		if ( !cachetex::findUnit( *getEngine(), m_loadMtx, m_loadedUnits, unitData, result ) )
		{
			doGetTexture( *unitData.base
				, [this, &unitData, result]( TextureData const & data
					, Texture const * texture )
				{
					auto config = unitData.base->sourceInfo.textureConfig();

					if ( data.sourceInfo.isRenderTarget() )
					{
						config.tileSet->z = 1u;
						config.tileSet->w = 1u;
						texture = &data.sourceInfo.renderTarget()->getTexture();
						std::atomic_bool isInitialised = false;
						auto renderTarget = unitData.base->sourceInfo.renderTarget();
						renderTarget->initialise( [&isInitialised]( RenderTarget const & rt, QueueData const & queue )
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
					result->setConfiguration( std::move( config ) );

					result->initialise();

					if ( data.sourceInfo.isRenderTarget() )
					{
						doAddWrite( *result );
					}
					else
					{
						auto & array = m_unitsToAdd.emplace( texture, std::vector< TextureUnit * >{} ).first->second;
						array.push_back( result );
					}
				} );
		}
		else
		{
			log::info << "TextureCache: Reusing existing TextureUnitData for [" << unitData.base->sourceInfo.name() << "]" << std::endl;
		}

		return result;
	}

	TextureUnitData & TextureUnitCache::getSourceData( TextureSourceInfo const & sourceInfo
		, PassTextureConfig const & passConfig
		, TextureAnimationUPtr animation )
	{
		auto & sourceData = getSourceData( sourceInfo );
		auto hash = cachetex::makeHash( sourceInfo );
		auto ires = m_unitDatas.emplace( hash, nullptr );

		if ( ires.second )
		{
			ires.first->second = castor::makeUnique< TextureUnitData >( &sourceData
				, passConfig
				, std::move( animation ) );
		}

		return *ires.first->second;
	}

	Texture const * TextureUnitCache::doGetTexture( TextureData & data
		, std::function< void( TextureData const &, Texture const * ) > onEndCpuLoad )
	{
		Texture * result{};
		bool wasFound{};
		{
			auto hash = cachetex::makeHash( data.sourceInfo );
			auto lock( castor::makeUniqueLock( m_loadMtx ) );
			auto ires = m_loaded.emplace( hash, nullptr );
			auto it = ires.first;
			wasFound = !ires.second;

			if ( !wasFound )
			{
				if ( data.sourceInfo.isRenderTarget() )
				{
					auto texture = &data.sourceInfo.renderTarget()->getTexture();

					if ( onEndCpuLoad )
					{
						onEndCpuLoad( data, texture );
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
						auto lock( castor::makeUniqueLock( m_loadMtx ) );
						doInitTexture( threadData );

						if ( onEndCpuLoad )
						{
							onEndCpuLoad( *threadData.data, threadData.texture );
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
							onEndCpuLoad( data, result );
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
			auto & device = *getEngine()->getRenderDevice();
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
		m_loading.emplace_back( std::make_unique< ThreadData >( data ) );
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

	std::vector< TextureUnit * > TextureUnitCache::doListTextureUnits( Texture const * texture )
	{
		auto lock( castor::makeUniqueLock( m_loadMtx ) );
		auto it = m_unitsToAdd.find( texture );

		if ( it == m_unitsToAdd.end() )
		{
			return {};
		}

		auto result = std::move( it->second );
		m_unitsToAdd.erase( it );
		return result;
	}

	//*********************************************************************************************
}
