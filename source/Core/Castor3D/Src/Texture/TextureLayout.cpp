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
			, renderer::ImageCreateInfo const & info )
		{
			renderer::TextureType result = type;

			switch ( result )
			{
			case renderer::TextureType::e1D:
				if ( info.arrayLayers > 1 )
				{
					return renderer::TextureViewType::e1DArray;
				}
				return renderer::TextureViewType::e1D;

			case renderer::TextureType::e2D:
				if ( info.arrayLayers > 1 )
				{
					if ( checkFlag( info.flags, renderer::ImageCreateFlag::eCubeCompatible ) )
					{
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

		std::vector< TextureViewUPtr > createSubviews( TextureLayout & layout
			, renderer::ImageCreateInfo const & info )
		{
			std::vector< TextureViewUPtr > result{ info.arrayLayers + 1u };
			result[0] = std::make_unique< TextureView >( layout
				, info.imageType
				, 0u
				, layout.getMipmapCount()
				, 0u
				, ~( 0u )
				, 0u );
			return result;
		}
	}

	//************************************************************************************************

	TextureLayout::TextureLayout( RenderSystem & renderSystem
		, renderer::ImageCreateInfo info
		, renderer::MemoryPropertyFlags memoryProperties )
		: OwnedBy< RenderSystem >{ renderSystem }
		, m_info{ std::move( info ) }
		, m_properties{ memoryProperties }
		, m_views{ createSubviews( *this, m_info ) }
		, m_defaultView{ *m_views[0] }
	{
		auto viewType = getSubviewType( m_info.imageType, m_info );

		if ( uint32_t( viewType ) != uint32_t( m_info.imageType ) )
		{
			for ( uint32_t i = 1u; i < m_views.size(); ++i )
			{
				m_views[i] = std::make_unique< TextureView >( *this
					, viewType
					, 0u
					, ~( 0u )
					, i
					, 1u
					, i );
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
			m_texture = getRenderSystem()->getCurrentDevice()->createTexture( m_info );

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

			m_texture.reset();
		}

		m_initialised = false;
	}

	void TextureLayout::generateMipmaps()const
	{
		REQUIRE( m_texture );
		m_texture->generateMipmaps();
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
