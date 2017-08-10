#include "TextureLayout.hpp"

#include "Engine.hpp"

using namespace castor;

namespace castor3d
{
	namespace
	{
		size_t getImagesCount( TextureType type, uint32_t depth )
		{
			size_t result = depth;

			if ( type == TextureType::eCube || type == TextureType::eCubeArray )
			{
				result *= size_t( CubeMapFace::eCount );
			}

			return result;
		}

		TextureStorageType getStorageType( TextureType type )
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
		, m_images{ getImagesCount( type, 1 ) }
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
		, m_images{ getImagesCount( type, 1 ) }
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
		, m_images{ getImagesCount( type, size[2] ) }
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

	bool TextureLayout::initialise()
	{
		if ( !m_initialised )
		{
			bool result = doInitialise();

			if ( result )
			{
				doBind( 0 );
				result = doCreateStorage( getStorageType( m_type ) );
				doUnbind( 0 );
			}

			m_initialised = result;
		}

		return m_initialised;
	}

	void TextureLayout::cleanup()
	{
		if ( m_initialised )
		{
			m_storage.reset();
			doCleanup();
		}

		m_initialised = false;
	}

	void TextureLayout::bind( uint32_t index )const
	{
		REQUIRE( m_initialised );
		doBind( index );
		REQUIRE( m_storage );
		m_storage->bind( index );
	}

	void TextureLayout::unbind( uint32_t index )const
	{
		REQUIRE( m_initialised );
		REQUIRE( m_storage );
		m_storage->unbind( index );
		doUnbind( index );
	}

	void TextureLayout::resize( Size const & size )
	{
		REQUIRE( m_type != TextureType::eThreeDimensions
				 && m_type != TextureType::eOneDimensionArray
				 && m_type != TextureType::eTwoDimensionsArray
				 && m_type != TextureType::eTwoDimensionsMSArray
				 && m_type != TextureType::eCubeArray );

		doResetStorage();

		for ( auto & image : m_images )
		{
			image->resize( size );
		}
	}

	void TextureLayout::resize( Point3ui const & size )
	{
		REQUIRE( m_type == TextureType::eThreeDimensions
				 || m_type == TextureType::eOneDimensionArray
				 || m_type == TextureType::eTwoDimensionsArray
				 || m_type == TextureType::eTwoDimensionsMSArray
				 || m_type == TextureType::eCubeArray );

		doResetStorage();

		for ( auto & image : m_images )
		{
			image->resize( size );
		}
	}

	uint8_t * TextureLayout::lock( AccessTypes const & lock )
	{
		doBind( 0u );
		REQUIRE( m_storage );
		return m_storage->lock( lock );
	}

	void TextureLayout::unlock( bool p_modified )
	{
		REQUIRE( m_storage );
		m_storage->unlock( p_modified );
		doUnbind( 0u );
	}

	uint8_t * TextureLayout::lock( AccessTypes const & lock
		, uint32_t index )
	{
		REQUIRE( m_storage );
		return m_storage->lock( lock, index );
	}

	void TextureLayout::unlock( bool modified
		, uint32_t index )
	{
		REQUIRE( m_storage );
		m_storage->unlock( modified, index );
	}

	void TextureLayout::setSource( Path const & folder
		, Path const & relative )
	{
		m_images[0]->initialiseSource( folder, relative );
		auto buffer = m_images[0]->getBuffer();

		if ( m_size != buffer->dimensions()
			 || m_format != buffer->format() )
		{
			m_size = buffer->dimensions();
			m_format = buffer->format();
		}
	}

	void TextureLayout::setSource( PxBufferBaseSPtr buffer )
	{
		auto & image = m_images[0];

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

	bool TextureLayout::doResetStorage()
	{
		bool result = true;

		if ( m_storage )
		{
			auto type = m_storage->getType();
			m_storage.reset();
			result = doCreateStorage( type );
		}

		return result;
	}

	bool TextureLayout::doCreateStorage( TextureStorageType type )
	{
		bool result = false;

		if ( !m_storage )
		{
			try
			{
				m_storage = getRenderSystem()->createTextureStorage( type
					, *this
					, m_cpuAccess
					, m_gpuAccess );
				result = true;
			}
			catch ( std::exception & p_exc )
			{
				Logger::logError( StringStream() << cuT( "TextureImage::Initialise - Error encountered while allocating storage: " ) << string::stringCast< xchar >( p_exc.what() ) );
			}
		}

		ENSURE( m_storage );
		return result;
	}
}
