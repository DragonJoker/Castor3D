template< typename T >
castor::TextFile & castor::operator<<( castor::TextFile & file, T const & toWrite )
{
	String tmp;
	tmp << toWrite;
	file.writeText( tmp );
	return file;
}

template< typename T >
castor::TextFile & castor::operator>>( castor::TextFile & file, T & toRead )
{
	String strWord;
	file.readWord( strWord );
	strWord >> toRead;
	return file;
}
