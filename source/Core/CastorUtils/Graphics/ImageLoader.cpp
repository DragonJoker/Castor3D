#include "CastorUtils/Graphics/ImageLoader.hpp"

#include "CastorUtils/Data/BinaryFile.hpp"
#include "CastorUtils/Data/LoaderException.hpp"
#include "CastorUtils/Data/Path.hpp"
#include "CastorUtils/Graphics/ImageLayout.hpp"

namespace castor
{
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
		, Path const & path )const
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
				, path.getExtension()
				, data.data()
				, uint32_t( data.size() ) );
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

		auto it = m_extLoaders.find( string::lowerCase( imageFormat ) );

		if ( it == m_extLoaders.end() )
		{
			CU_LoaderError( "Can't load image: Unsupported image file format (no loader registered for it)" );
		}

		return it->second->load( name, imageFormat, data, size );
	}
}
