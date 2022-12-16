namespace castor
{
	template< typename T >
	uint64_t BinaryFile::write( T const & toWrite )
	{
		return doWrite( reinterpret_cast< uint8_t const * >( &toWrite ), sizeof( T ) );
	}

	template< typename T >
	uint64_t BinaryFile::read( T & toRead )
	{
		return doRead( reinterpret_cast< uint8_t * >( &toRead ), sizeof( T ) );
	}

	template< typename T >
	uint64_t BinaryFile::writeArray( T const * toWrite, uint64_t count )
	{
		return doWrite( reinterpret_cast< uint8_t const * >( toWrite ), sizeof( T ) * count );
	}

	template< typename T, uint64_t N >
	uint64_t BinaryFile::writeArray( T const( & toWrite )[N] )
	{
		return doWrite( reinterpret_cast< uint8_t const * >( toWrite ), sizeof( T ) * N );
	}

	template< typename T >
	uint64_t BinaryFile::readArray( T * toRead, uint64_t count )
	{
		return doRead( reinterpret_cast< uint8_t * >( toRead ), sizeof( T ) * count );
	}

	template< typename T, uint64_t N >
	uint64_t BinaryFile::readArray( T( & toRead )[N] )
	{
		return doRead( reinterpret_cast< uint8_t * >( toRead ), sizeof( T ) * N );
	}
}

template< typename T >
castor::BinaryFile & castor::operator<<( castor::BinaryFile & file, T const & toWrite )
{
	file.write( toWrite );
	return file;
}

template< typename T >
castor::BinaryFile & castor::operator>>( castor::BinaryFile & file, T & toRead )
{
	file.read( toRead );
	return file;
}
