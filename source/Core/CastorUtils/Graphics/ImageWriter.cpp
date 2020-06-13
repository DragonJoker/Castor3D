#include "CastorUtils/Graphics/ImageWriter.hpp"

#include "CastorUtils/Data/LoaderException.hpp"
#include "CastorUtils/Data/Path.hpp"

namespace castor
{
	void ImageWriter::registerWriter( String const & extension, ImageWriterPtr writer )
	{
		m_writers.emplace_back( std::move( writer ) );
		auto ptr = m_writers.back().get();
		m_extWriters[string::lowerCase( extension )] = ptr;
	}

	void ImageWriter::registerWriter( StringArray const & extensions, ImageWriterPtr writer )
	{
		m_writers.emplace_back( std::move( writer ) );
		auto ptr = m_writers.back().get();

		for ( auto & extension : extensions )
		{
			m_extWriters[string::lowerCase( extension )] = ptr;
		}
	}

	void ImageWriter::unregisterWriter( String const & extension )
	{
		auto it = m_extWriters.find( string::lowerCase( extension ) );

		if ( it != m_extWriters.end() )
		{
			m_extWriters.erase( it );
		}
	}

	void ImageWriter::unregisterWriter( StringArray const & extensions )
	{
		for ( auto & extension : extensions )
		{
			unregisterWriter( string::lowerCase( extension ) );
		}
	}

	bool ImageWriter::write( Path const & path
		, PxBufferBase const & buffer )const
	{
		if ( path.empty() )
		{
			CU_LoaderError( "Can't write image: Path is empty" );
		}

		auto it = m_extWriters.find( string::lowerCase( path.getExtension() ) );
		bool result = false;

		if ( it != m_extWriters.end() )
		{
			result = it->second->write( path, buffer );
		}
		else
		{
			CU_LoaderError( "Can't write image: Unsupported image file extension (no writer registered for it)" );
		}

		return result;
	}
}
