#include "Castor3D/Material/Texture/TextureLayout.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Material/Texture/TextureSource.hpp"
#include "Castor3D/Material/Texture/TextureView.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

#include <CastorUtils/Miscellaneous/BitSize.hpp>
#include <CastorUtils/Miscellaneous/Hash.hpp>
#include <CastorUtils/Graphics/ImageCache.hpp>
#include <CastorUtils/Graphics/PixelBufferBase.hpp>
#include <CastorUtils/Graphics/Size.hpp>

#include <ashes/ashes.hpp>
#include <ashespp/Command/CommandBuffer.hpp>
#include <ashespp/Image/Image.hpp>
#include <ashespp/Image/ImageView.hpp>
#include <ashespp/Sync/Fence.hpp>

CU_ImplementSmartPtr( castor3d, TextureLayout )
CU_ImplementSmartPtr( castor3d, TextureSource )

namespace castor3d
{
	//************************************************************************************************

	namespace texlayt
	{
		static VkImageViewType getSubviewType( VkImageType type
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

		static ashes::ImageViewCreateInfo getSubviewCreateInfos( ashes::ImageCreateInfo const & info
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

		static TextureViewUPtr createSubview( TextureLayout & layout
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

		static uint32_t getMinMipLevels( uint32_t mipLevels
			, VkExtent3D const & extent
			, VkFormat format )
		{
			return std::min( getMipLevels( extent, format ), mipLevels );
		}

		static uint32_t adjustMipLevels( uint32_t mipLevels
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

		static bool eraseViews( uint32_t mipLevels
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
		
		static bool eraseViews( uint32_t mipLevels
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
		static void eraseViews( uint32_t mipLevels
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
		static void eraseViews( uint32_t mipLevels
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

		static void createViews( ashes::ImageCreateInfo const & info
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

		static void createViews( ashes::ImageCreateInfo const & info
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

		static void createViews( ashes::ImageCreateInfo const & info
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
		static void createViews( ashes::ImageCreateInfo const & info
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
		static void createViews( ashes::ImageCreateInfo const & info
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

		static MipView createViews( ashes::ImageCreateInfo const & info
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

		static void update( MipView & view
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

		static void update( MipView & view
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

		static void update( CubeView & view
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
		static void update( ArrayView< ViewT > & view
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
		static void update( SliceView< ViewT > & view
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

		static void update( MipView & view
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

		static void update( MipView & view
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

		static void update( CubeView & view
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
		static void update( ArrayView< ViewT > & view
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
		static void update( SliceView< ViewT > & view
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

		static castor::PxBufferBaseUPtr adaptBuffer( castor::PxBufferBaseRPtr buffer
			, uint32_t mipLevels )
		{
			auto dstFormat = buffer->getFormat();
			auto result = buffer->clone();

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

			if ( result->getFormat() != dstFormat )
			{
				auto flipped = result->isFlipped();
				result = castor::PxBufferBase::create( result->getDimensions()
					, result->getLayers()
					, result->getLevels()
					, dstFormat
					, result->getConstPtr()
					, result->getFormat()
					, result->getAlign() );

				if ( flipped )
				{
					result->flip();
				}
			}

			result->update( result->getLayers()
				, mipLevels );
			return result;
		}

		castor::Image getFileImage( Engine & engine
			, castor::String const & name
			, castor::Path const & folder
			, castor::Path const & relative
			, uint32_t mipLevels
			, uint32_t & srcMipLevels
			, castor::ImageLoaderConfig config )
		{
			auto & image = castor3d::getFileImage( engine, name, folder, relative );
			auto buffer = texlayt::adaptBuffer( image.getPixels(), mipLevels );

			if ( !buffer )
			{
				CU_LoaderError( "Couldn't adapt image buffer." );
			}

			srcMipLevels = buffer->getLevels();
			castor::ImageLayout layout{ image.getLayout().type, * buffer };
			return castor::Image{ name
				, folder / relative
				, layout
				, std::move( buffer ) };
		}

		static auto updateMipLevels( bool genNeeded
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

		static auto updateMipLevels( bool genNeeded
			, uint32_t mipLevels
			, MipView & mipView )
		{
			updateMipLevels( genNeeded
				, mipLevels
				, mipView.view->getLevelCount()
				, mipView );
		}

		static castor::ImageLayout::Type convert( VkImageCreateFlags flags
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

		static castor::ImageLayout convert( ashes::ImageCreateInfo const & value )
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

		static VkImageType convert( castor::ImageLayout::Type type )
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
	}

	//************************************************************************************************

	void uploadTexture( RenderDevice const & device
		, QueueData const & queueData
		, castor::Image const & image
		, ashes::Image const & texture
		, ashes::ImageViewCreateInfo & viewInfo )
	{
		bool is3D = image.getLayout().type == castor::ImageLayout::e3D;
		auto & layout = image.getLayout();
		auto mappedSize = ashes::getAlignedSize( image.getPxBuffer().getSize()
			, device.renderSystem.getValue( GpuMin::eBufferMapSize ) );
		auto buffer = makeBufferBase( device
			, mappedSize
			, VK_BUFFER_USAGE_TRANSFER_SRC_BIT
			, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			, image.getName() + "StagingBuffer" );

		if ( auto data = buffer->lock( 0u, mappedSize, 0u ) )
		{
			std::memcpy( data, image.getPxBuffer().getConstPtr(), size_t( image.getPxBuffer().getSize() ) );
			buffer->flush( 0u, mappedSize );
			buffer->unlock();
		}
		else
		{
			return;
		}

		ashes::VkBufferImageCopyArray copies;
		VkExtent3D baseDimensions{ image.getWidth(), image.getHeight(), image.getLayout().depthLayers() };

		for ( auto layer = viewInfo->subresourceRange.baseArrayLayer;
			layer < viewInfo->subresourceRange.baseArrayLayer + viewInfo->subresourceRange.layerCount;
			++layer )
		{
			VkImageSubresourceLayers subresourceLayers{ viewInfo->subresourceRange.aspectMask
				, 0u
				, ( is3D ? 0u : layer )
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
					, VkOffset3D{ 0
					, 0
					, int32_t( is3D ? std::max( 1u, layer >> level ) : 0u ) }
					, VkExtent3D{ std::max( 1u, baseDimensions.width >> level )
					, std::max( 1u, baseDimensions.height >> level )
					, 1u } } );
			}
		}

		if ( is3D )
		{
			viewInfo->viewType = VK_IMAGE_VIEW_TYPE_3D;
			viewInfo->subresourceRange.layerCount = 1u;
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
		, m_image{ debugName, castor::Path{}, texlayt::convert( m_info ) }
		, m_defaultView{ texlayt::createViews( m_info, *this, m_image.getName() ) }
		, m_arrayView{ &m_defaultView }
		, m_cubeView{ &m_defaultView }
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
				texlayt::createViews( m_info, *this, m_image.getName(), m_cubeView );
			}
			else
			{
				m_arrayView.layers.resize( m_info->arrayLayers );
				texlayt::createViews( m_info, *this, m_image.getName(), m_arrayView );
			}
		}
		else if ( m_info->extent.depth > 1u )
		{
			m_sliceView.slices.resize( m_info->extent.depth );
			texlayt::createViews( m_info, *this, m_image.getName(), m_sliceView );
		}
	}

	TextureLayout::TextureLayout( RenderSystem & renderSystem
		, castor::String const & name
		, VkImage image
		, crg::ImageViewId imageView )
		: TextureLayout{ renderSystem
			, std::make_unique< ashes::Image >( *renderSystem.getRenderDevice()
				, name
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
		, m_defaultView{ texlayt::createViews( m_info, *this, m_image.getName() ) }
		, m_arrayView{ &m_defaultView }
		, m_cubeView{ &m_defaultView }
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
				texlayt::createViews( m_info, *this, m_image.getName(), m_cubeView );
			}
			else
			{
				m_arrayView.layers.resize( m_info->arrayLayers );
				texlayt::createViews( m_info, *this, m_image.getName(), m_arrayView );
			}
		}
		else if ( m_info->extent.depth > 1u )
		{
			m_sliceView.slices.resize( m_info->extent.depth );
			texlayt::createViews( m_info, *this, m_image.getName(), m_sliceView );
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

			if ( castor::checkFlag( props.optimalTilingFeatures, VK_FORMAT_FEATURE_BLIT_DST_BIT ) )
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

			if ( castor::checkFlag( props.optimalTilingFeatures, VK_FORMAT_FEATURE_BLIT_SRC_BIT ) )
			{
				res = device->getPhysicalDevice().getImageFormatProperties( m_info->format
					, m_info->imageType
					, m_info->tiling
					, m_info->usage | VK_IMAGE_USAGE_TRANSFER_SRC_BIT
					, m_info->flags
					, imageProps );

				if ( res == VK_SUCCESS )
				{
					m_info->usage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
				}
				else if ( m_defaultView.view->isMipmapsGenerationNeeded() )
				{
					m_info->mipLevels = 1u;
				}
			}
			else if ( m_defaultView.view->isMipmapsGenerationNeeded() )
			{
				m_info->mipLevels = 1u;
			}

			if ( m_info->mipLevels == 0 )
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
				uploadTexture( device, queueData, m_image, *m_texture, viewInfo );
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

	void TextureLayout::generateMipmaps( QueueData const & queueData
		, VkImageLayout srcLayout )const
	{
		if ( m_info->mipLevels > 1u
			&& getDefaultView().isMipmapsGenerationNeeded() )
		{
			CU_Require( m_texture );
			auto commandBuffer = queueData.commandPool->createCommandBuffer( "TextureGenMipmaps" );
			commandBuffer->begin();
			commandBuffer->beginDebugBlock( { getName() + " Mipmaps Generation"
				, makeFloatArray( getRenderSystem()->getEngine()->getNextRainbowColour() ) } );
			generateMipmaps( *commandBuffer
				, srcLayout );
			commandBuffer->endDebugBlock();
			commandBuffer->end();
			queueData.queue->submit( *commandBuffer, nullptr );
			queueData.queue->waitIdle();
		}
	}

	void TextureLayout::generateMipmaps( RenderDevice const & device
		, VkImageLayout srcLayout )const
	{
		generateMipmaps( *device.graphicsData()
			, srcLayout );
	}

	void TextureLayout::generateMipmaps( ashes::CommandBuffer & cmd
		, VkImageLayout srcLayout )const
	{
		CU_Require( m_texture );
		m_texture->generateMipmaps( cmd
			, srcLayout
			, VK_IMAGE_LAYOUT_UNDEFINED
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
	}

	void TextureLayout::setSource( castor::Path const & folder
		, castor::Path const & relative
		, castor::ImageLoaderConfig config )
	{
		uint32_t srcMips = 1u;
		m_image = texlayt::getFileImage( *getRenderSystem()->getEngine()
			, m_image.getName()
			, folder
			, relative
			, m_image.getLevels()
			, srcMips
			, std::move( config ) );
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

	void TextureLayout::setSource( castor::PxBufferBaseUPtr buffer
		, uint32_t bufferOrigLevels
		, bool isStatic )
	{
		buffer = texlayt::adaptBuffer( buffer.get(), buffer->getLevels() );
		auto layout = castor::ImageLayout{ *buffer };
		m_image = { m_image.getName(), layout, std::move( buffer ) };
		doUpdateCreateInfo( m_image.getLayout() );
		doUpdateMips( false, bufferOrigLevels );
		m_static = isStatic;
	}

	void TextureLayout::setSource( castor::PxBufferBaseUPtr buffer
		, bool isStatic )
	{
		setSource( std::move( buffer )
			, m_image.getPixels()->getLevels()
			, isStatic );
	}

	castor::String TextureLayout::getDefaultSourceString()const
	{
		return getDefaultView().toString();
	}

	ashes::ImageView const & TextureLayout::getDefaultSampledView()const noexcept
	{
		CU_Require( m_defaultView.view );
		return m_defaultView.view->getSampledView();
	}

	ashes::ImageView const & TextureLayout::getDefaultTargetView()const noexcept
	{
		CU_Require( m_defaultView.view );
		return m_defaultView.view->getTargetView();
	}

	ashes::ImageView const & TextureLayout::getLayerCubeTargetView( size_t layer )const noexcept
	{
		CU_Require( getLayerCube( layer ).view.view );
		return getLayerCube( layer ).view.view->getTargetView();
	}

	castor::String TextureLayout::getLayerCubeSourceString( size_t layer )const noexcept
	{
		CU_Require( getLayerCube( layer ).view.view );
		return getLayerCube( layer ).view.view->toString();
	}

	ashes::ImageView const & TextureLayout::getLayerCubeFaceTargetView( size_t layer
		, CubeMapFace face )const noexcept
	{
		CU_Require( getLayerCubeFace( layer, face ).view );
		return getLayerCubeFace( layer, face ).view->getTargetView();
	}

	castor::String TextureLayout::getLayerCubeFaceSourceString( size_t layer
		, CubeMapFace face )const noexcept
	{
		CU_Require( getLayerCubeFace( layer, face ).view );
		return getLayerCubeFace( layer, face ).view->toString();
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
		if ( getDefaultView().needsYInversion()
			&& m_image.getPixels()->isFlipped() )
		{
			return false;
		}

		return getDefaultView().needsYInversion()
			|| m_image.getPixels()->isFlipped();
	}

	bool TextureLayout::hasBuffer()const
	{
		return getDefaultView().hasBuffer();
	}

	uint32_t TextureLayout::doUpdateViews()
	{
		auto mipLevels = m_info->mipLevels;

		if ( mipLevels > 1u )
		{
			m_info->mipLevels = texlayt::getMinMipLevels( mipLevels, m_info->extent, m_info->format );
		}

		texlayt::eraseViews( m_info->mipLevels, m_defaultView );
		m_info->mipLevels = std::min( m_info->mipLevels
			, ( getDefaultView().getLevelCount() > 1u
				? getDefaultView().getLevelCount()
				: m_info->mipLevels ) );
		texlayt::eraseViews( m_info->mipLevels, m_arrayView );
		texlayt::eraseViews( m_info->mipLevels, m_cubeView );
		texlayt::eraseViews( m_info->mipLevels, m_sliceView );
		m_image.getLayout().levels = m_info->mipLevels;
		return mipLevels;
	}

	void TextureLayout::doUpdateCreateInfo( castor::ImageLayout const & layout )
	{
		auto layersDepth = std::max( layout.extent->z, layout.layers );
		auto layoutType = ( m_info->imageType == VK_IMAGE_TYPE_3D && layersDepth > 1u )
			? castor::ImageLayout::e3D
			: layout.type;
		m_image.getLayout().type = layoutType;
		m_image.getLayout().extent->z = ( layoutType == castor::ImageLayout::e3D ? layersDepth : 1u );
		m_image.getLayout().layers = ( layoutType == castor::ImageLayout::e3D ? 1u : layersDepth );

		m_info->imageType = texlayt::convert( layoutType );
		m_info->extent.width = layout.extent->x;
		m_info->extent.height = layout.extent->y;
		m_info->extent.depth = ( m_info->imageType == VK_IMAGE_TYPE_3D ? layersDepth : 1u );
		m_info->arrayLayers = ( m_info->imageType == VK_IMAGE_TYPE_3D ? 1u : layersDepth );
		m_info->mipLevels = layout.levels;
		m_info->format = VkFormat( layout.format );

		texlayt::update( m_defaultView
			, m_info->extent
			, m_info->format
			, m_info->mipLevels
			, m_info->arrayLayers );
		texlayt::update( m_arrayView
			, m_info->extent
			, m_info->format
			, m_info->mipLevels );
		texlayt::update( m_cubeView
			, m_info->extent
			, m_info->format
			, m_info->mipLevels );
		texlayt::update( m_sliceView
			, m_info->extent
			, m_info->format
			, m_info->mipLevels );

		doUpdateViews();
	}

	void TextureLayout::doUpdateMips( bool genNeeded, uint32_t mipLevels )
	{
		if ( getDepth() <= 1u && getLayersCount() <= 1u )
		{
			texlayt::updateMipLevels( genNeeded, mipLevels, m_info->mipLevels, m_defaultView );
		}
	}

	//************************************************************************************************
}
