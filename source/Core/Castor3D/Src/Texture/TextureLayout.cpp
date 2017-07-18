#include "TextureLayout.hpp"

#include "Engine.hpp"

using namespace Castor;

namespace Castor3D
{
	namespace
	{
		size_t GetImagesCount( TextureType p_type, uint32_t p_depth )
		{
			size_t result = p_depth;

			if ( p_type == TextureType::eCube || p_type == TextureType::eCubeArray )
			{
				result *= size_t( CubeMapFace::eCount );
			}

			return result;
		}

		TextureStorageType GetStorageType( TextureType p_type )
		{
			TextureStorageType result = TextureStorageType::eCount;

			switch ( p_type )
			{
			case TextureType::eBuffer:
				result = TextureStorageType::eBuffer;
				break;

			case TextureType::eOneDimension:
				result = TextureStorageType::eOneDimension;
				break;

			case TextureType::eOneDimensionArray:
				result = TextureStorageType::eOneDimensionArray;
				break;

			case TextureType::eTwoDimensions:
				result = TextureStorageType::eTwoDimensions;
				break;

			case TextureType::eTwoDimensionsArray:
				result = TextureStorageType::eTwoDimensionsArray;
				break;

			case TextureType::eTwoDimensionsMS:
				result = TextureStorageType::eTwoDimensionsMS;
				break;

			case TextureType::eThreeDimensions:
				result = TextureStorageType::eThreeDimensions;
				break;

			case TextureType::eCube:
				result = TextureStorageType::eCubeMap;
				break;

			case TextureType::eCubeArray:
				result = TextureStorageType::eCubeMapArray;
				break;

			default:
				FAILURE( "The given texture type doesn't have any associated storage type" );
				CASTOR_EXCEPTION( cuT( "The given texture type doesn't have any associated storage type" ) );
				break;
			}

			return result;
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
		uint32_t index = 0u;

		for ( auto & image : m_images )
		{
			image = std::make_unique< TextureImage >( *this, index++ );
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
		uint32_t index = 0u;

		for ( auto & image : m_images )
		{
			image = std::make_unique< TextureImage >( *this, index++ );
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
		uint32_t index = 0u;

		for ( auto & image : m_images )
		{
			image = std::make_unique< TextureImage >( *this, index++ );
		}
	}

	TextureLayout::~TextureLayout()
	{
	}

	bool TextureLayout::Initialise()
	{
		if ( !m_initialised )
		{
			bool result = DoInitialise();

			if ( result )
			{
				DoBind( 0 );
				result = DoCreateStorage( GetStorageType( m_type ) );
				DoUnbind( 0 );
			}

			m_initialised = result;
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

		for ( auto & image : m_images )
		{
			image->Resize( p_size );
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

		for ( auto & image : m_images )
		{
			image->Resize( p_size );
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
		auto buffer = m_images[0]->GetBuffer();

		if ( m_size != buffer->dimensions()
			 || m_format != buffer->format() )
		{
			m_size = buffer->dimensions();
			m_format = buffer->format();
		}
	}

	void TextureLayout::SetSource( PxBufferBaseSPtr p_buffer )
	{
		auto & image = m_images[0];

		if ( !image->HasSource() )
		{
			image->InitialiseSource( p_buffer );
		}
		else
		{
			image->SetBuffer( p_buffer );
		}

		auto buffer = image->GetBuffer();

		if ( m_size != buffer->dimensions()
			 || m_format != buffer->format() )
		{
			m_size = buffer->dimensions();
			m_format = buffer->format();
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
		bool result = true;

		if ( m_storage )
		{
			auto type = m_storage->GetType();
			m_storage.reset();
			result = DoCreateStorage( type );
		}

		return result;
	}

	bool TextureLayout::DoCreateStorage( TextureStorageType p_type )
	{
		bool result = false;

		if ( !m_storage )
		{
			try
			{
				m_storage = GetRenderSystem()->CreateTextureStorage( p_type, *this, m_cpuAccess, m_gpuAccess );
				result = true;
			}
			catch ( std::exception & p_exc )
			{
				Logger::LogError( StringStream() << cuT( "TextureImage::Initialise - Error encountered while allocating storage: " ) << string::string_cast< xchar >( p_exc.what() ) );
			}
		}

		ENSURE( m_storage );
		return result;
	}
}
