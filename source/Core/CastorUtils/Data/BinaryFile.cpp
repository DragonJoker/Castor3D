#include "CastorUtils/Data/BinaryFile.hpp"

#include "CastorUtils/Miscellaneous/Utils.hpp"

namespace castor
{
	BinaryFile::BinaryFile( Path const & fileName
		, FlagCombination< OpenMode > const & mode
		, EncodingMode encodingMode )
		: File{ fileName, mode | OpenMode::eBinary, encodingMode }
	{
	}

	bool BinaryFile::write( String const & toWrite )
	{
		CU_CheckInvariants();
		CU_Require( checkFlag( m_mode, OpenMode::eWrite ) || checkFlag( m_mode, OpenMode::eAppend ) );
		bool result = write( uint32_t( toWrite.size() ) ) == sizeof( uint32_t );

		if ( result && toWrite.size() > 0 )
		{
			result = writeArray< xchar >( toWrite.c_str(), toWrite.size() ) ==  toWrite.size() * sizeof( xchar );
		}

		CU_CheckInvariants();
		return result;
	}

	bool BinaryFile::read( String & toRead )
	{
		CU_CheckInvariants();
		CU_Require( checkFlag( m_mode, OpenMode::eRead ) );
		toRead.clear();
		uint32_t uiSize = 0;
		bool result = read( uiSize ) == sizeof( uint32_t );

		if ( result && uiSize > 0 )
		{
			std::vector< xchar > pTmp( uiSize + 1, 0 );
			result = readArray< xchar >( pTmp.data(), uiSize ) == uiSize * sizeof( xchar );

			if ( result )
			{
				toRead = pTmp.data();
			}
		}

		CU_CheckInvariants();
		return result;
	}
}
