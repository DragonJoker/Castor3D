#include "BinaryFile.hpp"

#include "Miscellaneous/Utils.hpp"

namespace castor
{
	BinaryFile::BinaryFile( Path const & p_fileName, FlagCombination< OpenMode > const & p_mode, EncodingMode p_encodingMode )
		: File{ p_fileName, p_mode | OpenMode::eBinary, p_encodingMode }
	{
	}

	BinaryFile::~BinaryFile()
	{
	}

	bool BinaryFile::write( String const & p_toWrite )
	{
		CHECK_INVARIANTS();
		REQUIRE( checkFlag( m_mode, OpenMode::eWrite ) || checkFlag( m_mode, OpenMode::eAppend ) );
		bool result = write( uint32_t( p_toWrite.size() ) ) == sizeof( uint32_t );

		if ( result && p_toWrite.size() > 0 )
		{
			result = writeArray< xchar >( p_toWrite.c_str(), p_toWrite.size() ) ==  p_toWrite.size() * sizeof( xchar );
		}

		CHECK_INVARIANTS();
		return result;
	}

	bool BinaryFile::read( String & p_toRead )
	{
		CHECK_INVARIANTS();
		REQUIRE( checkFlag( m_mode, OpenMode::eRead ) );
		p_toRead.clear();
		uint32_t uiSize = 0;
		bool result = read( uiSize ) == sizeof( uint32_t );

		if ( result && uiSize > 0 )
		{
			std::vector< xchar > pTmp( uiSize + 1, 0 );
			result = readArray< xchar >( pTmp.data(), uiSize ) == uiSize * sizeof( xchar );

			if ( result )
			{
				p_toRead = pTmp.data();
			}
		}

		CHECK_INVARIANTS();
		return result;
	}
}
