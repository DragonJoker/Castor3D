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

	TextFile::~TextFile()
	{
	}

	uint64_t TextFile::readLine( String & toRead
		, uint64_t size
		, String strSeparators )
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
					cChar = string::stringCast< xchar, char >( { char( iOrigChar ), char( 0 ) } )[0];
				}
				else
				{
					iOrigChar = getwc( m_file );
					cChar = string::stringCast< xchar, wchar_t >( { wchar_t( iOrigChar ), wchar_t( 0 ) } )[0];
				}

				bContinue =  ! feof( m_file );

				if ( bContinue )
				{
					bContinue = strSeparators.find( cChar ) == String::npos;

					//if ( ! bContinue)
					//{
					//	if (m_encoding == eASCII)
					//	{
					//		ungetc( int( String( cChar).char_str()[0]), m_file);
					//	}
					//	else
					//	{
					//		ungetwc( wint_t( String( cChar).wchar_str()[0]), m_file);
					//	}
					//}
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
				toRead = string::stringCast< xchar, char >( { char( iOrigChar ), char( 0 ) } )[0];
			}
			else
			{
				iOrigChar = getwc( m_file );
				toRead = string::stringCast< xchar, wchar_t >( { wchar_t( iOrigChar ), wchar_t( 0 ) } )[0];
			}

			uiReturn++;
		}

		return uiReturn;
	}

	uint64_t TextFile::writeText( String const & line )
	{
		CU_CheckInvariants();
		CU_Require( checkFlag( m_mode, OpenMode::eWrite ) || checkFlag( m_mode, OpenMode::eAppend ) );
		uint64_t uiReturn = 0;

		if ( isOk() )
		{
			if ( m_encoding != EncodingMode::eASCII )
			{
				uiReturn =  doWrite( reinterpret_cast< uint8_t const * >( line.c_str() ), sizeof( xchar ) * line.size() );
			}
			else
			{
				auto ln = string::stringCast< char >( line );
				uiReturn =  doWrite( reinterpret_cast< uint8_t const * >( ln.c_str() ), sizeof( char ) * ln.size() );
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

	uint64_t TextFile::print( uint64_t uiMaxSize, xchar const * pFormat, ... )
	{
		CU_CheckInvariants();
		CU_Require( checkFlag( m_mode, OpenMode::eWrite ) || checkFlag( m_mode, OpenMode::eAppend ) );
		uint64_t uiReturn = 0;
		xchar * text = new xchar[uiMaxSize];

		if ( pFormat )
		{
			va_list vaList;
			va_start( vaList, pFormat );
			vsnprintf( text, uiMaxSize, pFormat, vaList );
			va_end( vaList );
			uiReturn = writeText( text );
		}

		delete [] text;
		CU_Ensure( uiReturn <= uiMaxSize );
		CU_CheckInvariants();
		return uiReturn;
	}
}
