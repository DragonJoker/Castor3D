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
			, ashes::ImageCreateInfo const & info
			, uint32_t baseArrayLayer
			, uint32_t arrayLayers )
		{
			return std::make_unique< TextureView >( layout
				, getSubviewCreateInfos( info
					, VK_NULL_HANDLE
					, baseArrayLayer
					, arrayLayers
					, 0u
					, std::max( info->mipLevels, 1u ) )
				, 0u );
		}

		TextureViewUPtr createMipSubview( TextureLayout & layout
			, ashes::ImageCreateInfo const & info
			, uint32_t baseMipLevel
			, uint32_t levelCount )
		{
			return std::make_unique< TextureView >( layout
				, getSubviewCreateInfos( info
					, VK_NULL_HANDLE
					, 0u
					, info->arrayLayers
					, baseMipLevel, levelCount )
				, 0u );
		}

		uint32_t getMinMipLevels( uint32_t mipLevels
			, VkExtent3D const & extent )
		{
			return std::min( getMipLevels( extent ), mipLevels );
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
		, m_defaultView{ createSubview( *this, m_info, 0u, m_info->arrayLayers ) }
		, m_debugName{ std::move( debugName ) }
	{
		m_info->mipLevels = std::max( 1u, m_info->mipLevels );
		uint32_t max = std::max( m_info->arrayLayers, m_info->extent.depth );

		if ( max > 1u )
		{
			m_views.resize( max );
			uint32_t index = 0u;

			for ( auto & view : m_views )
			{
				view = createSubview( *this, m_info, index, 1u );
				++index;
			}
		}
		else if ( info->mipLevels > 1u )
		{
			m_views.resize( info->mipLevels );
			uint32_t index = 0u;

			for ( auto & view : m_views )
			{
				view = createMipSubview( *this, m_info, index, 1u );
				++index;
			}
		}
		else
		{
			m_views.resize( 1u );
			m_views.back() = createSubview( *this, m_info, 0u, 1u );
		}
	}

	TextureLayout::~TextureLayout()
	{
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
			m_defaultView->initialise();

			for ( auto & view : m_views )
			{
				view->initialise();
			}

			m_initialised = m_texture != nullptr;
		}

		return m_initialised;
	}

	void TextureLayout::cleanup()
	{
		if ( m_initialised )
		{
			for ( auto & view : m_views )
			{
				view->cleanup();
			}

			m_defaultView->cleanup();
			m_texture.reset();
		}

		m_initialised = false;
	}

	void TextureLayout::generateMipmaps()const
	{
		if ( m_info->mipLevels > 1u
			&& m_defaultView->needsMipmapsGeneration() )
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
		m_defaultView->initialiseSource( folder, relative );
	}

	void TextureLayout::setSource( PxBufferBaseSPtr buffer )
	{
		if ( !m_defaultView->hasSource() )
		{
			m_defaultView->initialiseSource( buffer );
		}
		else
		{
			m_defaultView->setBuffer( buffer );
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

			m_defaultView->doUpdate( getSubviewCreateInfos( m_info
				, texture
				, 0u
				, m_info->arrayLayers
				, 0u
				, m_info->mipLevels ) );

			if ( m_views.size() == 1 )
			{
				m_views.back()->doUpdate( getSubviewCreateInfos( m_info
					, texture
					, 0u
					, 1u
					, 0u
					, 1u ) );
			}
			else if ( mipLevels > 1u )
			{
				m_info->usage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
				m_info->usage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
				m_info->mipLevels = std::min( m_info->mipLevels
					, ( m_defaultView->getLevelCount() > 1u
						? m_defaultView->getLevelCount()
						: m_info->mipLevels ) );

				if ( m_info->mipLevels != mipLevels )
				{
					auto it = m_views.begin();
					uint32_t index = 0u;

					while ( it != m_views.end() )
					{
						auto & view = *it;

						if ( view->getBaseMipLevel() >= m_info->mipLevels )
						{
							it = m_views.erase( it );
						}
						else
						{
							view->doUpdate( getSubviewCreateInfos( m_info
								, texture
								, 0u
								, 1u
								, index++
								, 1u ) );
							++it;
						}
					}
				}
				else
				{
					uint32_t index = 0u;

					for ( auto & view : m_views )
					{
						view->doUpdate( getSubviewCreateInfos( m_info
							, texture
							, 0u
							, 1u
							, index++
							, 1u ) );
					}
				}
			}
		}
	}

	//************************************************************************************************
}
