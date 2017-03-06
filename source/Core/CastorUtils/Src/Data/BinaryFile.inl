namespace Castor
{
	template< typename T >
	uint64_t BinaryFile::Write( T const & p_toWrite )
	{
		return DoWrite( reinterpret_cast< uint8_t const * >( &p_toWrite ), sizeof( T ) );
	}

	template< typename T >
	uint64_t BinaryFile::Read( T & p_toRead )
	{
		return DoRead( reinterpret_cast< uint8_t * >( &p_toRead ), sizeof( T ) );
	}

	template< typename T >
	uint64_t BinaryFile::WriteArray( T const * p_toWrite, uint64_t p_count )
	{
		return DoWrite( reinterpret_cast< uint8_t const * >( p_toWrite ), sizeof( T ) * p_count );
	}

	template< typename T, uint64_t N >
	uint64_t BinaryFile::WriteArray( T const( & p_toWrite )[N] )
	{
		return DoWrite( reinterpret_cast< uint8_t const * >( p_toWrite ), sizeof( T ) * N );
	}

	template< typename T >
	uint64_t BinaryFile::ReadArray( T * p_toRead, uint64_t p_count )
	{
		return DoRead( reinterpret_cast< uint8_t * >( p_toRead ), sizeof( T ) * p_count );
	}

	template< typename T, uint64_t N >
	uint64_t BinaryFile::ReadArray( T( & p_toRead )[N] )
	{
		return DoRead( reinterpret_cast< uint8_t * >( p_toRead ), sizeof( T ) * N );
	}
}

template< typename T >
Castor::BinaryFile & Castor::operator<<( Castor::BinaryFile & p_file, T const & p_toWrite )
{
	p_file.Write( p_toWrite );
	return p_file;
}

template< typename T >
Castor::BinaryFile & Castor::operator>>( Castor::BinaryFile & p_file, T & p_toRead )
{
	p_file.Read( p_toRead );
	return p_file;
}
