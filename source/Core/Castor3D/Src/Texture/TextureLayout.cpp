#include "TextureLayout.hpp"

#include "Engine.hpp"

#include <Miscellaneous/BitSize.hpp>

#include <Ashes/Image/Texture.hpp>
#include <Ashes/Image/TextureView.hpp>

using namespace castor;

namespace castor3d
{
	//************************************************************************************************

	namespace
	{
		ashes::TextureViewType getSubviewType( ashes::TextureType type
			, ashes::ImageCreateFlags flags
			, uint32_t arrayLayers )
		{
			ashes::TextureType result = type;

			switch ( result )
			{
			case ashes::TextureType::e1D:
				if ( arrayLayers > 1 )
				{
					return ashes::TextureViewType::e1DArray;
				}
				return ashes::TextureViewType::e1D;

			case ashes::TextureType::e2D:
				if ( arrayLayers > 1 )
				{
					if ( checkFlag( flags, ashes::ImageCreateFlag::eCubeCompatible ) )
					{
						CU_Require( ( arrayLayers % 6 ) == 0 );
						return arrayLayers == 6u
							? ashes::TextureViewType::eCube
							: ashes::TextureViewType::eCubeArray;
					}

					return ashes::TextureViewType::e2DArray;
				}

				return ashes::TextureViewType::e2D;

			case ashes::TextureType::e3D:
				return ashes::TextureViewType::e3D;

			default:
				CU_Failure( "Unsupported texture type." );
				return ashes::TextureViewType::e2D;
			}
		}

		ashes::ImageViewCreateInfo getSubviewCreateInfos( ashes::ImageCreateInfo const & info
			, uint32_t baseArrayLayer
			, uint32_t arrayLayers
			, uint32_t baseMipLevel
			, uint32_t levelCount )
		{
			ashes::ImageViewCreateInfo view{};
			view.format = info.format;
			view.viewType = getSubviewType( info.imageType, info.flags, arrayLayers );
			view.subresourceRange.aspectMask = ashes::getAspectMask( info.format );
			view.subresourceRange.baseArrayLayer = baseArrayLayer;
			view.subresourceRange.layerCount = arrayLayers;
			view.subresourceRange.baseMipLevel = baseMipLevel;
			view.subresourceRange.levelCount = levelCount;
			return view;
		}

		TextureViewUPtr createSubview( TextureLayout & layout
			, ashes::ImageCreateInfo const & info
			, uint32_t baseArrayLayer
			, uint32_t arrayLayers )
		{
			return std::make_unique< TextureView >( layout
				, getSubviewCreateInfos( info, baseArrayLayer, arrayLayers, 0u, info.mipLevels )
				, 0u );
		}

		TextureViewUPtr createMipSubview( TextureLayout & layout
			, ashes::ImageCreateInfo const & info
			, uint32_t baseMipLevel
			, uint32_t levelCount )
		{
			return std::make_unique< TextureView >( layout
				, getSubviewCreateInfos( info, 0u, info.arrayLayers, baseMipLevel, levelCount )
				, 0u );
		}

		uint32_t getMaxMipLevels( uint32_t mipLevels
			, ashes::Extent3D const & extent )
		{
			auto min = std::min( extent.width, extent.height );
			auto bitSize = uint32_t( castor::getBitSize( min ) );
			return std::min( bitSize, mipLevels );
		}
	}

	//************************************************************************************************

	TextureLayout::TextureLayout( RenderSystem & renderSystem
		, ashes::ImageCreateInfo info
		, ashes::MemoryPropertyFlags memoryProperties )
		: OwnedBy< RenderSystem >{ renderSystem }
		, m_info{ std::move( info ) }
		, m_properties{ memoryProperties }
		, m_defaultView{ createSubview( *this, m_info, 0u, m_info.arrayLayers ) }
	{
		uint32_t max = std::max( m_info.arrayLayers, m_info.extent.depth );

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
		else if ( info.mipLevels > 1u )
		{
			m_views.resize( info.mipLevels );
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
			m_info.usage |= ashes::ImageUsageFlag::eTransferDst;

			if ( m_info.mipLevels > 1u )
			{
				m_info.usage |= ashes::ImageUsageFlag::eTransferSrc;
			}
			else if ( m_info.mipLevels == 0 )
			{
				m_info.mipLevels = 1u;
			}

			m_texture = getCurrentDevice( *this ).createTexture( m_info, m_properties );
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
		if ( m_info.mipLevels > 1u )
		{
			CU_Require( m_texture );
			m_texture->generateMipmaps();
		}
	}

	void TextureLayout::setSource( Path const & folder
		, Path const & relative
		, ImageComponents components )
	{
		m_defaultView->initialiseSource( folder, relative, components );
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

	void TextureLayout::doUpdateFromFirstImage( castor::Size const & size
		, ashes::Format format )
	{
		if ( m_info.extent == ashes::Extent3D{}
			|| m_info.extent.width != size.getWidth()
			|| m_info.extent.height != size.getHeight()
			|| m_info.format != format )
		{
			m_info.extent.width = size.getWidth();
			m_info.extent.height = size.getHeight();
			m_info.extent.depth = 1u;
			m_info.format = format;
			auto mipLevels = m_info.mipLevels;

			if ( m_info.mipLevels > 1u )
			{
				m_info.mipLevels = getMaxMipLevels( m_info.mipLevels
					, m_info.extent );
			}

			m_defaultView->doUpdate( getSubviewCreateInfos( m_info, 0u, m_info.arrayLayers, 0u, m_info.mipLevels ) );

			if ( m_views.size() == 1 )
			{
				m_views.back()->doUpdate( getSubviewCreateInfos( m_info, 0u, 1u, 0u, 1u ) );
			}
			else if ( mipLevels > 1u )
			{
				m_info.usage |= ashes::ImageUsageFlag::eTransferSrc;

				if ( m_info.mipLevels != mipLevels )
				{
					auto it = m_views.begin();
					uint32_t index = 0u;

					while ( it != m_views.end() )
					{
						auto & view = *it;

						if ( view->getBaseMipLevel() >= m_info.mipLevels )
						{
							it = m_views.erase( it );
						}
						else
						{
							view->doUpdate( getSubviewCreateInfos( m_info, 0u, 1u, index++, 1u ) );
							++it;
						}
					}
				}
				else
				{
					uint32_t index = 0u;

					for ( auto & view : m_views )
					{
						view->doUpdate( getSubviewCreateInfos( m_info, 0u, 1u, index++, 1u ) );
					}
				}
			}
		}
	}

	//************************************************************************************************
}
