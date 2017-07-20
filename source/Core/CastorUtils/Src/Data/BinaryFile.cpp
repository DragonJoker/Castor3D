#include "BinaryFile.hpp"

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
		bool result = Write( uint32_t( p_toWrite.size() ) ) == sizeof( uint32_t );

		if ( result && p_toWrite.size() > 0 )
		{
			result = WriteArray< xchar >( p_toWrite.c_str(), p_toWrite.size() ) ==  p_toWrite.size() * sizeof( xchar );
		}

		CHECK_INVARIANTS();
		return result;
	}

	bool BinaryFile::Read( String & p_toRead )
	{
		CHECK_INVARIANTS();
		REQUIRE( CheckFlag( m_mode, OpenMode::eRead ) );
		p_toRead.clear();
		uint32_t uiSize = 0;
		bool result = Read( uiSize ) == sizeof( uint32_t );

		if ( result && uiSize > 0 )
		{
			std::vector< xchar > pTmp( uiSize + 1, 0 );
			result = ReadArray< xchar >( pTmp.data(), uiSize ) == uiSize * sizeof( xchar );

			if ( result )
			{
				p_toRead = pTmp.data();
			}
		}

		CHECK_INVARIANTS();
		return result;
	}
}
