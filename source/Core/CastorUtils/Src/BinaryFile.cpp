#include "BinaryFile.hpp"

namespace Castor
{
	BinaryFile::BinaryFile( Path const & p_fileName, int p_iMode, eENCODING_MODE p_encodingMode )
		:	File( p_fileName, p_iMode | eOPEN_MODE_BINARY, p_encodingMode	)
	{
	}

	BinaryFile::~BinaryFile()
	{
	}

	bool BinaryFile::Write( String const & p_strToWrite )
	{
		CHECK_INVARIANTS();
		REQUIRE( ( m_iMode & eOPEN_MODE_WRITE ) || ( m_iMode & eOPEN_MODE_APPEND ) );
		bool l_return = Write( uint32_t( p_strToWrite.size() ) ) == sizeof( uint32_t );

		if ( l_return && p_strToWrite.size() > 0 )
		{
			l_return = WriteArray< xchar >( p_strToWrite.c_str(), p_strToWrite.size() ) ==  p_strToWrite.size() * sizeof( xchar );
		}

		CHECK_INVARIANTS();
		return l_return;
	}

	bool BinaryFile::Read( String & p_strToRead )
	{
		CHECK_INVARIANTS();
		REQUIRE( m_iMode & eOPEN_MODE_READ );
		p_strToRead.clear();
		uint32_t l_uiSize = 0;
		bool l_return = Read( l_uiSize ) == sizeof( uint32_t );

		if ( l_return && l_uiSize > 0 )
		{
			std::vector< xchar > l_pTmp( l_uiSize + 1, 0 );
			l_return = ReadArray< xchar >( l_pTmp.data(), l_uiSize ) == l_uiSize * sizeof( xchar );

			if ( l_return )
			{
				p_strToRead = l_pTmp.data();
			}
		}

		CHECK_INVARIANTS();
		return l_return;
	}
}
