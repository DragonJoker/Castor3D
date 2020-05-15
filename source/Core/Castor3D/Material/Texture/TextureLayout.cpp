#include "Castor3D/Material/Texture/TextureLayout.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Texture/TextureSource.hpp"

#include <CastorUtils/Miscellaneous/BitSize.hpp>
#include <CastorUtils/Graphics/PixelBufferBase.hpp>
#include <CastorUtils/Graphics/Size.hpp>

#include <ashes/ashes.hpp>
#include <ashespp/Command/CommandBuffer.hpp>
#include <ashespp/Image/Image.hpp>
#include <ashespp/Image/ImageView.hpp>

using namespace castor;

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
					if ( checkFlag( flags, VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT ) )
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
			, uint32_t baseArrayLayer
			, uint32_t arrayLayers
			, uint32_t baseMipLevel
			, uint32_t levelCount )
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
			return std::make_unique< TextureView >( layout
				, getSubviewCreateInfos( info
					, VK_NULL_HANDLE
					, baseArrayLayer
					, arrayLayers
					, baseMipLevel
					, levelCount )
				, 0u
				, std::move( debugName ) );
		}

		uint32_t getMinMipLevels( uint32_t mipLevels
			, VkExtent3D const & extent )
		{
			return std::min( getMipLevels( extent ), mipLevels );
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
				, info->mipLevels
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
	}

	//************************************************************************************************

	TextureLayoutSPtr createTextureLayout( Engine const & engine
		, Path const & relative
		, Path const & folder )
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
		auto texture = std::make_shared < TextureLayout >( *engine.getRenderSystem()
			, createInfo
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			, relative );
		texture->setSource( folder
			, relative );
		return texture;
	}

	TextureLayoutSPtr createTextureLayout( Engine const & engine
		, String const & name
		, PxBufferBaseSPtr buffer )
	{
		ashes::ImageCreateInfo createInfo
		{
			0u,
			( buffer->getHeight() > 1u
				? VK_IMAGE_TYPE_2D
				: VK_IMAGE_TYPE_1D ),
			VK_FORMAT_UNDEFINED,
			{ buffer->getWidth(), buffer->getHeight(), 1u },
			uint32_t( castor::getBitSize( std::min( buffer->getWidth(), buffer->getHeight() ) ) ),
			1u,
			VK_SAMPLE_COUNT_1_BIT,
			VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
		};
		auto texture = std::make_shared < TextureLayout >( *engine.getRenderSystem()
			, createInfo
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			, name );
		texture->setSource( buffer );
		return texture;
	}

	uint32_t getMipLevels( VkExtent3D const & extent )
	{
		auto min = std::min( extent.width, extent.height );
		return uint32_t( castor::getBitSize( min ) );
	}

	//************************************************************************************************

	TextureLayout::TextureLayout( RenderSystem & renderSystem
		, ashes::ImageCreateInfo info
		, VkMemoryPropertyFlags memoryProperties
		, castor::String debugName )
		: OwnedBy< RenderSystem >{ renderSystem }
		, m_info{ std::move( info ) }
		, m_properties{ memoryProperties }
		, m_defaultView{ createViews( m_info, *this, debugName ) }
		, m_cubeView{ &m_defaultView }
		, m_arrayView{ &m_defaultView }
		, m_sliceView{ &m_defaultView }
		, m_debugName{ std::move( debugName ) }
	{
		m_info->mipLevels = std::max( 1u, m_info->mipLevels );

		if ( m_info->arrayLayers > 1u )
		{
			if ( m_info->arrayLayers >= 6u
				&& ( m_info->arrayLayers % 6u ) == 0u
				&& checkFlag( m_info->flags, VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT ) )
			{
				m_cubeView.layers.resize( m_info->arrayLayers / 6u );
				createViews( m_info, *this, m_debugName, m_cubeView );
			}
			else
			{
				m_arrayView.layers.resize( m_info->arrayLayers );
				createViews( m_info, *this, m_debugName, m_arrayView );
			}
		}
		else if ( m_info->extent.depth > 1u )
		{
			m_sliceView.slices.resize( m_info->extent.depth );
			createViews( m_info, *this, m_debugName, m_sliceView );
		}
	}

	TextureLayout::~TextureLayout()
	{
		m_sliceView.slices.clear();
		m_arrayView.layers.clear();
		m_cubeView.layers.clear();
		m_defaultView.levels.clear();
		m_defaultView.view.reset();
	}

	bool TextureLayout::initialise()
	{
		if ( !m_initialised )
		{
			auto & device = getCurrentRenderDevice( *this );
			auto props = device->getPhysicalDevice().getFormatProperties( m_info->format );

			if ( checkFlag( props.optimalTilingFeatures, VK_FORMAT_FEATURE_TRANSFER_DST_BIT ) )
			{
				m_info->usage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
			}

			if ( m_info->mipLevels > 1u )
			{
				CU_Require( checkFlag( props.optimalTilingFeatures, VK_FORMAT_FEATURE_TRANSFER_SRC_BIT ) );
				m_info->usage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
			}
			else if ( m_info->mipLevels == 0 )
			{
				m_info->mipLevels = 1u;
			}

			m_texture = makeImage( device
				, m_info
				, m_properties
				, m_debugName );
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
			m_texture.reset();
		}

		m_initialised = false;
	}

	void TextureLayout::generateMipmaps()const
	{
		if ( m_info->mipLevels > 1u
			&& getDefaultView().needsMipmapsGeneration() )
		{
			CU_Require( m_texture );
			auto & device = getCurrentRenderDevice( *this );
			auto commandBuffer = device.transferCommandPool->createCommandBuffer();
			commandBuffer->begin();
			m_texture->generateMipmaps( *commandBuffer );
			commandBuffer->end();
			device.transferQueue->submit( *commandBuffer, nullptr );
			device.transferQueue->waitIdle();
		}
	}

	void TextureLayout::setSource( Path const & folder
		, Path const & relative )
	{
		getDefaultView().initialiseSource( folder, relative );
	}

	void TextureLayout::setSource( PxBufferBaseSPtr buffer )
	{
		if ( !getDefaultView().hasSource() )
		{
			getDefaultView().initialiseSource( buffer );
		}
		else
		{
			getDefaultView().setBuffer( buffer );
		}
	}

	castor::String TextureLayout::getName()const
	{
		return m_debugName;
	}

	void TextureLayout::doUpdateFromFirstImage( castor::Size const & size
		, VkFormat format )
	{
		using ashes::operator==;

		if ( m_info->extent == VkExtent3D{}
			|| m_info->extent.width != size.getWidth()
			|| m_info->extent.height != size.getHeight()
			|| m_info->format != format )
		{
			VkImage texture = VK_NULL_HANDLE;

			if ( m_texture )
			{
				texture = *m_texture;
			}

			m_info->extent.width = size.getWidth();
			m_info->extent.height = size.getHeight();
			m_info->extent.depth = 1u;
			m_info->format = format;
			auto mipLevels = m_info->mipLevels;

			if ( m_info->mipLevels > 1u )
			{
				m_info->mipLevels = getMinMipLevels( m_info->mipLevels
					, m_info->extent );
			}

			eraseViews( m_info->mipLevels, m_defaultView );
			update( m_defaultView
				, texture
				, 0u
				, m_info->arrayLayers
				, 0u
				, m_info->mipLevels );

			if ( mipLevels > 1u )
			{
				m_info->usage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
				m_info->usage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
				m_info->mipLevels = std::min( m_info->mipLevels
					, ( getDefaultView().getLevelCount() > 1u
						? getDefaultView().getLevelCount()
						: m_info->mipLevels ) );

				if ( m_info->mipLevels != mipLevels )
				{
					eraseViews( m_info->mipLevels, m_arrayView );
					eraseViews( m_info->mipLevels, m_cubeView );
					eraseViews( m_info->mipLevels, m_sliceView );
				}
			}

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

	//************************************************************************************************
}
