template <typename T>
File & File :: operator <<( const T & p_toWrite)
{
	if (m_iMode & eBinary)
	{
		Write( p_toWrite);
	}
	else
	{
		String l_strTmp;
		l_strTmp << p_toWrite;
		WriteArray<xchar>( l_strTmp.c_str(), l_strTmp.size());
	}

	return *this;
}
template <typename T>
File & File :: operator >>( T & p_toRead)
{
	if (m_iMode & eBinary)
	{
		Read( p_toRead);
	}
	else
	{
		String l_strWord;
		ReadWord( l_strWord);
		l_strWord >> p_toRead;
	}

	return *this;
}
template <typename T>
size_t File :: Write( const T & p_toWrite)
{
	CASTOR_ASSERT( (m_iMode | eWrite) && IsOk());
	return _write( reinterpret_cast<const unsigned char *>( & p_toWrite), sizeof( T));
}
template <typename T>
size_t File :: Read( T & p_toRead)
{
	CASTOR_ASSERT( (m_iMode | eRead) && IsOk());
	return _read( reinterpret_cast <unsigned char *>( & p_toRead), sizeof( T));
}
template <typename T>
size_t File :: WriteArray( const T * p_toWrite, size_t p_count)
{
	CASTOR_ASSERT( (m_iMode | eWrite) && IsOk());
	return _write( reinterpret_cast <const unsigned char *>( p_toWrite), sizeof( T) * p_count);
}
template <typename T>
size_t File :: ReadArray( T * p_toRead, size_t p_count)
{
	CASTOR_ASSERT( (m_iMode | eRead) && IsOk());
	return _read( reinterpret_cast <unsigned char *>( & p_toRead), sizeof( T) * p_count);
}