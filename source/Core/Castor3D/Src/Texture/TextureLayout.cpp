#include "TextureLayout.hpp"

#include "Engine.hpp"

#include "TextureStorage.hpp"

#include "Render/RenderSystem.hpp"

using namespace Castor;

namespace Castor3D
{
	namespace
	{
		size_t GetImagesCount( TextureType p_type, uint32_t p_depth )
		{
			size_t l_return = p_depth;

			if ( p_type == TextureType::eCube || p_type == TextureType::eCubeArray )
			{
				l_return *= size_t( CubeMapFace::eCount );
			}

			return l_return;
		}

		TextureStorageType GetStorageType( TextureType p_type )
		{
			TextureStorageType l_return = TextureStorageType::eCount;

			switch ( p_type )
			{
			case TextureType::eBuffer:
				l_return = TextureStorageType::eBuffer;
				break;

			case TextureType::eOneDimension:
				l_return = TextureStorageType::eOneDimension;
				break;

			case TextureType::eOneDimensionArray:
				l_return = TextureStorageType::eOneDimensionArray;
				break;

			case TextureType::eTwoDimensions:
				l_return = TextureStorageType::eTwoDimensions;
				break;

			case TextureType::eTwoDimensionsArray:
				l_return = TextureStorageType::eTwoDimensionsArray;
				break;

			case TextureType::eTwoDimensionsMS:
				l_return = TextureStorageType::eTwoDimensionsMS;
				break;

			case TextureType::eThreeDimensions:
				l_return = TextureStorageType::eThreeDimensions;
				break;

			case TextureType::eCube:
				l_return = TextureStorageType::eCubeMap;
				break;

			case TextureType::eCubeArray:
				l_return = TextureStorageType::eCubeMapArray;
				break;

			default:
				FAILURE( "The given texture type doesn't have any associated storage type" );
				CASTOR_EXCEPTION( cuT( "The given texture type doesn't have any associated storage type" ) );
				break;
			}

			return l_return;
		}
	}

	TextureLayout::TextureLayout(
		RenderSystem & p_renderSystem,
		TextureType p_type,
		AccessTypes const & p_cpuAccess,
		AccessTypes const & p_gpuAccess )
		: OwnedBy< RenderSystem >{ p_renderSystem }
		, m_type{ p_type }
		, m_images{ GetImagesCount( p_type, 1 ) }
		, m_cpuAccess{ p_cpuAccess }
		, m_gpuAccess{ p_gpuAccess }
		, m_depth{ 1 }
	{
		uint32_t l_index = 0u;

		for ( auto & l_image : m_images )
		{
			l_image = std::make_unique< TextureImage >( *this, l_index++ );
		}
	}

	TextureLayout::TextureLayout(
		RenderSystem & p_renderSystem,
		TextureType p_type,
		AccessTypes const & p_cpuAccess,
		AccessTypes const & p_gpuAccess,
		PixelFormat p_format,
		Size const & p_size )
		: OwnedBy< RenderSystem >{ p_renderSystem }
		, m_type{ p_type }
		, m_images{ GetImagesCount( p_type, 1 ) }
		, m_cpuAccess{ p_cpuAccess }
		, m_gpuAccess{ p_gpuAccess }
		, m_format{ p_format }
		, m_size{ p_size }
		, m_depth{ 1 }
	{
		REQUIRE( m_type != TextureType::eThreeDimensions
				 && m_type != TextureType::eOneDimensionArray
				 && m_type != TextureType::eTwoDimensionsArray
				 && m_type != TextureType::eTwoDimensionsMSArray
				 && m_type != TextureType::eCubeArray );
		uint32_t l_index = 0u;

		for ( auto & l_image : m_images )
		{
			l_image = std::make_unique< TextureImage >( *this, l_index++ );
		}
	}

	TextureLayout::TextureLayout(
		RenderSystem & p_renderSystem,
		TextureType p_type,
		AccessTypes const & p_cpuAccess,
		AccessTypes const & p_gpuAccess,
		PixelFormat p_format,
		Point3ui const & p_size )
		: OwnedBy< RenderSystem >{ p_renderSystem }
		, m_type{ p_type }
		, m_images{ GetImagesCount( p_type, p_size[2] ) }
		, m_cpuAccess{ p_cpuAccess }
		, m_gpuAccess{ p_gpuAccess }
		, m_format{ p_format }
		, m_size{ p_size[0], p_size[1] }
		, m_depth{ p_size[2] }
	{
		REQUIRE( m_type == TextureType::eThreeDimensions
				 || m_type == TextureType::eOneDimensionArray
				 || m_type == TextureType::eTwoDimensionsArray
				 || m_type == TextureType::eTwoDimensionsMSArray
				 || m_type == TextureType::eCubeArray );
		uint32_t l_index = 0u;

		for ( auto & l_image : m_images )
		{
			l_image = std::make_unique< TextureImage >( *this, l_index++ );
		}
	}

	TextureLayout::~TextureLayout()
	{
	}

	bool TextureLayout::Initialise()
	{
		if ( !m_initialised )
		{
			bool l_return = DoInitialise();

			if ( l_return )
			{
				DoBind( 0 );
				l_return = DoCreateStorage( GetStorageType( m_type ) );
				DoUnbind( 0 );
			}

			m_initialised = l_return;
		}

		return m_initialised;
	}

	void TextureLayout::Cleanup()
	{
		if ( m_initialised )
		{
			m_storage.reset();
			DoCleanup();
		}

		m_initialised = false;
	}

	void TextureLayout::Bind( uint32_t p_index )const
	{
		REQUIRE( m_initialised );
		DoBind( p_index );
		REQUIRE( m_storage );
		m_storage->Bind( p_index );
	}

	void TextureLayout::Unbind( uint32_t p_index )const
	{
		REQUIRE( m_initialised );
		REQUIRE( m_storage );
		m_storage->Unbind( p_index );
		DoUnbind( p_index );
	}

	void TextureLayout::Resize( Size const & p_size )
	{
		REQUIRE( m_type != TextureType::eThreeDimensions
				 && m_type != TextureType::eOneDimensionArray
				 && m_type != TextureType::eTwoDimensionsArray
				 && m_type != TextureType::eTwoDimensionsMSArray
				 && m_type != TextureType::eCubeArray );

		DoResetStorage();

		for ( auto & l_image : m_images )
		{
			l_image->Resize( p_size );
		}
	}

	void TextureLayout::Resize( Point3ui const & p_size )
	{
		REQUIRE( m_type == TextureType::eThreeDimensions
				 || m_type == TextureType::eOneDimensionArray
				 || m_type == TextureType::eTwoDimensionsArray
				 || m_type == TextureType::eTwoDimensionsMSArray
				 || m_type == TextureType::eCubeArray );

		DoResetStorage();

		for ( auto & l_image : m_images )
		{
			l_image->Resize( p_size );
		}
	}

	uint8_t * TextureLayout::Lock( AccessTypes const & p_lock )
	{
		DoBind( 0u );
		REQUIRE( m_storage );
		return m_storage->Lock( p_lock );
	}

	void TextureLayout::Unlock( bool p_modified )
	{
		REQUIRE( m_storage );
		m_storage->Unlock( p_modified );
		DoUnbind( 0u );
	}

	uint8_t * TextureLayout::Lock( AccessTypes const & p_lock, uint32_t p_index )
	{
		REQUIRE( m_storage );
		return m_storage->Lock( p_lock, p_index );
	}

	void TextureLayout::Unlock( bool p_modified, uint32_t p_index )
	{
		REQUIRE( m_storage );
		m_storage->Unlock( p_modified, p_index );
	}

	void TextureLayout::SetSource( Path const & p_folder, Path const & p_relative )
	{
		m_images[0]->InitialiseSource( p_folder, p_relative );
		auto l_buffer = m_images[0]->GetBuffer();

		if ( m_size != l_buffer->dimensions()
			 || m_format != l_buffer->format() )
		{
			m_size = l_buffer->dimensions();
			m_format = l_buffer->format();
		}
	}

	void TextureLayout::SetSource( PxBufferBaseSPtr p_buffer )
	{
		auto & l_image = m_images[0];

		if ( !l_image->HasSource() )
		{
			l_image->InitialiseSource( p_buffer );
		}
		else
		{
			l_image->SetBuffer( p_buffer );
		}

		auto l_buffer = l_image->GetBuffer();

		if ( m_size != l_buffer->dimensions()
			 || m_format != l_buffer->format() )
		{
			m_size = l_buffer->dimensions();
			m_format = l_buffer->format();
		}
	}

	void TextureLayout::DoUpdateFromFirstImage( Castor::Size const & p_size, Castor::PixelFormat p_format )
	{
		if ( m_size == Size{} )
		{
			m_size = p_size;
			m_format = p_format;
		}
	}

	bool TextureLayout::DoResetStorage()
	{
		bool l_return = true;

		if ( m_storage )
		{
			auto l_type = m_storage->GetType();
			m_storage.reset();
			l_return = DoCreateStorage( l_type );
		}

		return l_return;
	}

	bool TextureLayout::DoCreateStorage( TextureStorageType p_type )
	{
		bool l_return = false;

		if ( !m_storage )
		{
			try
			{
				m_storage = GetRenderSystem()->CreateTextureStorage( p_type, *this, m_cpuAccess, m_gpuAccess );
				l_return = true;
			}
			catch ( std::exception & p_exc )
			{
				Logger::LogError( StringStream() << cuT( "TextureImage::Initialise - Error encountered while allocating storage: " ) << string::string_cast< xchar >( p_exc.what() ) );
			}
		}

		ENSURE( m_storage );
		return l_return;
	}
}
