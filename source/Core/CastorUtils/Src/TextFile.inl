namespace Castor
{
	template< typename T >
	TextFile & TextFile::operator <<( T const & p_toWrite )
	{
		String l_strTmp;
		l_strTmp << p_toWrite;
		WriteText( l_strTmp );
		return *this;
	}

	template< typename T >
	TextFile & TextFile::operator >>( T & p_toRead )
	{
		String l_strWord;
		ReadWord( l_strWord );
		l_strWord >> p_toRead;
		return *this;
	}
}
