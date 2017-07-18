template< typename T >
Castor::TextFile & Castor::operator<<( Castor::TextFile & p_file, T const & p_toWrite )
{
	String tmp;
	tmp << p_toWrite;
	p_file.WriteText( tmp );
	return p_file;
}

template< typename T >
Castor::TextFile & Castor::operator>>( Castor::TextFile & p_file, T & p_toRead )
{
	String strWord;
	p_file.ReadWord( strWord );
	strWord >> p_toRead;
	return p_file;
}
