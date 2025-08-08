#include "Castor3D/Material/Texture/TextureLayout.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Buffer/UploadData.hpp"
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

CU_ImplementSmartPtr( c3d, TextureLayout )
CU_ImplementSmartPtr( c3d, TextureSource )

namespace c3d
{
	//************************************************************************************************

	namespace texlayt
	{
		static ImageViewType getSubviewType( crg::ImageType type
			, crg::ImageCreateFlags flags
			, uint32_t arrayLayers )
		{
			crg::ImageType result = type;

			switch ( result )
			{
			case crg::ImageType::e1D:
				if ( arrayLayers > 1 )
				{
					return ImageViewType::e1DArray;
				}
				return ImageViewType::e1D;

			case crg::ImageType::e2D:
				if ( arrayLayers > 1 )
				{
					if ( checkFlag( flags, ImageCreateFlags::eCubeCompatible ) )
					{
						CU_Require( ( arrayLayers % 6 ) == 0 );
						return arrayLayers == 6u
							? ImageViewType::eCube
							: ImageViewType::eCubeArray;
					}

					return ImageViewType::e2DArray;
				}

				return ImageViewType::e2D;

			case crg::ImageType::e3D:
				return ImageViewType::e3D;

			default:
				CU_Failure( "Unsupported texture type." );
				return ImageViewType::e2D;
			}
		}

		static ImageViewCreateInfo getSubviewCreateInfos( ImageCreateInfo const & info
			, VkImage image
			, uint32_t baseMipLevel
			, uint32_t levelCount
			, uint32_t baseArrayLayer
			, uint32_t arrayLayers )
		{
			ImageViewCreateInfo view{ ImageViewCreateFlags::eNone
				, getSubviewType( info.imageType, info.flags, arrayLayers )
				, info.format
				, { getAspectMask( info.format )
					, baseMipLevel
					, levelCount
					, baseArrayLayer
					, arrayLayers }
			};
			return view;
		}

		static TextureViewUPtr createSubview( TextureLayout & layout
			, String debugName
			, ImageCreateInfo const & info
			, uint32_t baseMipLevel
			, uint32_t levelCount
			, uint32_t baseArrayLayer
			, uint32_t arrayLayers )
		{
			return makeUnique< TextureView >( layout
				, getSubviewCreateInfos( info
					, VK_NULL_HANDLE
					, baseMipLevel
					, levelCount
					, baseArrayLayer
					, arrayLayers )
				, 0u
				, c3d::move( debugName ) );
		}

		static uint32_t getMinMipLevels( uint32_t mipLevels
			, Extent3D const & extent
			, PixelFormat format )
		{
			return std::min( getMipLevels( extent, format ), mipLevels );
		}

		static uint32_t adjustMipLevels( uint32_t mipLevels
			, PixelFormat format )
		{
			if ( format == PixelFormat::eUNDEFINED )
			{
				return mipLevels;
			}

			auto blockSize = ashes::getBlockSize( convert( format ) );
			auto bitSize = getBitSize( blockSize.extent.width );
			return mipLevels / bitSize;
		}

		static bool eraseViews( uint32_t mipLevels
			, ImageMipView & views )
		{
			auto itLevel = views.levels.begin();

			while ( itLevel != views.levels.end() )
			{
				if ( auto const & level = *itLevel;
					level->getBaseMipLevel() >= mipLevels )
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
			, CubeImageView & views )
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
			, ArrayImageView< ViewT > & views )
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
			, ImageSliceView< ViewT > & views )
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

		static void createViews( ImageCreateInfo const & info
			, String const & debugName
			, TextureLayout & layout
			, ImageMipView & view
			, uint32_t baseArrayLayer
			, uint32_t layerCount )
		{
			view.view = createSubview( layout
				, debugName
				, info
				, 0u
				, adjustMipLevels( info.mipLevels, info.format )
				, baseArrayLayer
				, layerCount );

			if ( info.mipLevels > 1 )
			{
				uint32_t levelIndex = 0u;
				view.levels.resize( info.mipLevels );

				for ( auto & level : view.levels )
				{
					level = createSubview( layout
						, debugName
						, info
						, levelIndex
						, 1u
						, baseArrayLayer
						, layerCount );
					++levelIndex;
				}
			}
		}

		static void createViews( ImageCreateInfo const & info
			, String const & debugName
			, TextureLayout & layout
			, ImageMipView & view
			, uint32_t & baseArrayLayer )
		{
			createViews( info
				, debugName
				, layout
				, view
				, baseArrayLayer
				, 1u );
			++baseArrayLayer;
		}

		static void createViews( ImageCreateInfo const & info
			, String const & debugName
			, TextureLayout & layout
			, CubeImageView & view
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
		static void createViews( ImageCreateInfo const & info
			, TextureLayout & layout
			, String debugName
			, ArrayImageView< ViewT > & view )
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
		static void createViews( ImageCreateInfo const & info
			, TextureLayout & layout
			, String debugName
			, ImageSliceView< ViewT > & view )
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

		static ImageMipView createViews( ImageCreateInfo const & info
			, TextureLayout & layout
			, String const & debugName )
		{
			ImageMipView result;
			createViews( info
				, debugName
				, layout
				, result
				, 0u
				, info.arrayLayers );
			return result;
		}

		static void update( ImageMipView & view
			, Extent3D const & extent
			, PixelFormat format
			, uint32_t mipLevels
			, uint32_t arrayLayers )
		{
			view.view->update( extent
				, format
				, mipLevels
				, arrayLayers );

			for ( auto const & level : view.levels )
			{
				level->update( extent
					, format
					, 1u
					, 1u );
			}
		}

		static void update( ImageMipView & view
			, Extent3D const & extent
			, PixelFormat format
			, uint32_t mipLevels )
		{
			update( view
				, extent
				, format
				, 1u
				, mipLevels );
		}

		static void update( CubeImageView & view
			, Extent3D const & extent
			, PixelFormat format
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
		static void update( ArrayImageView< ViewT > & view
			, Extent3D const & extent
			, PixelFormat format
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
		static void update( ImageSliceView< ViewT > & view
			, Extent3D const & extent
			, PixelFormat format
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

		static PxBufferBaseUPtr adaptBuffer( PxBufferBase const * buffer
			, uint32_t mipLevels )
		{
			auto dstFormat = buffer->getFormat();
			auto result = buffer->clone();

			switch ( dstFormat )
			{
			case PixelFormat::eR8G8B8_UNORM:
				dstFormat = PixelFormat::eR8G8B8A8_UNORM;
				break;
			case PixelFormat::eB8G8R8_UNORM:
				dstFormat = PixelFormat::eA8B8G8R8_UNORM;
				break;
			case PixelFormat::eR8G8B8_SRGB:
				dstFormat = PixelFormat::eR8G8B8A8_SRGB;
				break;
			case PixelFormat::eB8G8R8_SRGB:
				dstFormat = PixelFormat::eA8B8G8R8_SRGB;
				break;
			case PixelFormat::eR16G16B16_SFLOAT:
				dstFormat = PixelFormat::eR16G16B16A16_SFLOAT;
				break;
			case PixelFormat::eR32G32B32_SFLOAT:
				dstFormat = PixelFormat::eR32G32B32A32_SFLOAT;
				break;
			default:
				// No conversion
				break;
			}

			if ( result->getFormat() != dstFormat )
			{
				auto invertX = result->isXInverted();
				auto invertY = result->isYInverted();
				auto invertZ = result->isZInverted();
				result = PxBufferBase::create( result->getDimensions()
					, result->getLayers()
					, result->getLevels()
					, dstFormat
					, result->getConstPtr()
					, result->getFormat()
					, result->getAlign() );

				if ( invertX )
					result->invertX();
				if ( invertY )
					result->invertY();
				if ( invertZ )
					result->invertZ();
			}

			result->update( result->getLayers()
				, mipLevels );
			return result;
		}

		static Image getFileImage( Engine & engine
			, String const & name
			, Path const & folder
			, Path const & relative
			, uint32_t mipLevels
			, uint32_t & srcMipLevels )
		{
			auto & image = getFileImage( engine, name, folder, relative );
			auto buffer = texlayt::adaptBuffer( image.getPixels(), mipLevels );

			if ( !buffer )
			{
				CU_LoaderError( "Couldn't adapt image buffer." );
			}

			srcMipLevels = buffer->getLevels();
			ImageMemoryLayout layout{ image.getLayout().type, *buffer };
			return Image{ name
				, folder / relative
				, layout
				, c3d::move( buffer ) };
		}

		static auto updateMipLevels( bool genNeeded
			, uint32_t mipLevels
			, uint32_t imgMipLevels
			, ImageMipView & mipView )
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

		static ImageViewType convert( crg::ImageCreateFlags flags
			, Extent3D const & extent
			, uint32_t arrayLayers )
		{
			if ( extent.depth > 1u )
			{
				return ImageViewType::e3D;
			}

			if ( extent.height > 1u || extent.width <= 1u )
			{
				if ( checkFlag( flags, ImageCreateFlags::eCubeCompatible ) )
				{
					if ( arrayLayers == 6u )
					{
						return ImageViewType::eCube;
					}

					if ( arrayLayers > 6u && ( arrayLayers % 6u == 0u ) )
					{
						return ImageViewType::eCubeArray;
					}
				}

				if ( arrayLayers > 1u )
				{
					return ImageViewType::e2DArray;
				}

				return ImageViewType::e2D;
			}

			if ( arrayLayers > 1u )
			{
				return ImageViewType::e1DArray;
			}

			return ImageViewType::e1D;
		}

		static ImageMemoryLayout convert( ImageCreateInfo const & value )
		{
			return ImageMemoryLayout
			{
				convert( value.flags, value.extent, value.arrayLayers ),
				PixelFormat( value.format ),
				Point3ui{ value.extent.width, value.extent.height, value.extent.depth },
				0u,
				value.arrayLayers,
				0u,
				value.mipLevels,
			};
		}

		static ImageType convert( ImageViewType type )
		{
			switch ( type )
			{
			case ImageViewType::e1D:
			case ImageViewType::e1DArray:
				return ImageType::e1D;
			case ImageViewType::e2D:
			case ImageViewType::eCube:
			case ImageViewType::e2DArray:
			case ImageViewType::eCubeArray:
				return ImageType::e2D;
			case ImageViewType::e3D:
				return ImageType::e3D;
			default:
				CU_Failure( "Unexpected ImageViewType" );
				return ImageType::e2D;
			}
		}
	}

	//************************************************************************************************

	TextureLayout::TextureLayout( RenderSystem & renderSystem
		, ImageCreateInfo info
		, MemoryPropertyFlags memoryProperties
		, String const & debugName
		, bool isStatic )
		: OwnedBy< RenderSystem >{ renderSystem }
		, m_static{ isStatic }
		, m_info{ c3d::move( info ) }
		, m_properties{ memoryProperties }
		, m_image{ debugName, Path{}, texlayt::convert( m_info ) }
		, m_defaultView{ texlayt::createViews( m_info, *this, m_image.getName() ) }
		, m_arrayView{ &m_defaultView }
		, m_cubeView{ &m_defaultView }
		, m_sliceView{ &m_defaultView }
	{
		m_info.mipLevels = std::max( 1u, m_info.mipLevels );

		if ( m_info.arrayLayers > 1u )
		{
			if ( isCube() )
			{
				m_cubeView.layers.resize( m_info.arrayLayers / 6u );
				texlayt::createViews( m_info, *this, m_image.getName(), m_cubeView );
			}
			else
			{
				m_arrayView.layers.resize( m_info.arrayLayers );
				texlayt::createViews( m_info, *this, m_image.getName(), m_arrayView );
			}
		}
		else if ( m_info.extent.depth > 1u )
		{
			m_sliceView.slices.resize( m_info.extent.depth );
			texlayt::createViews( m_info, *this, m_image.getName(), m_sliceView );
		}
	}

	TextureLayout::TextureLayout( RenderSystem & renderSystem
		, String const & name
		, VkImage image
		, crg::ImageViewId imageView )
		: TextureLayout{ renderSystem
			, makeRawUnique< ashes::Image >( *renderSystem.getRenderDevice()
				, toUtf8( name )
				, image
				, ashes::ImageCreateInfo{ convert( imageView.data->image.data->info ) } )
			, imageView.data->image.data->info }
	{
	}

	TextureLayout::TextureLayout( RenderSystem & renderSystem
		, ashes::ImagePtr image
		, ImageCreateInfo const & createInfo )
		: OwnedBy< RenderSystem >{ renderSystem }
		, m_info{ createInfo }
		, m_properties{}
		, m_image{ cuEmptyString
			, Path{ cuEmptyString }
			, makeSize( m_info.extent )
			, PixelFormat( m_info.format ) }
		, m_defaultView{ texlayt::createViews( m_info, *this, m_image.getName() ) }
		, m_arrayView{ &m_defaultView }
		, m_cubeView{ &m_defaultView }
		, m_sliceView{ &m_defaultView }
		, m_ownTexture{ c3d::move( image ) }
		, m_texture{ m_ownTexture.get() }
	{
		m_info.mipLevels = std::max( 1u, m_info.mipLevels );

		if ( m_info.arrayLayers > 1u )
		{
			if ( isCube() )
			{
				m_cubeView.layers.resize( m_info.arrayLayers / 6u );
				texlayt::createViews( m_info, *this, m_image.getName(), m_cubeView );
			}
			else
			{
				m_arrayView.layers.resize( m_info.arrayLayers );
				texlayt::createViews( m_info, *this, m_image.getName(), m_arrayView );
			}
		}
		else if ( m_info.extent.depth > 1u )
		{
			m_sliceView.slices.resize( m_info.extent.depth );
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

	bool TextureLayout::initialise( RenderDevice const & device )
	{
		if ( !m_initialised )
		{
			auto props = device->getPhysicalDevice().getFormatProperties( convert( m_info.format ) );
			VkImageFormatProperties imageProps{};
			auto res = device->getPhysicalDevice().getImageFormatProperties( convert( m_info.format )
				, convert( m_info.imageType )
				, convert( m_info.tiling )
				, getImageUsageFlags( m_info.usage )
				, getImageCreateFlags( m_info.flags )
				, imageProps );

			if ( res != VK_SUCCESS )
			{
				CU_Exception( "Unsupported image format properties" );
			}

			if ( checkFlag( props.optimalTilingFeatures, VK_FORMAT_FEATURE_BLIT_DST_BIT ) )
			{
				res = device->getPhysicalDevice().getImageFormatProperties( convert( m_info.format )
					, convert( m_info.imageType )
					, convert( m_info.tiling )
					, getImageUsageFlags( m_info.usage | ImageUsageFlags::eTransferDst )
					, getImageCreateFlags( m_info.flags )
					, imageProps );

				if ( res == VK_SUCCESS )
					m_info.usage |= ImageUsageFlags::eTransferDst;
			}

			if ( checkFlag( props.optimalTilingFeatures, VK_FORMAT_FEATURE_BLIT_SRC_BIT ) )
			{
				res = device->getPhysicalDevice().getImageFormatProperties( convert( m_info.format )
					, convert( m_info.imageType )
					, convert( m_info.tiling )
					, getImageUsageFlags( m_info.usage | ImageUsageFlags::eTransferSrc )
					, getImageCreateFlags( m_info.flags )
					, imageProps );

				if ( res == VK_SUCCESS )
					m_info.usage |= ImageUsageFlags::eTransferSrc;
				else if ( m_defaultView.view->isMipmapsGenerationNeeded() )
					m_info.mipLevels = 1u;
			}
			else if ( m_defaultView.view->isMipmapsGenerationNeeded() )
			{
				m_info.mipLevels = 1u;
			}

			if ( m_info.mipLevels == 0 )
			{
				m_info.mipLevels = 1u;
			}

			if ( !m_ownTexture )
			{
				m_ownTexture = makeImage( device
					, m_info
					, m_properties
					, m_image.getName() );
			}

			m_texture = m_ownTexture.get();
			CU_Require( m_info.mipLevels <= 1u
				|| ( checkFlag( props.optimalTilingFeatures, VK_FORMAT_FEATURE_BLIT_DST_BIT )
					|| !m_defaultView.view->isMipmapsGenerationNeeded() ) );

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

	void TextureLayout::upload( UploadData & uploader )
	{
		if ( m_texture && isStatic() )
		{
			ImageSubresourceRange subresourceRange{ getAspectMask( m_info.format )
				, 0u
				, 1u
				, 0u
				, m_image.getLayout().depthLayers() };
			uint32_t mipLevels = m_defaultView.view->isMipmapsGenerationNeeded()
				? 1u
				: m_image.getLayout().levels;
			subresourceRange.levelCount = mipLevels;
			uploader.pushUpload( m_image.getPxBuffer().getConstPtr()
				, m_image.getPxBuffer().getSize()
				, *m_texture
				, m_image.getLayout()
				, subresourceRange
				, ImageLayout::eShaderReadOnly
				, PipelineStageFlags::eFragmentShader );
		}
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
		, ImageLayout srcLayout )const
	{
		if ( m_info.mipLevels > 1u
			&& getDefaultView().isMipmapsGenerationNeeded() )
		{
			CU_Require( m_texture );
			auto commandBuffer = queueData.commandPool->createCommandBuffer( "TextureGenMipmaps" );
			commandBuffer->begin();
			commandBuffer->beginDebugBlock( { toUtf8( getName() ) + " Mipmaps Generation"
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
		, ImageLayout srcLayout )const
	{
		generateMipmaps( *device.graphicsData()
			, srcLayout );
	}

	void TextureLayout::generateMipmaps( ashes::CommandBuffer const & cmd
		, ImageLayout srcLayout )const
	{
		CU_Require( m_texture );
		m_texture->generateMipmaps( cmd
			, convert( srcLayout )
			, convert( ImageLayout::eUndefined )
			, convert( ImageLayout::eShaderReadOnly ) );
	}

	void TextureLayout::setSource( Path const & folder
		, Path const & relative )
	{
		uint32_t srcMips = 1u;
		m_image = texlayt::getFileImage( *getRenderSystem()->getEngine()
			, m_image.getName()
			, folder
			, relative
			, m_image.getLevels()
			, srcMips );
		doUpdateCreateInfo( m_image.getLayout() );
		doUpdateMips( false, srcMips );
		m_static = true;
	}

	void TextureLayout::setSource( Extent3D const & extent
		, PixelFormat format )
	{
		setSource( PxBufferBase::create( { extent.width, extent.height }
			, extent.depth
			, 1u
			, PixelFormat( format ) ) );
		m_static = true;
	}

	void TextureLayout::setSource( PxBufferBaseUPtr buffer
		, uint32_t bufferOrigLevels
		, bool isStatic )
	{
		buffer = texlayt::adaptBuffer( buffer.get(), buffer->getLevels() );
		auto layout = ImageMemoryLayout{ *buffer };
		m_image = { m_image.getName(), layout, c3d::move( buffer ) };
		doUpdateCreateInfo( m_image.getLayout() );
		doUpdateMips( false, bufferOrigLevels );
		m_static = isStatic;
	}

	void TextureLayout::setSource( PxBufferBaseUPtr buffer
		, bool isStatic )
	{
		setSource( c3d::move( buffer )
			, m_image.getPixels()->getLevels()
			, isStatic );
	}

	String TextureLayout::getDefaultSourceString()const
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

	String TextureLayout::getLayerCubeSourceString( size_t layer )const noexcept
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

	String TextureLayout::getLayerCubeFaceSourceString( size_t layer
		, CubeMapFace face )const noexcept
	{
		CU_Require( getLayerCubeFace( layer, face ).view );
		return getLayerCubeFace( layer, face ).view->toString();
	}

	String TextureLayout::getName()const
	{
		return m_image.getName();
	}

	Path TextureLayout::getPath()const
	{
		return m_image.getPath();
	}

	bool TextureLayout::needsXInversion()const
	{
		if ( getDefaultView().needsXInversion()
			&& m_image.getPixels()->isXInverted() )
		{
			return false;
		}

		return getDefaultView().needsXInversion()
			|| m_image.getPixels()->isXInverted();
	}

	bool TextureLayout::needsYInversion()const
	{
		if ( getDefaultView().needsYInversion()
			&& m_image.getPixels()->isYInverted() )
		{
			return false;
		}

		return getDefaultView().needsYInversion()
			|| m_image.getPixels()->isYInverted();
	}

	bool TextureLayout::needsZInversion()const
	{
		if ( getDefaultView().needsZInversion()
			&& m_image.getPixels()->isZInverted() )
		{
			return false;
		}

		return getDefaultView().needsZInversion()
			|| m_image.getPixels()->isZInverted();
	}

	bool TextureLayout::hasBuffer()const
	{
		return getDefaultView().hasBuffer();
	}

	uint32_t TextureLayout::doUpdateViews()
	{
		auto mipLevels = m_info.mipLevels;
		if ( mipLevels > 1u )
			m_info.mipLevels = texlayt::getMinMipLevels( mipLevels, m_info.extent, m_info.format );

		texlayt::eraseViews( m_info.mipLevels, m_defaultView );
		m_info.mipLevels = std::min( m_info.mipLevels
			, ( getDefaultView().getLevelCount() > 1u
				? getDefaultView().getLevelCount()
				: m_info.mipLevels ) );
		texlayt::eraseViews( m_info.mipLevels, m_arrayView );
		texlayt::eraseViews( m_info.mipLevels, m_cubeView );
		texlayt::eraseViews( m_info.mipLevels, m_sliceView );
		m_image.getLayout().levels = m_info.mipLevels;
		return mipLevels;
	}

	void TextureLayout::doUpdateCreateInfo( ImageMemoryLayout const & layout )
	{
		auto layersDepth = std::max( layout.extent->z, layout.layers );
		auto layoutType = ( m_info.imageType == ImageType::e3D && layersDepth > 1u )
			? ImageViewType::e3D
			: layout.type;
		m_image.getLayout().type = layoutType;
		m_image.getLayout().extent->z = ( layoutType == ImageViewType::e3D ? layersDepth : 1u );
		m_image.getLayout().layers = ( layoutType == ImageViewType::e3D ? 1u : layersDepth );

		m_info.imageType = texlayt::convert( layoutType );
		m_info.extent.width = layout.extent->x;
		m_info.extent.height = layout.extent->y;
		m_info.extent.depth = ( m_info.imageType == ImageType::e3D ? layersDepth : 1u );
		m_info.arrayLayers = ( m_info.imageType == ImageType::e3D ? 1u : layersDepth );
		m_info.mipLevels = layout.levels;
		m_info.format = layout.format;

		texlayt::update( m_defaultView
			, m_info.extent
			, m_info.format
			, m_info.mipLevels
			, m_info.arrayLayers );
		texlayt::update( m_arrayView
			, m_info.extent
			, m_info.format
			, m_info.mipLevels );
		texlayt::update( m_cubeView
			, m_info.extent
			, m_info.format
			, m_info.mipLevels );
		texlayt::update( m_sliceView
			, m_info.extent
			, m_info.format
			, m_info.mipLevels );

		doUpdateViews();
	}

	void TextureLayout::doUpdateMips( bool genNeeded, uint32_t mipLevels )
	{
		if ( getDepth() <= 1u && getLayersCount() <= 1u )
		{
			texlayt::updateMipLevels( genNeeded, mipLevels, m_info.mipLevels, m_defaultView );
		}
	}

	//************************************************************************************************
}
