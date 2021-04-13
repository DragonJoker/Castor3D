#include "CastorUtils/Graphics/ImageLoader.hpp"

#include "CastorUtils/Data/BinaryFile.hpp"
#include "CastorUtils/Data/LoaderException.hpp"
#include "CastorUtils/Data/Path.hpp"
#include "CastorUtils/Graphics/ImageLayout.hpp"

namespace castor
{
	//*********************************************************************************************

	namespace
	{
		Image postProcess( PxBufferConvertOptions const & options
			, Image image )
		{
			auto dstFormat = options.getCompressed( image.getPixelFormat() );

			if ( dstFormat == image.getPixelFormat() )
			{
				return image;
			}

			auto path = image.getPath();
			auto name = image.getName();
			auto layout = image.getLayout();
			auto buffer = PxBufferBase::create( &options
				, image.getDimensions()
				, dstFormat
				, image.getPxBuffer().getConstPtr()
				, image.getPixelFormat() );

			if ( image.getPxBuffer().isFlipped() )
			{
				buffer->flip();
			}

			ImageLayout newLayout{ layout.type, *buffer };
			return Image{ name
				, path
				, newLayout
				, std::move( buffer ) };
		}
	}

	//*********************************************************************************************

	Image ImageLoaderImpl::load( String const & name
		, Path const & imagePath
		, String const & imageFormat
		, uint8_t const * data
		, uint32_t size )const
	{
		PxBufferBaseSPtr buffer;
		auto layout = load( imageFormat, data, size, buffer );
		return Image{ name, imagePath, layout, std::move( buffer ) };
	}

	Image ImageLoaderImpl::load( String const & name
		, String const & imageFormat
		, uint8_t const * data
		, uint32_t size )const
	{
		return load( name
			, {}
			, imageFormat
			, data
			, size );
	}

	Image ImageLoaderImpl::load( String const & name
		, Path const & imagePath
		, uint8_t const * data
		, uint32_t size )const
	{
		return load( name
			, imagePath
			, string::lowerCase( imagePath.getExtension() )
			, data
			, size );
	}

	//*********************************************************************************************

	ImageLoader::ImageLoader( PxCompressionSupport support )
		: m_options{ std::move( support ) }
	{
	}

	ImageLoader::~ImageLoader()
	{
	}

	void ImageLoader::registerLoader( String const & extension, ImageLoaderPtr loader )
	{
		m_loaders.emplace_back( std::move( loader ) );
		auto ptr = m_loaders.back().get();
		m_extLoaders[string::lowerCase( extension )] = ptr;
	}

	void ImageLoader::registerLoader( StringArray const & extensions, ImageLoaderPtr loader )
	{
		m_loaders.emplace_back( std::move( loader ) );
		auto ptr = m_loaders.back().get();

		for ( auto & extension : extensions )
		{
			m_extLoaders[string::lowerCase( extension )] = ptr;
		}
	}

	void ImageLoader::unregisterLoader( String const & extension )
	{
		auto it = m_extLoaders.find( string::lowerCase( extension ) );

		if ( it != m_extLoaders.end() )
		{
			m_extLoaders.erase( it );
		}
	}

	void ImageLoader::unregisterLoader( StringArray const & extensions )
	{
		for ( auto & extension : extensions )
		{
			unregisterLoader( string::lowerCase( extension ) );
		}
	}

	Image ImageLoader::load( String const & name
		, Path const & path
		, bool allowCompression )const
	{
		if ( path.empty() )
		{
			CU_LoaderError( "Can't load image: Path is empty" );
		}

		ByteArray data;
		{
			BinaryFile file{ path, File::OpenMode::eRead };
			auto size = file.getLength();

			if ( !size )
			{
				CU_LoaderError( "Can't load image: Empty file" );
			}

			data.resize( size_t( size ) );

			if ( file.readArray( data.data(), data.size() ) < data.size() )
			{
				CU_LoaderError( "Can't load image: Couldn't read image file" );
			}
		}

		try
		{
			return load( name
				, path
				, data.data()
				, uint32_t( data.size() )
				, allowCompression );
		}
		catch ( std::exception & exc )
		{
			std::cerr << exc.what() << "\n" << path << std::endl;
			throw;
		}
	}

	Image ImageLoader::load( String const & name
		, String const & imageFormat
		, uint8_t const * data
		, uint32_t size
		, bool allowCompression )const
	{
		checkData( data, size );
		auto loader = findLoader( imageFormat );
		auto result = loader->load( name, imageFormat, data, size );

		if ( allowCompression )
		{
			result = postProcess( m_options
				, std::move( result ) );
		}

		return result;
	}

	Image ImageLoader::load( String const & name
		, Path const & imagePath
		, uint8_t const * data
		, uint32_t size
		, bool allowCompression )const
	{
		checkData( data, size );
		auto loader = findLoader( imagePath );
		auto result = loader->load( name, imagePath, data, size );

		if ( allowCompression )
		{
			result = postProcess( m_options
				, std::move( result ) );
		}

		return result;
	}

	void ImageLoader::checkData( uint8_t const * data
		, uint32_t size )const
	{
		if ( !data )
		{
			CU_LoaderError( "Can't load image: Null data" );
		}

		if ( !size )
		{
			CU_LoaderError( "Can't load image: Empty data" );
		}
	}

	ImageLoaderImpl * ImageLoader::findLoader( Path imagePath )const
	{
		return findLoader( string::lowerCase( imagePath.getExtension() ) );
	}

	ImageLoaderImpl * ImageLoader::findLoader( String imageFormat )const
	{
		auto it = m_extLoaders.find( string::lowerCase( imageFormat ) );

		if ( it == m_extLoaders.end() )
		{
			CU_LoaderError( "Can't load image: Unsupported image file format (no loader registered for it)" );
		}

		return it->second;
	}

	//*********************************************************************************************
}
