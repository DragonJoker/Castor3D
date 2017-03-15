#include "BinaryFile.hpp"

#include "Design/FlagCombination.hpp"
#include "Miscellaneous/Utils.hpp"

namespace Castor
{
	BinaryFile::BinaryFile( Path const & p_fileName, FlagCombination< OpenMode > const & p_mode, EncodingMode p_encodingMode )
		: File{ p_fileName, p_mode | OpenMode::eBinary, p_encodingMode }
	{
	}

	BinaryFile::~BinaryFile()
	{
	}

	bool BinaryFile::Write( String const & p_toWrite )
	{
		CHECK_INVARIANTS();
		REQUIRE( CheckFlag( m_mode, OpenMode::eWrite ) || CheckFlag( m_mode, OpenMode::eAppend ) );
		bool l_result = Write( uint32_t( p_toWrite.size() ) ) == sizeof( uint32_t );

		if ( l_result && p_toWrite.size() > 0 )
		{
			l_result = WriteArray< xchar >( p_toWrite.c_str(), p_toWrite.size() ) ==  p_toWrite.size() * sizeof( xchar );
		}

		CHECK_INVARIANTS();
		return l_result;
	}

	bool BinaryFile::Read( String & p_toRead )
	{
		CHECK_INVARIANTS();
		REQUIRE( CheckFlag( m_mode, OpenMode::eRead ) );
		p_toRead.clear();
		uint32_t l_uiSize = 0;
		bool l_result = Read( l_uiSize ) == sizeof( uint32_t );

		if ( l_result && l_uiSize > 0 )
		{
			std::vector< xchar > l_pTmp( l_uiSize + 1, 0 );
			l_result = ReadArray< xchar >( l_pTmp.data(), l_uiSize ) == l_uiSize * sizeof( xchar );

			if ( l_result )
			{
				p_toRead = l_pTmp.data();
			}
		}

		CHECK_INVARIANTS();
		return l_result;
	}
}
