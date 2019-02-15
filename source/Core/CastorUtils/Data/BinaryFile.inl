namespace castor
{
	template< typename T >
	uint64_t BinaryFile::write( T const & p_toWrite )
	{
		return doWrite( reinterpret_cast< uint8_t const * >( &p_toWrite ), sizeof( T ) );
	}

	template< typename T >
	uint64_t BinaryFile::read( T & p_toRead )
	{
		return doRead( reinterpret_cast< uint8_t * >( &p_toRead ), sizeof( T ) );
	}

	template< typename T >
	uint64_t BinaryFile::writeArray( T const * p_toWrite, uint64_t p_count )
	{
		return doWrite( reinterpret_cast< uint8_t const * >( p_toWrite ), sizeof( T ) * p_count );
	}

	template< typename T, uint64_t N >
	uint64_t BinaryFile::writeArray( T const( & p_toWrite )[N] )
	{
		return doWrite( reinterpret_cast< uint8_t const * >( p_toWrite ), sizeof( T ) * N );
	}

	template< typename T >
	uint64_t BinaryFile::readArray( T * p_toRead, uint64_t p_count )
	{
		return doRead( reinterpret_cast< uint8_t * >( p_toRead ), sizeof( T ) * p_count );
	}

	template< typename T, uint64_t N >
	uint64_t BinaryFile::readArray( T( & p_toRead )[N] )
	{
		return doRead( reinterpret_cast< uint8_t * >( p_toRead ), sizeof( T ) * N );
	}
}

template< typename T >
castor::BinaryFile & castor::operator<<( castor::BinaryFile & p_file, T const & p_toWrite )
{
	p_file.write( p_toWrite );
	return p_file;
}

template< typename T >
castor::BinaryFile & castor::operator>>( castor::BinaryFile & p_file, T & p_toRead )
{
	p_file.read( p_toRead );
	return p_file;
}
