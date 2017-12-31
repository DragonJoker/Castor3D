template< typename T >
castor::TextFile & castor::operator<<( castor::TextFile & p_file, T const & p_toWrite )
{
	String tmp;
	tmp << p_toWrite;
	p_file.writeText( tmp );
	return p_file;
}

template< typename T >
castor::TextFile & castor::operator>>( castor::TextFile & p_file, T & p_toRead )
{
	String strWord;
	p_file.readWord( strWord );
	strWord >> p_toRead;
	return p_file;
}
