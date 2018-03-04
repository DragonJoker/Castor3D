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
		size_t getSubviewCount( renderer::TextureType type
			, uint32_t layerCount )
		{
			size_t result = layerCount;

			if ( type == renderer::TextureType::eCube
				|| type == renderer::TextureType::eCubeArray )
			{
				result *= size_t( CubeMapFace::eCount );
			}
			else if ( type == renderer::TextureType::e3D )
			{
				result = 1u;
			}

			return result;
		}

		renderer::TextureType getSubviewType( renderer::TextureType type )
		{
			renderer::TextureType result = type;

			switch ( result )
			{
			case renderer::TextureType::e1D:
				return renderer::TextureType::e1D;

			case renderer::TextureType::e2D:
				return renderer::TextureType::e2D;

			case renderer::TextureType::e3D:
				return renderer::TextureType::e3D;

			case renderer::TextureType::eCube:
				return renderer::TextureType::e2D;

			case renderer::TextureType::e1DArray:
				return renderer::TextureType::e1D;

			case renderer::TextureType::e2DArray:
				return renderer::TextureType::e2D;

			case renderer::TextureType::eCubeArray:
				return renderer::TextureType::e2D;

			default:
				FAILURE( "Unsupported texture type." );
				return renderer::TextureType::e2D;
			}
		}

		std::vector< TextureViewUPtr > createSubviews( TextureLayout & layout
			, renderer::TextureType type
			, uint32_t layerCount )
		{
			std::vector< TextureViewUPtr > result{ getSubviewCount( type, 1u ) + 1u };
			result[0] = std::make_unique< TextureView >( layout
				, type
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
		, renderer::TextureType type
		, renderer::ImageUsageFlags usage
		, renderer::MemoryPropertyFlags memoryProperties )
		: OwnedBy< RenderSystem >{ renderSystem }
		, m_type{ type }
		, m_usage{ usage }
		, m_properties{ memoryProperties }
		, m_views{ createSubviews( *this, type, 1u ) }
		, m_defaultView{ *m_views[0] }
	{
		auto viewType = getSubviewType( type );

		if ( viewType != type )
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

	TextureLayout::TextureLayout( RenderSystem & renderSystem
		, renderer::TextureType type
		, renderer::ImageUsageFlags usage
		, renderer::MemoryPropertyFlags memoryProperties
		, uint32_t mipmapCount )
		: OwnedBy< RenderSystem >{ renderSystem }
		, m_type{ type }
		, m_usage{ usage }
		, m_properties{ memoryProperties }
		, m_mipmapCount{ mipmapCount }
		, m_views{ createSubviews( *this, type, 1u ) }
		, m_defaultView{ *m_views[0] }
	{
		auto viewType = getSubviewType( type );

		if ( viewType != type )
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

	TextureLayout::TextureLayout( RenderSystem & renderSystem
		, renderer::TextureType type
		, renderer::ImageUsageFlags usage
		, renderer::MemoryPropertyFlags memoryProperties
		, PixelFormat format
		, Size const & size )
		: OwnedBy< RenderSystem >{ renderSystem }
		, m_type{ type }
		, m_usage{ usage }
		, m_properties{ memoryProperties }
		, m_size{ size }
		, m_format{ format }
		, m_views{ createSubviews( *this, type, 1u ) }
		, m_defaultView{ *m_views[0] }
	{
		REQUIRE( m_type != renderer::TextureType::e3D
			&& m_type != renderer::TextureType::e1DArray
			&& m_type != renderer::TextureType::e2DArray
			&& m_type != renderer::TextureType::eCubeArray );
		auto viewType = getSubviewType( type );

		if ( viewType != type )
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

	TextureLayout::TextureLayout( RenderSystem & renderSystem
		, renderer::TextureType type
		, renderer::ImageUsageFlags usage
		, renderer::MemoryPropertyFlags memoryProperties
		, PixelFormat format
		, Point3ui const & size )
		: OwnedBy< RenderSystem >{ renderSystem }
		, m_type{ type }
		, m_usage{ usage }
		, m_properties{ memoryProperties }
		, m_size{ size[0], size[1] }
		, m_format{ format }
		, m_depth{ size[2] }
		, m_views{ createSubviews( *this, type, 1u ) }
		, m_defaultView{ *m_views[0] }
	{
		REQUIRE( m_type == renderer::TextureType::e3D
			|| m_type == renderer::TextureType::e1DArray
			|| m_type == renderer::TextureType::e2DArray
			|| m_type == renderer::TextureType::eCubeArray );
		auto viewType = getSubviewType( type );

		if ( viewType != type )
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

	TextureLayout::TextureLayout( RenderSystem & renderSystem
		, renderer::TextureType type
		, renderer::ImageUsageFlags usage
		, renderer::MemoryPropertyFlags memoryProperties
		, castor::PixelFormat format
		, castor::Size const & size
		, uint32_t mipmapCount )
		: OwnedBy< RenderSystem >{ renderSystem }
		, m_type{ type }
		, m_usage{ usage }
		, m_properties{ memoryProperties }
		, m_size{ size[0], size[1] }
		, m_format{ format }
		, m_mipmapCount{ mipmapCount }
		, m_views{ createSubviews( *this, type, 1u ) }
		, m_defaultView{ *m_views[0] }
	{
		REQUIRE( m_type == renderer::TextureType::e3D
			|| m_type == renderer::TextureType::e1DArray
			|| m_type == renderer::TextureType::e2DArray
			|| m_type == renderer::TextureType::eCubeArray );
		auto viewType = getSubviewType( type );

		if ( viewType != type )
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
			m_texture = getRenderSystem()->getCurrentDevice()->createTexture();

			switch ( m_type )
			{
			case renderer::TextureType::e1D:
				m_texture->setImage( m_format
					, m_size.getWidth()
					, m_usage );
				break;

			case renderer::TextureType::e2D:
				m_texture->setImage( m_format
					, Point2ui{ m_size.getWidth(), m_size.getHeight() }
					, m_usage );
				break;

			case renderer::TextureType::e3D:
				m_texture->setImage( m_format
					, Point3ui{ m_size.getWidth(), m_size.getHeight(), m_depth }
					, m_usage );
				break;

			case renderer::TextureType::eCube:
				m_texture->setImage( m_format
					, Point2ui{ m_size.getWidth(), m_size.getHeight() }
					, 6u
					, m_usage );
				break;

			case renderer::TextureType::e1DArray:
				m_texture->setImage( m_format
					, m_size.getWidth()
					, m_depth
					, m_usage );
				break;

			case renderer::TextureType::e2DArray:
				m_texture->setImage( m_format
					, Point2ui{ m_size.getWidth(), m_size.getHeight() }
					, m_depth
					, m_usage );
				break;

			case renderer::TextureType::eCubeArray:
				m_texture->setImage( m_format
					, Point2ui{ m_size.getWidth(), m_size.getHeight() }
					, m_depth
					, m_usage );
				break;
			}

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

		if ( m_size != buffer->dimensions()
			 || m_format != buffer->format() )
		{
			m_size = buffer->dimensions();
			m_format = buffer->format();
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

		if ( m_size != buffer->dimensions()
			 || m_format != buffer->format() )
		{
			m_size = buffer->dimensions();
			m_format = buffer->format();
		}
	}

	void TextureLayout::doUpdateFromFirstImage( castor::Size const & size
		, castor::PixelFormat format )
	{
		if ( m_size == Size{}
			|| m_size != size
			|| m_format != format )
		{
			m_size = size;
			m_format = format;
		}
	}

	//************************************************************************************************
}
