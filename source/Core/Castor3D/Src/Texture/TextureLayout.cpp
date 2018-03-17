#include "TextureLayout.hpp"

#include "Engine.hpp"

#include <Image/Texture.hpp>
#include <Image/TextureView.hpp>

using namespace castor;

namespace castor3d
{
	//************************************************************************************************

	namespace
	{
		renderer::TextureViewType getSubviewType( renderer::TextureType type
			, renderer::ImageCreateFlags flags
			, uint32_t arrayLayers )
		{
			renderer::TextureType result = type;

			switch ( result )
			{
			case renderer::TextureType::e1D:
				if ( arrayLayers > 1 )
				{
					return renderer::TextureViewType::e1DArray;
				}
				return renderer::TextureViewType::e1D;

			case renderer::TextureType::e2D:
				if ( arrayLayers > 1 )
				{
					if ( checkFlag( flags, renderer::ImageCreateFlag::eCubeCompatible ) )
					{
						REQUIRE( ( arrayLayers % 6 ) == 0 );
						return renderer::TextureViewType::eCube;
					}

					return renderer::TextureViewType::e2DArray;
				}

				return renderer::TextureViewType::e2D;

			case renderer::TextureType::e3D:
				return renderer::TextureViewType::e3D;

			default:
				FAILURE( "Unsupported texture type." );
				return renderer::TextureViewType::e2D;
			}
		}

		TextureViewUPtr createSubview( TextureLayout & layout
			, renderer::ImageCreateInfo const & info
			, uint32_t baseArrayLayer
			, uint32_t arrayLayers )
		{
			renderer::ImageViewCreateInfo view{};
			view.format = info.format;
			view.viewType = getSubviewType( info.imageType, info.flags, arrayLayers );
			view.subresourceRange.aspectMask = renderer::getAspectMask( info.format );
			view.subresourceRange.baseArrayLayer = baseArrayLayer;
			view.subresourceRange.layerCount = arrayLayers;
			view.subresourceRange.baseMipLevel = 0u;
			view.subresourceRange.levelCount = info.mipLevels;
			return std::make_unique< TextureView >( layout
				, view
				, 0u );
		}
	}

	//************************************************************************************************

	TextureLayout::TextureLayout( RenderSystem & renderSystem
		, renderer::ImageCreateInfo info
		, renderer::MemoryPropertyFlags memoryProperties )
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
	}

	TextureLayout::~TextureLayout()
	{
	}

	bool TextureLayout::initialise()
	{
		if ( !m_initialised )
		{
			m_texture = getRenderSystem()->getCurrentDevice()->createTexture( m_info, m_properties );
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
			REQUIRE( m_texture );
			m_texture->generateMipmaps();
		}
	}

	void TextureLayout::setSource( Path const & folder
		, Path const & relative )
	{
		m_views[0]->initialiseSource( folder, relative );
		auto buffer = m_views[0]->getBuffer();

		if ( m_info.extent.width != buffer->dimensions().getWidth()
			|| m_info.extent.height != buffer->dimensions().getHeight()
			|| m_info.format != convert( buffer->format() ) )
		{
			m_info.extent.width = buffer->dimensions().getWidth();
			m_info.extent.height = buffer->dimensions().getHeight();
			m_info.extent.depth = 1u;
			m_info.format = convert( buffer->format() );
		}
	}

	void TextureLayout::setSource( PxBufferBaseSPtr buffer )
	{
		auto & image = m_views[0];

		if ( !image->hasSource() )
		{
			image->initialiseSource( buffer );
		}
		else
		{
			image->setBuffer( buffer );
		}

		buffer = image->getBuffer();

		if ( m_info.extent.width != buffer->dimensions().getWidth()
			|| m_info.extent.height != buffer->dimensions().getHeight()
			|| m_info.format != convert( buffer->format() ) )
		{
			m_info.extent.width = buffer->dimensions().getWidth();
			m_info.extent.height = buffer->dimensions().getHeight();
			m_info.extent.depth = 1u;
			m_info.format = convert( buffer->format() );
		}
	}

	void TextureLayout::doUpdateFromFirstImage( castor::Size const & size
		, renderer::Format format )
	{
		if ( m_info.extent == renderer::Extent3D{}
			|| m_info.extent.width != size.getWidth()
			|| m_info.extent.height != size.getHeight()
			|| m_info.format != format )
		{
			m_info.extent.width = size.getWidth();
			m_info.extent.height = size.getHeight();
			m_info.extent.depth = 1u;
			m_info.format = format;
		}
	}

	//************************************************************************************************
}
