#include "TextFile.hpp"

#include "Miscellaneous/Utils.hpp"

namespace castor
{
	TextFile::TextFile( Path const & p_fileName, FlagCombination< OpenMode > const & p_mode, EncodingMode p_encoding )
		: File{ p_fileName, p_mode & ~uint32_t( OpenMode::eBinary ), p_encoding }
	{
	}

	TextFile::~TextFile()
	{
	}

	uint64_t TextFile::readLine( String & p_toRead, uint64_t p_size, String p_strSeparators )
	{
		CHECK_INVARIANTS();
		REQUIRE( checkFlag( m_mode, OpenMode::eRead ) );
		uint64_t uiReturn = 0;
		p_toRead.clear();

		if ( isOk() )
		{
			xchar cChar;
			String strLine;
			bool bContinue = true;
			int iOrigChar;

			while ( bContinue && uiReturn < p_size )
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
					bContinue = p_strSeparators.find( cChar ) == String::npos;

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
					p_toRead += cChar;
				}

				uiReturn++;
				m_cursor++;
			}
		}

		ENSURE( uiReturn <= p_size );
		CHECK_INVARIANTS();
		return uiReturn;
	}

	uint64_t TextFile::readWord( String & p_toRead )
	{
		return readLine( p_toRead, 1024, cuT( " \r\n;,.\t" ) );
	}

	uint64_t TextFile::readChar( xchar & p_toRead )
	{
		CHECK_INVARIANTS();
		REQUIRE( checkFlag( m_mode, OpenMode::eRead ) );
		uint64_t uiReturn = 0;

		if ( isOk() )
		{
			int iOrigChar;

			if ( m_encoding == EncodingMode::eASCII )
			{
				iOrigChar = getc( m_file );
				p_toRead = string::stringCast< xchar, char >( { char( iOrigChar ), char( 0 ) } )[0];
			}
			else
			{
				iOrigChar = getwc( m_file );
				p_toRead = string::stringCast< xchar, wchar_t >( { wchar_t( iOrigChar ), wchar_t( 0 ) } )[0];
			}

			uiReturn++;
		}

		return uiReturn;
	}

	uint64_t TextFile::writeText( String const & p_line )
	{
		CHECK_INVARIANTS();
		REQUIRE( checkFlag( m_mode, OpenMode::eWrite ) || checkFlag( m_mode, OpenMode::eAppend ) );
		uint64_t uiReturn = 0;

		if ( isOk() )
		{
			if ( m_encoding != EncodingMode::eASCII )
			{
				uiReturn =  doWrite( reinterpret_cast< uint8_t const * >( p_line.c_str() ), sizeof( xchar ) * p_line.size() );
			}
			else
			{
				auto line = string::stringCast< char >( p_line );
				uiReturn =  doWrite( reinterpret_cast< uint8_t const * >( line.c_str() ), sizeof( char ) * line.size() );
			}
		}

		CHECK_INVARIANTS();
		return uiReturn;
	}

	uint64_t TextFile::copyToString( String & p_strOut )
	{
		CHECK_INVARIANTS();
		REQUIRE( checkFlag( m_mode, OpenMode::eRead ) );
		uint64_t uiReturn = 0;
		p_strOut.clear();
		String strLine;

		while ( isOk() )
		{
			uiReturn += readLine( strLine, 1024 );
			p_strOut += strLine + cuT( "\n" );
		}

		CHECK_INVARIANTS();
		return uiReturn;
	}

	uint64_t TextFile::print( uint64_t p_uiMaxSize, xchar const * p_pFormat, ... )
	{
		CHECK_INVARIANTS();
		REQUIRE( checkFlag( m_mode, OpenMode::eWrite ) || checkFlag( m_mode, OpenMode::eAppend ) );
		uint64_t uiReturn = 0;
		xchar * text = new xchar[std::size_t( p_uiMaxSize )];

		if ( p_pFormat )
		{
			va_list vaList;
			va_start( vaList, p_pFormat );
			cvsnprintf( text, std::size_t( p_uiMaxSize ), std::size_t( p_uiMaxSize ), p_pFormat, vaList );
			va_end( vaList );
			uiReturn = writeText( text );
		}

		delete [] text;
		ENSURE( uiReturn <= p_uiMaxSize );
		CHECK_INVARIANTS();
		return uiReturn;
	}
}
