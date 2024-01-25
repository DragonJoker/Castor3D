namespace castor
{
	template< typename T >
	TextFile & operator<<( TextFile & file, T const & toWrite )
	{
		String tmp;
		tmp << toWrite;
		file.writeText( tmp );
		return file;
	}

	template< typename T >
	TextFile & operator>>( TextFile & file, T & toRead )
	{
		String strWord;
		file.readWord( strWord );
		strWord >> toRead;
		return file;
	}
}
