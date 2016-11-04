#include "TextFile.hpp"

#include "Miscellaneous/Utils.hpp"

namespace Castor
{
	TextFile::TextFile( Path const & p_fileName, OpenMode p_mode, EncodingMode p_encoding )
		: TextFile{ p_fileName, uint32_t( p_mode ), p_encoding }
	{
	}

	TextFile::TextFile( Path const & p_fileName, uint32_t p_mode, EncodingMode p_encodingMode )
		: File{ p_fileName, p_mode & ~uint32_t( OpenMode::eBinary ), p_encodingMode }
	{
	}

	TextFile::~TextFile()
	{
	}

	uint64_t TextFile::ReadLine( String & p_toRead, uint64_t p_size, String p_strSeparators )
	{
		CHECK_INVARIANTS();
		REQUIRE( CheckFlag( m_mode, OpenMode::eRead ) );
		uint64_t l_uiReturn = 0;
		p_toRead.clear();

		if ( IsOk() )
		{
			xchar l_cChar;
			String l_strLine;
			bool l_bContinue = true;
			int l_iOrigChar;

			while ( l_bContinue && l_uiReturn < p_size )
			{
				if ( m_encoding == EncodingMode::eASCII )
				{
					l_iOrigChar = getc( m_pFile );
					l_cChar = string::string_cast< xchar, char >( { char( l_iOrigChar ), char( 0 ) } )[0];
				}
				else
				{
					l_iOrigChar = getwc( m_pFile );
					l_cChar = string::string_cast< xchar, wchar_t >( { wchar_t( l_iOrigChar ), wchar_t( 0 ) } )[0];
				}

				l_bContinue =  ! feof( m_pFile );

				if ( l_bContinue )
				{
					l_bContinue = p_strSeparators.find( l_cChar ) == String::npos;

					//if ( ! l_bContinue)
					//{
					//	if (m_encoding == eASCII)
					//	{
					//		ungetc( int( String( l_cChar).char_str()[0]), m_pFile);
					//	}
					//	else
					//	{
					//		ungetwc( wint_t( String( l_cChar).wchar_str()[0]), m_pFile);
					//	}
					//}
				}

				if ( l_bContinue )
				{
					p_toRead += l_cChar;
				}

				l_uiReturn++;
				m_ullCursor++;
			}
		}

		ENSURE( l_uiReturn <= p_size );
		CHECK_INVARIANTS();
		return l_uiReturn;
	}

	uint64_t TextFile::ReadWord( String & p_toRead )
	{
		return ReadLine( p_toRead, 1024, cuT( " \r\n;,.\t" ) );
	}

	uint64_t TextFile::ReadChar( xchar & p_toRead )
	{
		CHECK_INVARIANTS();
		REQUIRE( CheckFlag( m_mode, OpenMode::eRead ) );
		uint64_t l_uiReturn = 0;

		if ( IsOk() )
		{
			int l_iOrigChar;

			if ( m_encoding == EncodingMode::eASCII )
			{
				l_iOrigChar = getc( m_pFile );
				p_toRead = string::string_cast< xchar, char >( { char( l_iOrigChar ), char( 0 ) } )[0];
			}
			else
			{
				l_iOrigChar = getwc( m_pFile );
				p_toRead = string::string_cast< xchar, wchar_t >( { wchar_t( l_iOrigChar ), wchar_t( 0 ) } )[0];
			}

			l_uiReturn++;
		}

		return l_uiReturn;
	}

	uint64_t TextFile::WriteText( String const & p_line )
	{
		CHECK_INVARIANTS();
		REQUIRE( CheckFlag( m_mode, OpenMode::eWrite ) || CheckFlag( m_mode, OpenMode::eAppend ) );
		uint64_t l_uiReturn = 0;

		if ( IsOk() )
		{
			if ( m_encoding != EncodingMode::eASCII )
			{
				l_uiReturn =  DoWrite( reinterpret_cast< uint8_t const * >( p_line.c_str() ), sizeof( xchar ) * p_line.size() );
			}
			else
			{
				auto l_line = string::string_cast< char >( p_line );
				l_uiReturn =  DoWrite( reinterpret_cast< uint8_t const * >( l_line.c_str() ), sizeof( char ) * l_line.size() );
			}
		}

		CHECK_INVARIANTS();
		return l_uiReturn;
	}

	uint64_t TextFile::CopyToString( String & p_strOut )
	{
		CHECK_INVARIANTS();
		REQUIRE( CheckFlag( m_mode, OpenMode::eRead ) );
		uint64_t l_uiReturn = 0;
		p_strOut.clear();
		String l_strLine;

		while ( IsOk() )
		{
			l_uiReturn += ReadLine( l_strLine, 1024 );
			p_strOut += l_strLine + cuT( "\n" );
		}

		CHECK_INVARIANTS();
		return l_uiReturn;
	}

	uint64_t TextFile::Print( uint64_t p_uiMaxSize, xchar const * p_pFormat, ... )
	{
		CHECK_INVARIANTS();
		REQUIRE( CheckFlag( m_mode, OpenMode::eWrite ) || CheckFlag( m_mode, OpenMode::eAppend ) );
		uint64_t l_uiReturn = 0;
		xchar * l_text = new xchar[std::size_t( p_uiMaxSize )];
		va_list l_vaList;

		if ( p_pFormat )
		{
			va_start( l_vaList, p_pFormat );
			cvsnprintf( l_text, std::size_t( p_uiMaxSize ), std::size_t( p_uiMaxSize ), p_pFormat, l_vaList );
			va_end( l_vaList );
			l_uiReturn = WriteText( l_text );
		}

		delete [] l_text;
		ENSURE( l_uiReturn <= p_uiMaxSize );
		CHECK_INVARIANTS();
		return l_uiReturn;
	}
}
