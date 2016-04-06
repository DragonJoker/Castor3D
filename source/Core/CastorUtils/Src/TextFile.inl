
template< typename T >
Castor::TextFile & Castor::operator <<( Castor::TextFile & p_file, T const & p_toWrite )
{
	String l_tmp;
	l_tmp << p_toWrite;
	p_file.WriteText( l_tmp );
	return p_file;
}

template< typename T >
Castor::TextFile & Castor::operator >>( Castor::TextFile & p_file, T & p_toRead )
{
	String l_strWord;
	p_file.ReadWord( l_strWord );
	l_strWord >> p_toRead;
	return p_file;
}
