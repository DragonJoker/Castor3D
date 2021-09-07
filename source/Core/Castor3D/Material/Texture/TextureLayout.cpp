#include "Castor3D/Material/Texture/TextureLayout.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Material/Texture/TextureSource.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

#include <CastorUtils/Miscellaneous/BitSize.hpp>
#include <CastorUtils/Graphics/ImageCache.hpp>
#include <CastorUtils/Graphics/PixelBufferBase.hpp>
#include <CastorUtils/Graphics/Size.hpp>

#include <ashes/ashes.hpp>
#include <ashespp/Command/CommandBuffer.hpp>
#include <ashespp/Image/Image.hpp>
#include <ashespp/Image/ImageView.hpp>
#include <ashespp/Sync/Fence.hpp>

CU_ImplementCUSmartPtr( castor3d, TextureLayout )
CU_ImplementCUSmartPtr( castor3d, TextureSource )

namespace castor3d
{
	//************************************************************************************************

	namespace
	{
		VkImageViewType getSubviewType( VkImageType type
			, VkImageCreateFlags flags
			, uint32_t arrayLayers )
		{
			VkImageType result = type;

			switch ( result )
			{
			case VK_IMAGE_TYPE_1D:
				if ( arrayLayers > 1 )
				{
					return VK_IMAGE_VIEW_TYPE_1D_ARRAY;
				}
				return VK_IMAGE_VIEW_TYPE_1D;

			case VK_IMAGE_TYPE_2D:
				if ( arrayLayers > 1 )
				{
					if ( castor::checkFlag( flags, VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT ) )
					{
						CU_Require( ( arrayLayers % 6 ) == 0 );
						return arrayLayers == 6u
							? VK_IMAGE_VIEW_TYPE_CUBE
							: VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;
					}

					return VK_IMAGE_VIEW_TYPE_2D_ARRAY;
				}

				return VK_IMAGE_VIEW_TYPE_2D;

			case VK_IMAGE_TYPE_3D:
				return VK_IMAGE_VIEW_TYPE_3D;

			default:
				CU_Failure( "Unsupported texture type." );
				return VK_IMAGE_VIEW_TYPE_2D;
			}
		}

		ashes::ImageViewCreateInfo getSubviewCreateInfos( ashes::ImageCreateInfo const & info
			, VkImage image
			, uint32_t baseMipLevel
			, uint32_t levelCount
			, uint32_t baseArrayLayer
			, uint32_t arrayLayers )
		{
			ashes::ImageViewCreateInfo view
			{
				0u,
				image,
				getSubviewType( info->imageType, info->flags, arrayLayers ),
				info->format,
				VkComponentMapping{},
				{
					ashes::getAspectMask( info->format ),
					baseMipLevel,
					levelCount,
					baseArrayLayer,
					arrayLayers,
				}
			};
			return view;
		}

		TextureViewUPtr createSubview( TextureLayout & layout
			, castor::String debugName
			, ashes::ImageCreateInfo const & info
			, uint32_t baseMipLevel
			, uint32_t levelCount
			, uint32_t baseArrayLayer
			, uint32_t arrayLayers )
		{
			return castor::makeUnique< TextureView >( layout
				, getSubviewCreateInfos( info
					, VK_NULL_HANDLE
					, baseMipLevel
					, levelCount
					, baseArrayLayer
					, arrayLayers )
				, 0u
				, std::move( debugName ) );
		}

		uint32_t getMinMipLevels( uint32_t mipLevels
			, VkExtent3D const & extent
			, VkFormat format )
		{
			return std::min( getMipLevels( extent, format ), mipLevels );
		}

		uint32_t adjustMipLevels( uint32_t mipLevels
			, VkFormat format )
		{
			if ( format == VK_FORMAT_UNDEFINED )
			{
				return mipLevels;
			}

			auto blockSize = ashes::getBlockSize( format );
			auto bitSize = castor::getBitSize( blockSize.extent.width );
			return mipLevels / bitSize;
		}

		bool eraseViews( uint32_t mipLevels
			, MipView & views )
		{
			auto itLevel = views.levels.begin();

			while ( itLevel != views.levels.end() )
			{
				auto & level = *itLevel;

				if ( level->getBaseMipLevel() >= mipLevels )
				{
					itLevel = views.levels.erase( itLevel );
				}
				else
				{
					++itLevel;
				}
			}

			return views.levels.empty();
		}
		
		bool eraseViews( uint32_t mipLevels
			, CubeView & views )
		{
			auto itFace = views.faces.begin();

			while ( itFace != views.faces.end() )
			{
				auto & face = *itFace;

				if ( eraseViews( mipLevels, face ) )
				{
					itFace = views.faces.erase( itFace );
				}
				else
				{
					++itFace;
				}
			}

			return views.faces.empty();
		}

		template< typename ViewT >
		void eraseViews( uint32_t mipLevels
			, ArrayView< ViewT > & views )
		{
			auto itLayer = views.layers.begin();

			while ( itLayer != views.layers.end() )
			{
				auto & layer = *itLayer;

				if ( eraseViews( mipLevels, layer ) )
				{
					itLayer = views.layers.erase( itLayer );
				}
				else
				{
					++itLayer;
				}
			}
		}

		template< typename ViewT >
		void eraseViews( uint32_t mipLevels
			, SliceView< ViewT > & views )
		{
			auto itSlice = views.slices.begin();

			while ( itSlice != views.slices.end() )
			{
				auto & slice = *itSlice;

				if ( eraseViews( mipLevels, slice ) )
				{
					itSlice = views.slices.erase( itSlice );
				}
				else
				{
					++itSlice;
				}
			}
		}

		void createViews( ashes::ImageCreateInfo const & info
			, castor::String debugName
			, TextureLayout & layout
			, MipView & view
			, uint32_t baseArrayLayer
			, uint32_t layerCount )
		{
			view.view = createSubview( layout
				, debugName
				, info
				, 0u
				, adjustMipLevels( info->mipLevels, info->format )
				, baseArrayLayer
				, layerCount );

			if ( info->mipLevels > 1 )
			{
				uint32_t levelIndex = 0u;
				view.levels.resize( info->mipLevels );

				for ( auto & level : view.levels )
				{
					level = createSubview( layout
						, debugName
						, info
						, levelIndex++
						, 1u
						, baseArrayLayer
						, layerCount );
				}
			}
		}

		void createViews( ashes::ImageCreateInfo const & info
			, castor::String debugName
			, TextureLayout & layout
			, MipView & view
			, uint32_t & baseArrayLayer )
		{
			createViews( info
				, debugName
				, layout
				, view
				, baseArrayLayer++
				, 1u );
		}

		void createViews( ashes::ImageCreateInfo const & info
			, castor::String debugName
			, TextureLayout & layout
			, CubeView & view
			, uint32_t & baseArrayLayer )
		{
			createViews( info
				, debugName
				, layout
				, view.view
				, baseArrayLayer
				, 6u );
			view.faces.resize( 6u );

			for ( auto & face : view.faces )
			{
				createViews( info
					, debugName
					, layout
					, face
					, baseArrayLayer );
			}
		}

		template< typename ViewT >
		void createViews( ashes::ImageCreateInfo const & info
			, TextureLayout & layout
			, castor::String debugName
			, ArrayView< ViewT > & view )
		{
			uint32_t baseArrayLayer = 0u;

			for ( auto & layer : view.layers )
			{
				createViews( info
					, debugName
					, layout
					, layer
					, baseArrayLayer );
			}
		}

		template< typename ViewT >
		void createViews( ashes::ImageCreateInfo const & info
			, TextureLayout & layout
			, castor::String debugName
			, SliceView< ViewT > & view )
		{
			uint32_t baseSlice = 0u;

			for ( auto & slice : view.slices )
			{
				createViews( info
					, debugName
					, layout
					, slice
					, baseSlice );
			}
		}

		MipView createViews( ashes::ImageCreateInfo const & info
			, TextureLayout & layout
			, castor::String debugName )
		{
			MipView result;
			createViews( info
				, debugName
				, layout
				, result
				, 0u
				, info->arrayLayers );
			return result;
		}

		void update( MipView & view
			, VkImage image
			, uint32_t baseArrayLayer
			, uint32_t layerCount
			, uint32_t baseMipLevel
			, uint32_t levelCount )
		{
			view.view->update( image
				, baseArrayLayer
				, layerCount
				, baseMipLevel
				, levelCount );

			for ( auto & level : view.levels )
			{
				level->update( image
					, baseArrayLayer
					, layerCount
					, baseMipLevel++
					, 1u );
			}
		}

		void update( MipView & view
			, VkImage image
			, uint32_t & baseArrayLayer
			, uint32_t baseMipLevel
			, uint32_t levelCount )
		{
			update( view
				, image
				, baseArrayLayer++
				, 1u
				, baseMipLevel
				, levelCount );
		}

		void update( CubeView & view
			, VkImage image
			, uint32_t & baseArrayLayer
			, uint32_t baseMipLevel
			, uint32_t levelCount )
		{
			update( view.view
				, image
				, baseArrayLayer
				, 6u
				, baseMipLevel
				, levelCount );

			for ( auto & face : view.faces )
			{
				update( face
					, image
					, baseArrayLayer
					, baseMipLevel
					, levelCount );
			}
		}

		template< typename ViewT >
		void update( ArrayView< ViewT > & view
			, VkImage image
			, uint32_t levelCount )
		{
			uint32_t baseArrayLayer = 0u;

			for ( auto & layer : view.layers )
			{
				update( layer
					, image
					, baseArrayLayer
					, 0u
					, levelCount );
			}
		}

		template< typename ViewT >
		void update( SliceView< ViewT > & view
			, VkImage image
			, uint32_t levelCount )
		{
			uint32_t baseSlice = 0u;

			for ( auto & layer : view.slices )
			{
				update( layer
					, image
					, baseSlice
					, 0u
					, levelCount );
			}
		}

		void update( MipView & view
			, VkExtent3D const & extent
			, VkFormat format
			, uint32_t mipLevels
			, uint32_t arrayLayers )
		{
			view.view->update( extent
				, format
				, mipLevels
				, arrayLayers );

			for ( auto & level : view.levels )
			{
				level->update( extent
					, format
					, 1u
					, 1u );
			}
		}

		void update( MipView & view
			, VkExtent3D const & extent
			, VkFormat format
			, uint32_t mipLevels )
		{
			update( view
				, extent
				, format
				, 1u
				, mipLevels );
		}

		void update( CubeView & view
			, VkExtent3D const & extent
			, VkFormat format
			, uint32_t mipLevels )
		{
			update( view.view
				, extent
				, format
				, 6u
				, mipLevels );

			for ( auto & face : view.faces )
			{
				update( face
					, extent
					, format
					, mipLevels );
			}
		}

		template< typename ViewT >
		void update( ArrayView< ViewT > & view
			, VkExtent3D const & extent
			, VkFormat format
			, uint32_t mipLevels )
		{
			for ( auto & layer : view.layers )
			{
				update( layer
					, extent
					, format
					, mipLevels );
			}
		}

		template< typename ViewT >
		void update( SliceView< ViewT > & view
			, VkExtent3D const & extent
			, VkFormat format
			, uint32_t mipLevels )
		{
			for ( auto & slice : view.slices )
			{
				update( slice
					, extent
					, format
					, mipLevels );
			}
		}

		castor::String getBufferName( castor::PxBufferBase const & value )
		{
			auto stream = castor::makeStringStream();
			stream << value.getWidth()
				<< cuT( "x" ) << value.getHeight()
				<< cuT( "x" ) << value.getLayers()
				<< cuT( "_" ) << ashes::getName( VkFormat( value.getFormat() ) );
			return stream.str();
		}

		castor::PxBufferBaseSPtr adaptBuffer( castor::PxBufferBaseSPtr buffer
			, uint32_t mipLevels )
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
				buffer = castor::PxBufferBase::create( buffer->getDimensions()
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

			buffer->update( buffer->getLayers()
				, mipLevels );
			return buffer;
		}

		castor::Image getFileImage( Engine & engine
			, castor::String const & name
			, castor::Path const & folder
			, castor::Path const & relative
			, uint32_t mipLevels
			, uint32_t & srcMipLevels
			, bool allowCompression
			, bool generateMips )
		{
			auto image = engine.getImageCache().tryFind( name );

			if ( !image.lock() )
			{
				image = engine.getImageCache().add( name
					, castor::ImageCreateParams{ folder / relative
						, allowCompression
						, generateMips } );
			}

			auto buffer = adaptBuffer( image.lock()->getPixels(), mipLevels );
			srcMipLevels = buffer->getLevels();
			castor::ImageLayout layout{ image.lock()->getLayout().type, *buffer };
			return castor::Image{ name
				, folder / relative
				, layout
				, std::move( buffer ) };
		}

		void processLevels( RenderDevice const & device
			, QueueData const & queueData
			, castor::Image const & image
			, ashes::Image const & texture
			, ashes::ImageViewCreateInfo & viewInfo )
		{
			auto & layout = image.getLayout();
			auto buffer = makeBufferBase( device
				, image.getPxBuffer().getSize()
				, VK_BUFFER_USAGE_TRANSFER_SRC_BIT
				, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
				, image.getName() + "StagingBuffer" );
			auto mappedSize = ashes::getAlignedSize( image.getPxBuffer().getSize()
				, device.renderSystem.getValue( GpuMin::eBufferMapSize ) );

			if ( auto data = buffer->lock( 0u, mappedSize, 0u ) )
			{
				std::memcpy( data, image.getPxBuffer().getConstPtr(), image.getPxBuffer().getSize() );
				buffer->flush( 0u, mappedSize );
				buffer->unlock();
			}
			else
			{
				return;
			}

			ashes::VkBufferImageCopyArray copies;
			VkExtent2D baseDimensions{ image.getWidth(), image.getHeight() };

			for ( auto layer = viewInfo->subresourceRange.baseArrayLayer;
				layer < viewInfo->subresourceRange.baseArrayLayer + viewInfo->subresourceRange.layerCount;
				++layer )
			{
				VkImageSubresourceLayers subresourceLayers{ viewInfo->subresourceRange.aspectMask
					, 0u
					, layer
					, 1u };

				for ( auto level = layout.baseLevel;
					level < layout.baseLevel + viewInfo->subresourceRange.levelCount;
					++level )
				{
					subresourceLayers.mipLevel = level;
					auto offset = image.getLayout().layerMipOffset( layer
						, level );
					copies.push_back( { offset
						, 0u
						, 0u
						, subresourceLayers
						, VkOffset3D{}
						, VkExtent3D{ std::max( 1u, baseDimensions.width >> level )
							, std::max( 1u, baseDimensions.height >> level )
							, 1u } } );
				}
			}

			auto commandBuffer = queueData.commandPool->createCommandBuffer( image.getName() + "ImageUpload"
				, VK_COMMAND_BUFFER_LEVEL_PRIMARY );
			commandBuffer->begin( VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT );
			commandBuffer->beginDebugBlock( { "Upload " + image.getName() + " Image"
				, { 0.5f, 0.5f, 0.5f, 1.0f } } );
			commandBuffer->memoryBarrier( VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT
				, VK_PIPELINE_STAGE_TRANSFER_BIT
				, texture.makeTransition( VK_IMAGE_LAYOUT_UNDEFINED
					, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
					, viewInfo->subresourceRange ) );
			commandBuffer->copyToImage( copies, *buffer, texture );
			commandBuffer->memoryBarrier( VK_PIPELINE_STAGE_TRANSFER_BIT
				, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
				, texture.makeTransition( VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
					, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
					, viewInfo->subresourceRange
					, queueData.parent->familyIndex
					, device.getGraphicsQueueFamilyIndex() ) );
			commandBuffer->endDebugBlock();
			commandBuffer->end();
			auto fence = device.device->createFence();
			queueData.queue->submit( *commandBuffer
				, fence.get() );
			fence->wait( ashes::MaxTimeout );
		}

		auto updateMipLevels( bool genNeeded
			, uint32_t mipLevels
			, uint32_t imgMipLevels
			, MipView & mipView )
		{
			auto minLevels = std::min( mipLevels, uint32_t( mipView.levels.size() ) );

			for ( uint32_t level = 0u; level < minLevels; ++level )
			{
				mipView.levels[level]->setMipmapsGenerationNeeded( genNeeded );
			}

			if ( mipLevels >= mipView.view->getLevelCount()
				|| imgMipLevels <= mipLevels )
			{
				mipView.view->setMipmapsGenerationNeeded( genNeeded );
			}
		}

		auto updateMipLevels( bool genNeeded
			, uint32_t mipLevels
			, MipView & mipView )
		{
			updateMipLevels( genNeeded
				, mipLevels
				, mipView.view->getLevelCount()
				, mipView );
		}
	}

	//************************************************************************************************

	TextureLayoutSPtr createTextureLayout( Engine const & engine
		, castor::Path const & relative
		, castor::Path const & folder
		, bool allowCompression )
	{
		ashes::ImageCreateInfo createInfo
		{
			0u,
			VK_IMAGE_TYPE_2D,
			VK_FORMAT_UNDEFINED,
			{ 1u, 1u, 1u },
			20u,
			1u,
			VK_SAMPLE_COUNT_1_BIT,
			VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
		};
		auto texture = std::make_shared< TextureLayout >( *engine.getRenderSystem()
			, createInfo
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			, relative );
		texture->setSource( folder
			, relative
			, allowCompression
			, true );
		return texture;
	}

	TextureLayoutSPtr createTextureLayout( Engine const & engine
		, castor::String const & name
		, castor::PxBufferBaseUPtr buffer
		, bool isStatic )
	{
		ashes::ImageCreateInfo createInfo
		{
			0u,
			( buffer->getHeight() <= 1u && buffer->getWidth() > 1u
				? VK_IMAGE_TYPE_1D
				: VK_IMAGE_TYPE_2D ),
			VK_FORMAT_UNDEFINED,
			{ buffer->getWidth(), buffer->getHeight(), 1u },
			uint32_t( castor::getBitSize( std::min( buffer->getWidth(), buffer->getHeight() ) ) ),
			1u,
			VK_SAMPLE_COUNT_1_BIT,
			VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
		};
		log::debug << ( cuT( "Creating " ) + name + cuT( " texture layout.\n" ) );
		auto texture = std::make_shared < TextureLayout >( *engine.getRenderSystem()
			, createInfo
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			, name );
		texture->setSource( std::move( buffer ), isStatic );
		return texture;
	}

	uint32_t getMipLevels( VkExtent3D const & extent
		, VkFormat format )
	{
		auto blockSize = ashes::getBlockSize( format );
		auto min = std::min( extent.width / blockSize.extent.width, extent.height / blockSize.extent.height );
		return uint32_t( castor::getBitSize( min ) );
	}

	castor::ImageLayout::Type convert( VkImageCreateFlags flags
		, VkImageType imageType
		, VkExtent3D const & extent
		, uint32_t arrayLayers )
	{
		if ( extent.depth > 1u )
		{
			return castor::ImageLayout::e3D;
		}

		if ( extent.height > 1u || extent.width <= 1u )
		{
			if ( castor::checkFlag( flags, VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT ) )
			{
				if ( arrayLayers == 6u )
				{
					return castor::ImageLayout::eCube;
				}

				if ( arrayLayers > 6u && ( arrayLayers % 6u == 0u ) )
				{
					return castor::ImageLayout::eCubeArray;
				}
			}

			if ( arrayLayers > 1u )
			{
				return castor::ImageLayout::e2DArray;
			}

			return castor::ImageLayout::e2D;
		}

		if ( arrayLayers > 1u )
		{
			return castor::ImageLayout::e1DArray;
		}

		return castor::ImageLayout::e1D;
	}

	castor::ImageLayout convert( ashes::ImageCreateInfo const & value )
	{
		return castor::ImageLayout
		{
			convert( value->flags, value->imageType, value->extent, value->arrayLayers ),
			castor::PixelFormat( value->format ),
			castor::Point3ui{ value->extent.width, value->extent.height, value->extent.depth },
			0u,
			value->arrayLayers,
			0u,
			value->mipLevels,
		};
	}

	VkImageType convert( castor::ImageLayout::Type type )
	{
		switch ( type )
		{
		case castor::ImageLayout::e1D:
		case castor::ImageLayout::e1DArray:
			return VK_IMAGE_TYPE_1D;
		case castor::ImageLayout::e2D:
		case castor::ImageLayout::eCube:
		case castor::ImageLayout::e2DArray:
		case castor::ImageLayout::eCubeArray:
			return VK_IMAGE_TYPE_2D;
		case castor::ImageLayout::e3D:
			return VK_IMAGE_TYPE_3D;
		default:
			CU_Failure( "Unexpected castor::ImageLayout::Type" );
			return VK_IMAGE_TYPE_2D;
		}
	}

	//************************************************************************************************

	TextureLayout::TextureLayout( RenderSystem & renderSystem
		, ashes::ImageCreateInfo info
		, VkMemoryPropertyFlags memoryProperties
		, castor::String debugName
		, bool isStatic )
		: OwnedBy< RenderSystem >{ renderSystem }
		, m_static{ isStatic }
		, m_info{ std::move( info ) }
		, m_properties{ memoryProperties }
		, m_image{ debugName, castor::Path{}, convert( m_info ) }
		, m_defaultView{ createViews( m_info, *this, m_image.getName() ) }
		, m_cubeView{ &m_defaultView }
		, m_arrayView{ &m_defaultView }
		, m_sliceView{ &m_defaultView }
	{
		m_info->mipLevels = std::max( 1u, m_info->mipLevels );

		if ( m_info->arrayLayers > 1u )
		{
			if ( m_info->arrayLayers >= 6u
				&& ( m_info->arrayLayers % 6u ) == 0u
				&& castor::checkFlag( m_info->flags, VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT ) )
			{
				m_cubeView.layers.resize( m_info->arrayLayers / 6u );
				createViews( m_info, *this, m_image.getName(), m_cubeView );
			}
			else
			{
				m_arrayView.layers.resize( m_info->arrayLayers );
				createViews( m_info, *this, m_image.getName(), m_arrayView );
			}
		}
		else if ( m_info->extent.depth > 1u )
		{
			m_sliceView.slices.resize( m_info->extent.depth );
			createViews( m_info, *this, m_image.getName(), m_sliceView );
		}
	}

	TextureLayout::TextureLayout( RenderSystem & renderSystem
		, VkImage image
		, crg::ImageViewId imageView )
		: TextureLayout{ renderSystem
			, std::make_unique< ashes::Image >( *renderSystem.getRenderDevice()
				, image
				, imageView.data->image.data->info )
			, imageView.data->image.data->info }
	{
	}

	TextureLayout::TextureLayout( RenderSystem & renderSystem
		, ashes::ImagePtr image
		, VkImageCreateInfo const & createInfo )
		: OwnedBy< RenderSystem >{ renderSystem }
		, m_static{ false }
		, m_info{ createInfo }
		, m_properties{}
		, m_image{ castor::cuEmptyString
			, castor::Path{ castor::cuEmptyString }
			, makeSize( m_info->extent )
			, castor::PixelFormat( m_info->format ) }
		, m_defaultView{ createViews( m_info, *this, m_image.getName() ) }
		, m_cubeView{ &m_defaultView }
		, m_arrayView{ &m_defaultView }
		, m_sliceView{ &m_defaultView }
		, m_ownTexture{ std::move( image ) }
		, m_texture{ m_ownTexture.get() }
	{
		m_info->mipLevels = std::max( 1u, m_info->mipLevels );

		if ( m_info->arrayLayers > 1u )
		{
			if ( m_info->arrayLayers >= 6u
				&& ( m_info->arrayLayers % 6u ) == 0u
				&& castor::checkFlag( m_info->flags, VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT ) )
			{
				m_cubeView.layers.resize( m_info->arrayLayers / 6u );
				createViews( m_info, *this, m_image.getName(), m_cubeView );
			}
			else
			{
				m_arrayView.layers.resize( m_info->arrayLayers );
				createViews( m_info, *this, m_image.getName(), m_arrayView );
			}
		}
		else if ( m_info->extent.depth > 1u )
		{
			m_sliceView.slices.resize( m_info->extent.depth );
			createViews( m_info, *this, m_image.getName(), m_sliceView );
		}

		m_defaultView.forEachView( []( TextureViewUPtr const & view )
			{
				view->initialise();
			} );
		m_arrayView.forEachView( []( TextureViewUPtr const & view )
			{
				view->initialise();
			} );
		m_cubeView.forEachView( []( TextureViewUPtr const & view )
			{
				view->initialise();
			} );
		m_sliceView.forEachView( []( TextureViewUPtr const & view )
			{
				view->initialise();
			} );

		m_initialised = true;
	}

	TextureLayout::~TextureLayout()
	{
		m_sliceView.slices.clear();
		m_arrayView.layers.clear();
		m_cubeView.layers.clear();
		m_defaultView.levels.clear();
		m_defaultView.view.reset();
	}

	bool TextureLayout::initialise( RenderDevice const & device
		, QueueData const & queueData )
	{
		if ( !m_initialised )
		{
			auto props = device->getPhysicalDevice().getFormatProperties( m_info->format );
			VkImageFormatProperties imageProps{};
			auto res = device->getPhysicalDevice().getImageFormatProperties( m_info->format
				, m_info->imageType
				, m_info->tiling
				, m_info->usage
				, m_info->flags
				, imageProps );

			if ( res != VK_SUCCESS )
			{
				CU_Exception( "Unsupported image format properties" );
			}

			if ( castor::checkFlag( props.optimalTilingFeatures, VK_FORMAT_FEATURE_TRANSFER_DST_BIT ) )
			{
				res = device->getPhysicalDevice().getImageFormatProperties( m_info->format
					, m_info->imageType
					, m_info->tiling
					, m_info->usage | VK_IMAGE_USAGE_TRANSFER_DST_BIT
					, m_info->flags
					, imageProps );

				if ( res == VK_SUCCESS )
				{
					m_info->usage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
				}
			}

			if ( m_info->mipLevels > 1u )
			{
				CU_Require( castor::checkFlag( props.optimalTilingFeatures, VK_FORMAT_FEATURE_TRANSFER_SRC_BIT ) );
			}
			else if ( m_info->mipLevels == 0 )
			{
				m_info->mipLevels = 1u;
			}

			if ( !m_ownTexture )
			{
				m_ownTexture = makeImage( device
					, m_info
					, m_properties
					, m_image.getName() );
			}

			m_texture = m_ownTexture.get();
			CU_Require( m_info->mipLevels <= 1u
				|| ( castor::checkFlag( props.optimalTilingFeatures, VK_FORMAT_FEATURE_BLIT_DST_BIT )
					|| !m_defaultView.view->isMipmapsGenerationNeeded() ) );

			if ( isStatic() )
			{
				ashes::ImageViewCreateInfo viewInfo
				{
					0u,
					*m_texture,
					( ( m_info->extent.height > 1u || m_image.getLayout().type == castor::ImageLayout::e2D )
						? VK_IMAGE_VIEW_TYPE_2D
						: VK_IMAGE_VIEW_TYPE_1D ),
					m_info->format,
					{},
					{ ashes::getAspectMask( m_info->format ), 0u, 1u, 0u, m_image.getLayout().depthLayers() },
				};
				uint32_t mipLevels = m_defaultView.view->isMipmapsGenerationNeeded()
					? 1u
					: m_image.getLayout().levels;
				viewInfo->subresourceRange.levelCount = mipLevels;
				processLevels( device, queueData, m_image, *m_texture, viewInfo );
			}

			m_defaultView.forEachView( []( TextureViewUPtr const & view )
				{
					view->initialise();
				} );
			m_arrayView.forEachView( []( TextureViewUPtr const & view )
				{
					view->initialise();
				} );
			m_cubeView.forEachView( []( TextureViewUPtr const & view )
				{
					view->initialise();
				} );
			m_sliceView.forEachView( []( TextureViewUPtr const & view )
				{
					view->initialise();
				} );
			m_initialised = m_texture != nullptr;
		}

		if ( m_initialised )
		{
			CU_Require( m_texture );
		}

		return m_initialised;
	}

	void TextureLayout::cleanup()
	{
		if ( m_initialised )
		{
			m_sliceView.forEachView( []( TextureViewUPtr const & view )
				{
					view->cleanup();
				} );
			m_cubeView.forEachView( []( TextureViewUPtr const & view )
				{
					view->cleanup();
				} );
			m_arrayView.forEachView( []( TextureViewUPtr const & view )
				{
					view->cleanup();
				} );
			m_defaultView.forEachView( []( TextureViewUPtr const & view )
				{
					view->cleanup();
				} );
			m_ownTexture.reset();
		}

		m_initialised = false;
	}

	void TextureLayout::generateMipmaps( QueueData const & queueData )const
	{
		if ( m_info->mipLevels > 1u
			&& getDefaultView().isMipmapsGenerationNeeded() )
		{
			CU_Require( m_texture );
			auto commandBuffer = queueData.commandPool->createCommandBuffer( "TextureGenMipmaps" );
			commandBuffer->begin();
			commandBuffer->beginDebugBlock( { getName() + " Mipmaps Generation"
				, makeFloatArray( getRenderSystem()->getEngine()->getNextRainbowColour() ) } );
			generateMipmaps( *commandBuffer );
			commandBuffer->endDebugBlock();
			commandBuffer->end();
			queueData.queue->submit( *commandBuffer, nullptr );
			queueData.queue->waitIdle();
		}
	}

	void TextureLayout::generateMipmaps( RenderDevice const & device )const
	{
		generateMipmaps( *device.graphicsData() );
	}

	void TextureLayout::generateMipmaps( ashes::CommandBuffer & cmd )const
	{
		CU_Require( m_texture );
		m_texture->generateMipmaps( cmd
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
	}

	void TextureLayout::setSource( castor::Path const & folder
		, castor::Path const & relative
		, bool allowCompression
		, bool generateMips )
	{
		uint32_t srcMips = 1u;
		m_image = getFileImage( *getRenderSystem()->getEngine()
			, m_image.getName()
			, folder
			, relative
			, m_image.getLevels()
			, srcMips
			, allowCompression
			, generateMips );
		doUpdateCreateInfo( m_image.getLayout() );
		doUpdateMips( false, srcMips );
		m_static = true;
	}

	void TextureLayout::setSource( VkExtent3D const & extent
		, VkFormat format )
	{
		setSource( castor::PxBufferBase::create( { extent.width, extent.height }
			, extent.depth
			, 1u
			, castor::PixelFormat( format ) ) );
		m_static = true;
	}

	void TextureLayout::setSource( castor::PxBufferBaseSPtr buffer
		, uint32_t bufferOrigLevels
		, bool isStatic )
	{
		buffer = adaptBuffer( buffer, buffer->getLevels() );
		m_image = { m_image.getName(), castor::ImageLayout{ *buffer }, buffer };
		doUpdateCreateInfo( m_image.getLayout() );
		doUpdateMips( false, bufferOrigLevels );
		m_static = isStatic;
	}

	void TextureLayout::setSource( castor::PxBufferBaseSPtr buffer
		, bool isStatic )
	{
		setSource( buffer
			, m_image.getPixels()->getLevels()
			, isStatic );
	}

	void TextureLayout::setLayerSource( uint32_t index
		, castor::PxBufferBaseSPtr buffer
		, uint32_t bufferOrigLevels )
	{
		buffer = adaptBuffer( buffer, buffer->getLevels() );
		castor::Image srcImage{ getBufferName( *buffer ), castor::ImageLayout{ *buffer }, buffer };
		auto & srcLayout = srcImage.getLayout();
		doUpdateFromFirstImage( 0u, srcLayout );
		auto & dstLayout = m_image.getLayout();
		auto src = srcLayout.buffer( *buffer );
		auto dst = dstLayout.layerBuffer( m_image.getPxBuffer(), index );
		CU_Require( src.size() == dst.size() );
		std::memcpy( dst.data(), src.data(), std::min( src.size(), dst.size() ) );
		doUpdateLayerMips( false
			, index
			, m_image.getPixels()->getLevels() );
	}

	void TextureLayout::setLayerSource( uint32_t index
		, castor::PxBufferBaseSPtr buffer )
	{
		setLayerSource( index
			, buffer
			, m_image.getPixels()->getLevels() );
	}

	void TextureLayout::setLayerSource( uint32_t index
		, castor::Path const & folder
		, castor::Path const & relative
		, bool allowCompression
		, bool generateMips )
	{
		uint32_t srcMips = 1u;
		auto image = getFileImage( *getRenderSystem()->getEngine()
			, m_image.getName() + castor::string::toString( index )
			, folder
			, relative
			, m_image.getLevels()
			, srcMips
			, allowCompression
			, generateMips );
		setLayerSource( index
			, image.getPixels()
			, srcMips );
	}

	void TextureLayout::setLayerSource( uint32_t index
		, VkExtent3D const & extent
		, VkFormat format )
	{
		setLayerSource( index
			, castor::PxBufferBase::create( { extent.width, extent.height }
				, extent.depth
				, m_image.getLevels()
				, castor::PixelFormat( format ) )
			, m_image.getLevels() );
	}

	void TextureLayout::setLayerMipSource( uint32_t index
		, uint32_t level
		, castor::PxBufferBaseSPtr buffer )
	{
		buffer = adaptBuffer( buffer, 1u );
		castor::Image srcImage{ getBufferName( *buffer ), castor::ImageLayout{ *buffer }, buffer };
		auto & srcLayout = srcImage.getLayout();
		auto & dstLayout = m_image.getLayout();
		doUpdateFromFirstImage( level, srcLayout );
		auto src = srcLayout.buffer( *buffer );
		auto dst = dstLayout.layerMipBuffer( m_image.getPxBuffer(), index, level );
		CU_Require( src.size() == dst.size() );
		std::memcpy( dst.data(), src.data(), std::min( src.size(), dst.size() ) );
		doUpdateLayerMip( false
			, index
			, level );
	}

	void TextureLayout::setLayerMipSource( uint32_t index
		, uint32_t level
		, castor::Path const & folder
		, castor::Path const & relative
		, bool allowCompression )
	{
		uint32_t srcMips = 1u;
		auto image = getFileImage( *getRenderSystem()->getEngine()
			, m_image.getName() + castor::string::toString( index ) + "_" + castor::string::toString( level )
			, folder
			, relative
			, 1u
			, srcMips
			, allowCompression
			, false );
		setLayerMipSource( index
			, level
			, image.getPixels() );
	}

	void TextureLayout::setLayerMipSource( uint32_t index
		, uint32_t level
		, VkExtent3D const & extent
		, VkFormat format )
	{
		setLayerMipSource( index
			, level
			, castor::PxBufferBase::create( { extent.width, extent.height }
				, extent.depth
				, 1u
				, castor::PixelFormat( format ) ) );
	}

	void TextureLayout::setLayerCubeFaceSource( uint32_t layer
		, CubeMapFace face
		, castor::PxBufferBaseSPtr buffer )
	{
		setLayerSource( layer * 6u + uint32_t( face )
			, buffer );
	}

	void TextureLayout::setLayerCubeFaceSource( uint32_t layer
		, CubeMapFace face
		, castor::Path const & folder
		, castor::Path const & relative
		, bool allowCompression
		, bool generateMips )
	{
		setLayerSource( layer * 6u + uint32_t( face )
			, folder
			, relative
			, allowCompression
			, generateMips );
	}

	void TextureLayout::setLayerCubeFaceSource( uint32_t layer
		, CubeMapFace face
		, VkExtent2D const & extent
		, VkFormat format )
	{
		setLayerSource( layer * 6u + uint32_t( face )
			, extent
			, format );
	}

	void TextureLayout::setLayerCubeFaceMipSource( uint32_t layer
		, CubeMapFace face
		, uint32_t level
		, castor::PxBufferBaseSPtr buffer )
	{
		setLayerMipSource( layer * 6u + uint32_t( face )
			, level
			, buffer );
	}

	void TextureLayout::setLayerCubeFaceMipSource( uint32_t layer
		, CubeMapFace face
		, uint32_t level
		, castor::Path const & folder
		, castor::Path const & relative
		, bool allowCompression )
	{
		setLayerMipSource( layer * 6u + uint32_t( face )
			, level
			, folder
			, relative
			, allowCompression );
	}

	void TextureLayout::setLayerCubeFaceMipSource( uint32_t layer
		, CubeMapFace face
		, uint32_t level
		, VkExtent2D const & extent
		, VkFormat format )
	{
		setLayerMipSource( layer * 6u + uint32_t( face )
			, level
			, extent
			, format );
	}

	castor::String TextureLayout::getName()const
	{
		return m_image.getName();
	}

	castor::Path TextureLayout::getPath()const
	{
		return m_image.getPath();
	}

	bool TextureLayout::needsYInversion()const
	{
		return getDefaultView().needsYInversion()
			|| m_image.getPixels()->isFlipped();
	}

	uint32_t TextureLayout::doUpdateViews()
	{
		auto mipLevels = m_info->mipLevels;

		if ( mipLevels > 1u )
		{
			m_info->mipLevels = getMinMipLevels( mipLevels, m_info->extent, m_info->format );
		}

		eraseViews( m_info->mipLevels, m_defaultView );
		m_info->mipLevels = std::min( m_info->mipLevels
			, ( getDefaultView().getLevelCount() > 1u
				? getDefaultView().getLevelCount()
				: m_info->mipLevels ) );

		if ( mipLevels > 1u )
		{
			if ( m_info->mipLevels != mipLevels )
			{
				eraseViews( m_info->mipLevels, m_arrayView );
				eraseViews( m_info->mipLevels, m_cubeView );
				eraseViews( m_info->mipLevels, m_sliceView );
			}

			m_info->usage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
			m_info->usage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
		}

		m_image.getLayout().levels = m_info->mipLevels;
		return mipLevels;
	}

	void TextureLayout::doUpdateCreateInfo( castor::ImageLayout const & layout )
	{
		m_info->imageType = convert( layout.type );
		m_info->extent.width = layout.extent->x;
		m_info->extent.height = layout.extent->y;
		m_info->extent.depth = layout.extent->z;
		m_info->arrayLayers = layout.layers;
		m_info->mipLevels = layout.levels;
		m_info->format = VkFormat( layout.format );

		update( m_defaultView
			, m_info->extent
			, m_info->format
			, m_info->mipLevels
			, m_info->arrayLayers );
		update( m_arrayView
			, m_info->extent
			, m_info->format
			, m_info->mipLevels );
		update( m_cubeView
			, m_info->extent
			, m_info->format
			, m_info->mipLevels );
		update( m_sliceView
			, m_info->extent
			, m_info->format
			, m_info->mipLevels );

		doUpdateViews();
	}

	void TextureLayout::doUpdateFromFirstImage( uint32_t mipLevel
		, castor::ImageLayout layout )
	{
		using ashes::operator==;
		layout.extent->x <<= mipLevel;
		layout.extent->y <<= mipLevel;
		auto changed = m_image.updateLayerLayout( layout.dimensions(), layout.format )
			|| m_info->extent == VkExtent3D{}
			|| m_info->extent.width != layout.extent->x
			|| m_info->extent.height != layout.extent->y
			|| m_info->extent.depth != layout.extent->z
			|| m_info->format != VkFormat( layout.format );

		if ( changed )
		{
			VkImage texture = VK_NULL_HANDLE;

			if ( m_texture )
			{
				texture = *m_texture;
			}

			m_info->extent.width = layout.extent->x;
			m_info->extent.height = layout.extent->y;
			m_info->extent.depth = layout.extent->z;
			m_info->format = VkFormat( layout.format );
			doUpdateViews();

			update( m_defaultView
				, texture
				, 0u
				, m_info->arrayLayers
				, 0u
				, m_info->mipLevels );

			update( m_arrayView
				, texture
				, m_info->mipLevels );
			update( m_cubeView
				, texture
				, m_info->mipLevels );
			update( m_sliceView
				, texture
				, m_info->mipLevels );
		}
	}

	void TextureLayout::doUpdateMips( bool genNeeded, uint32_t mipLevels )
	{
		if ( getDepth() <= 1u && getLayersCount() <= 1u )
		{
			updateMipLevels( genNeeded, mipLevels, m_info->mipLevels, m_defaultView );
		}
	}

	void TextureLayout::doUpdateLayerMip( bool genNeeded, uint32_t layer, uint32_t level )
	{
		if ( isCube() )
		{
			getLayerCube( layer / 6u ).faces[layer % 6u].levels[level]->setMipmapsGenerationNeeded( genNeeded );
		}
		else if ( getDepth() > 1u )
		{
			getSlice( layer ).levels[level]->setMipmapsGenerationNeeded( genNeeded );
		}
		else
		{
			getLayer2D( layer ).levels[level]->setMipmapsGenerationNeeded( genNeeded );
		}
	}

	void TextureLayout::doUpdateLayerMips( bool genNeeded, uint32_t layer, uint32_t mipLevels )
	{
		if ( isCube() )
		{
			updateMipLevels( genNeeded
				, mipLevels
				, getLayerCube( layer / 6u ).faces[layer % 6u] );
		}
		else if ( getDepth() > 1u )
		{
			updateMipLevels( genNeeded
				, mipLevels
				, getSlice( layer ) );
		}
		else
		{
			updateMipLevels( genNeeded
				, mipLevels
				, getLayer2D( layer ) );
		}
	}

	//************************************************************************************************
}
