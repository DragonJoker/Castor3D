#include "CastorUtils/Data/TextFile.hpp"

#include "CastorUtils/Miscellaneous/Utils.hpp"

namespace castor
{
	TextFile::TextFile( Path const & fileName
		, FlagCombination< OpenMode > const & mode
		, EncodingMode encoding )
		: File{ fileName, mode & ~uint32_t( OpenMode::eBinary ), encoding }
	{
	}

	uint64_t TextFile::readLine( String & toRead
		, uint64_t size
		, StringView strSeparators )
	{
		CU_CheckInvariants();
		CU_Require( checkFlag( m_mode, OpenMode::eRead ) );
		uint64_t uiReturn = 0;
		toRead.clear();

		if ( isOk() )
		{
			xchar cChar;
			String strLine;
			bool bContinue = true;
			int iOrigChar;

			while ( bContinue && uiReturn < size )
			{
				if ( m_encoding == EncodingMode::eASCII )
				{
					iOrigChar = getc( m_file );
					cChar = makeString( MbString{ char( iOrigChar ), char( 0 ) } )[0];
				}
				else
				{
					iOrigChar = getwc( m_file );
					cChar = makeString( WString{ wchar_t( iOrigChar ), wchar_t( 0 ) } )[0];
				}

				bContinue =  ! feof( m_file );

				if ( bContinue )
				{
					bContinue = strSeparators.find( cChar ) == String::npos;
				}

				if ( bContinue )
				{
					toRead += cChar;
				}

				uiReturn++;
				m_cursor++;
			}
		}

		CU_Ensure( uiReturn <= size );
		CU_CheckInvariants();
		return uiReturn;
	}

	uint64_t TextFile::readWord( String & toRead )
	{
		return readLine( toRead, 1024, cuT( " \r\n;,.\t" ) );
	}

	uint64_t TextFile::readChar( xchar & toRead )
	{
		CU_CheckInvariants();
		CU_Require( checkFlag( m_mode, OpenMode::eRead ) );
		uint64_t uiReturn = 0;

		if ( isOk() )
		{
			int iOrigChar;

			if ( m_encoding == EncodingMode::eASCII )
			{
				iOrigChar = getc( m_file );
				toRead = makeString( MbString{ char( iOrigChar ), char( 0 ) } )[0];
			}
			else
			{
				iOrigChar = getwc( m_file );
				toRead = makeString( WString{ wchar_t( iOrigChar ), wchar_t( 0 ) } )[0];
			}

			uiReturn++;
		}

		return uiReturn;
	}

	uint64_t TextFile::writeText( String const & line )
	{
		using ByteCPtr = uint8_t const *;

		CU_CheckInvariants();
		CU_Require( checkFlag( m_mode, OpenMode::eWrite ) || checkFlag( m_mode, OpenMode::eAppend ) );
		uint64_t uiReturn = 0;

		if ( isOk() )
		{
			if ( m_encoding != EncodingMode::eASCII )
			{
				uiReturn = doWrite( ByteCPtr( line.c_str() ), sizeof( xchar ) * line.size() );
			}
			else
			{
				auto ln = toUtf8( line );
				uiReturn = doWrite( ByteCPtr( ln.c_str() ), sizeof( char ) * ln.size() );
			}
		}

		CU_CheckInvariants();
		return uiReturn;
	}

	uint64_t TextFile::copyToString( String & strOut )
	{
		CU_CheckInvariants();
		CU_Require( checkFlag( m_mode, OpenMode::eRead ) );
		uint64_t uiReturn = 0;
		strOut.clear();
		String strLine;

		while ( isOk() )
		{
			uiReturn += readLine( strLine, 1024 );
			strOut += strLine + cuT( "\n" );
		}

		CU_CheckInvariants();
		return uiReturn;
	}
}
