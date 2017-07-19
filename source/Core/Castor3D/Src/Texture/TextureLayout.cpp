#include "TextureLayout.hpp"

#include "Engine.hpp"

using namespace Castor;

namespace Castor3D
{
	namespace
	{
		size_t GetImagesCount( TextureType type, uint32_t depth )
		{
			size_t result = depth;

			if ( type == TextureType::eCube || type == TextureType::eCubeArray )
			{
				result *= size_t( CubeMapFace::eCount );
			}

			return result;
		}

		TextureStorageType GetStorageType( TextureType type )
		{
			TextureStorageType result = TextureStorageType::eCount;

			switch ( type )
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

	TextureLayout::TextureLayout( RenderSystem & renderSystem
		, TextureType type
		, AccessTypes const & cpuAccess
		, AccessTypes const & gpuAccess )
		: OwnedBy< RenderSystem >{ renderSystem }
		, m_type{ type }
		, m_images{ GetImagesCount( type, 1 ) }
		, m_cpuAccess{ cpuAccess }
		, m_gpuAccess{ gpuAccess }
		, m_depth{ 1 }
	{
		uint32_t index = 0u;

		for ( auto & image : m_images )
		{
			image = std::make_unique< TextureImage >( *this, index++ );
		}
	}

	TextureLayout::TextureLayout( RenderSystem & renderSystem
		, TextureType type
		, AccessTypes const & cpuAccess
		, AccessTypes const & gpuAccess
		, PixelFormat format
		, Size const & size )
		: OwnedBy< RenderSystem >{ renderSystem }
		, m_type{ type }
		, m_images{ GetImagesCount( type, 1 ) }
		, m_cpuAccess{ cpuAccess }
		, m_gpuAccess{ gpuAccess }
		, m_format{ format }
		, m_size{ size }
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

	TextureLayout::TextureLayout( RenderSystem & renderSystem
		, TextureType type
		, AccessTypes const & cpuAccess
		, AccessTypes const & gpuAccess
		, PixelFormat format
		, Point3ui const & size )
		: OwnedBy< RenderSystem >{ renderSystem }
		, m_type{ type }
		, m_images{ GetImagesCount( type, size[2] ) }
		, m_cpuAccess{ cpuAccess }
		, m_gpuAccess{ gpuAccess }
		, m_format{ format }
		, m_size{ size[0], size[1] }
		, m_depth{ size[2] }
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

	void TextureLayout::Bind( uint32_t index )const
	{
		REQUIRE( m_initialised );
		DoBind( index );
		REQUIRE( m_storage );
		m_storage->Bind( index );
	}

	void TextureLayout::Unbind( uint32_t index )const
	{
		REQUIRE( m_initialised );
		REQUIRE( m_storage );
		m_storage->Unbind( index );
		DoUnbind( index );
	}

	void TextureLayout::Resize( Size const & size )
	{
		REQUIRE( m_type != TextureType::eThreeDimensions
				 && m_type != TextureType::eOneDimensionArray
				 && m_type != TextureType::eTwoDimensionsArray
				 && m_type != TextureType::eTwoDimensionsMSArray
				 && m_type != TextureType::eCubeArray );

		DoResetStorage();

		for ( auto & image : m_images )
		{
			image->Resize( size );
		}
	}

	void TextureLayout::Resize( Point3ui const & size )
	{
		REQUIRE( m_type == TextureType::eThreeDimensions
				 || m_type == TextureType::eOneDimensionArray
				 || m_type == TextureType::eTwoDimensionsArray
				 || m_type == TextureType::eTwoDimensionsMSArray
				 || m_type == TextureType::eCubeArray );

		DoResetStorage();

		for ( auto & image : m_images )
		{
			image->Resize( size );
		}
	}

	uint8_t * TextureLayout::Lock( AccessTypes const & lock )
	{
		DoBind( 0u );
		REQUIRE( m_storage );
		return m_storage->Lock( lock );
	}

	void TextureLayout::Unlock( bool p_modified )
	{
		REQUIRE( m_storage );
		m_storage->Unlock( p_modified );
		DoUnbind( 0u );
	}

	uint8_t * TextureLayout::Lock( AccessTypes const & lock
		, uint32_t index )
	{
		REQUIRE( m_storage );
		return m_storage->Lock( lock, index );
	}

	void TextureLayout::Unlock( bool modified
		, uint32_t index )
	{
		REQUIRE( m_storage );
		m_storage->Unlock( modified, index );
	}

	void TextureLayout::SetSource( Path const & folder
		, Path const & relative )
	{
		m_images[0]->InitialiseSource( folder, relative );
		auto buffer = m_images[0]->GetBuffer();

		if ( m_size != buffer->dimensions()
			 || m_format != buffer->format() )
		{
			m_size = buffer->dimensions();
			m_format = buffer->format();
		}
	}

	void TextureLayout::SetSource( PxBufferBaseSPtr buffer )
	{
		auto & image = m_images[0];

		if ( !image->HasSource() )
		{
			image->InitialiseSource( buffer );
		}
		else
		{
			image->SetBuffer( buffer );
		}

		buffer = image->GetBuffer();

		if ( m_size != buffer->dimensions()
			 || m_format != buffer->format() )
		{
			m_size = buffer->dimensions();
			m_format = buffer->format();
		}
	}

	void TextureLayout::DoUpdateFromFirstImage( Castor::Size const & size
		, Castor::PixelFormat format )
	{
		if ( m_size == Size{}
			|| m_size != size
			|| m_format != format )
		{
			m_size = size;
			m_format = format;
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

	bool TextureLayout::DoCreateStorage( TextureStorageType type )
	{
		bool result = false;

		if ( !m_storage )
		{
			try
			{
				m_storage = GetRenderSystem()->CreateTextureStorage( type
					, *this
					, m_cpuAccess
					, m_gpuAccess );
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
