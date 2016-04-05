#include "BinaryFile.hpp"

namespace Castor
{
	BinaryFile::BinaryFile( Path const & p_fileName, int p_mode, eENCODING_MODE p_encodingMode )
		:	File( p_fileName, p_mode | eOPEN_MODE_BINARY, p_encodingMode	)
	{
	}

	BinaryFile::~BinaryFile()
	{
	}

	bool BinaryFile::Write( String const & p_toWrite )
	{
		CHECK_INVARIANTS();
		REQUIRE( ( m_iMode & eOPEN_MODE_WRITE ) || ( m_iMode & eOPEN_MODE_APPEND ) );
		bool l_return = Write( uint32_t( p_toWrite.size() ) ) == sizeof( uint32_t );

		if ( l_return && p_toWrite.size() > 0 )
		{
			l_return = WriteArray< xchar >( p_toWrite.c_str(), p_toWrite.size() ) ==  p_toWrite.size() * sizeof( xchar );
		}

		CHECK_INVARIANTS();
		return l_return;
	}

	bool BinaryFile::Read( String & p_toRead )
	{
		CHECK_INVARIANTS();
		REQUIRE( m_iMode & eOPEN_MODE_READ );
		p_toRead.clear();
		uint32_t l_uiSize = 0;
		bool l_return = Read( l_uiSize ) == sizeof( uint32_t );

		if ( l_return && l_uiSize > 0 )
		{
			std::vector< xchar > l_pTmp( l_uiSize + 1, 0 );
			l_return = ReadArray< xchar >( l_pTmp.data(), l_uiSize ) == l_uiSize * sizeof( xchar );

			if ( l_return )
			{
				p_toRead = l_pTmp.data();
			}
		}

		CHECK_INVARIANTS();
		return l_return;
	}
}
