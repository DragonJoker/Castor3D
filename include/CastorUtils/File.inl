//*************************************************************************************************

template< typename T >
TextFile & TextFile :: operator <<( T const & p_toWrite)
{
	String l_strTmp;
	l_strTmp << p_toWrite;
	WriteText( l_strTmp );
	return *this;
}

template< typename T >
TextFile & TextFile :: operator >>( T & p_toRead)
{
	String l_strWord;
	ReadWord( l_strWord );
	l_strWord >> p_toRead;
	return *this;
}

//*************************************************************************************************

template< typename T >
BinaryFile & BinaryFile :: operator <<( T const & p_toWrite)
{
	Write( p_toWrite );
	return *this;
}

template< typename T >
BinaryFile & BinaryFile :: operator >>( T & p_toRead)
{
	Read( p_toRead );
	return *this;
}

template< typename T >
uint64_t BinaryFile :: Write( T const & p_toWrite )
{
	return DoWrite( reinterpret_cast< uint8_t const * >( & p_toWrite ), sizeof( T ) );
}

template< typename T >
uint64_t BinaryFile :: Read( T & p_toRead )
{
	return DoRead( reinterpret_cast< uint8_t * >( & p_toRead ), sizeof( T ) );
}

template< typename T >
uint64_t BinaryFile :: WriteArray( T const * p_toWrite, uint64_t p_count )
{
	return DoWrite( reinterpret_cast< uint8_t const * >( p_toWrite ), sizeof( T ) * p_count );
}

template< typename T, uint64_t N >
uint64_t BinaryFile :: WriteArray( T const (& p_toWrite)[N] )
{
	return DoWrite( reinterpret_cast< uint8_t const * >( p_toWrite ), sizeof( T ) * N );
}

template< typename T >
uint64_t BinaryFile :: ReadArray( T * p_toRead, uint64_t p_count )
{
	return DoRead( reinterpret_cast< uint8_t * >( p_toRead ), sizeof( T ) * p_count );
}

template< typename T, uint64_t N >
uint64_t BinaryFile :: ReadArray( T (& p_toRead)[N] )
{
	return DoRead( reinterpret_cast< uint8_t * >( p_toRead ), sizeof( T ) * N );
}

//*************************************************************************************************
