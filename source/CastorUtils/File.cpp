#include "CastorUtils/PrecompiledHeader.hpp"

#include "CastorUtils/File.hpp"
#include "CastorUtils/Math.hpp"
#include "CastorUtils/Logger.hpp"

#if defined( _WIN32 )
#	include <direct.h>
#	if defined( _MSC_VER )
#		pragma warning( push )
#		pragma warning( disable:4311 )
#		pragma warning( disable:4312 )
#	endif
#	include <windows.h>
#	if defined( _MSC_VER )
#		pragma warning( pop )
#	endif
#	define GetCurrentDir _getcwd
#else
#	include <unistd.h>
#	include <sys/types.h>
#	include <dirent.h>
#	include <errno.h>
#	define GetCurrentDir getcwd
#endif

using namespace Castor;

//******************************************************************************************************

File :: File( Path const & p_strFileName, int p_iMode, eENCODING_MODE p_eEncoding )
	:	m_iMode				( p_iMode		)
	,	m_eEncoding			( p_eEncoding	)
	,	m_strFileFullPath	( p_strFileName	)
	,	m_ullCursor			( 0				)
	,	m_ullLength			( 0				)
	,	m_pFile				( NULL			)
	,	m_bOwnFile			( true			)
{
	REQUIRE( ! p_strFileName.empty() );
	String l_strMode;

	switch( p_iMode )
	{
	case eOPEN_MODE_READ:
		l_strMode = cuT( "r" );
		break;

	case eOPEN_MODE_WRITE:
		l_strMode = cuT( "w" );
		break;

	case eOPEN_MODE_APPEND:
		l_strMode = cuT( "a" );
		break;

	case eOPEN_MODE_READ | eOPEN_MODE_BINARY:
		l_strMode = cuT( "rb" );
		break;

	case eOPEN_MODE_WRITE | eOPEN_MODE_BINARY:
		l_strMode = cuT( "wb" );
		break;

	case eOPEN_MODE_APPEND | eOPEN_MODE_BINARY:
		l_strMode = cuT( "ab" );
		break;

	case eOPEN_MODE_READ | eOPEN_MODE_WRITE:
		l_strMode = cuT( "r+" );
		break;

	case eOPEN_MODE_READ | eOPEN_MODE_WRITE | eOPEN_MODE_APPEND:
		l_strMode = cuT( "a+" );
		break;

	default:
		CASTOR_ASSERT( false );
		break;
	}

	if( (p_iMode & eOPEN_MODE_BINARY) == 0 )
	{
		switch( p_eEncoding )
		{
		case eENCODING_MODE_AUTO:
#ifdef _UNICODE
			m_eEncoding = eENCODING_MODE_UTF8;
			l_strMode += cuT( ", ccs=UTF-8" );
#else
			m_eEncoding = eENCODING_MODE_ASCII;
#endif
			break;

		case eENCODING_MODE_UNICODE:
			l_strMode += cuT( ", ccs=ANSI" );
			break;

		case eENCODING_MODE_UTF8:
			l_strMode += cuT( ", ccs=UTF-8" );
			break;

		case eENCODING_MODE_UTF16:
			l_strMode += cuT( ", ccs=UTF-16LE" );
		}
	}

	m_pFile = fopen( str_utils::to_str( m_strFileFullPath ).c_str(), str_utils::to_str( l_strMode ).c_str() );

	if( m_pFile )
	{
		m_ullLength = 0;
		Castor::FSeek( m_pFile, 0, SEEK_END );
		m_ullLength = Castor::FTell( m_pFile );
		Castor::FSeek( m_pFile, 0, SEEK_SET );
	}
	else
	{
		std::exception( str_utils::to_str( GetSystemError() ).c_str() );
	}

	CHECK_INVARIANTS();
}

File :: File( File const & p_file )
	:	m_iMode				( p_file.m_iMode			)
	,	m_eEncoding			( p_file.m_eEncoding		)
	,	m_strFileFullPath	( p_file.m_strFileFullPath	)
	,	m_ullCursor			( p_file.m_ullCursor		)
	,	m_ullLength			( p_file.m_ullLength		)
	,	m_pFile				( p_file.m_pFile			)
	,	m_bOwnFile			( false						)
{
}

File :: File( File && p_file )
	:	m_iMode				( 0						)
	,	m_eEncoding			( eENCODING_MODE_AUTO	)
	,	m_strFileFullPath	(						)
	,	m_ullCursor			( 0						)
	,	m_ullLength			( 0						)
	,	m_pFile				( NULL					)
	,	m_bOwnFile			( true					)
{
	m_iMode				= std::move( p_file.m_iMode				);
	m_eEncoding			= std::move( p_file.m_eEncoding			);
	m_strFileFullPath	= std::move( p_file.m_strFileFullPath	);
	m_ullCursor			= std::move( p_file.m_ullCursor			);
	m_ullLength			= std::move( p_file.m_ullLength			);
	m_pFile				= std::move( p_file.m_pFile				);
	m_bOwnFile			= std::move( p_file.m_bOwnFile			);

	p_file.m_iMode				= 0;
	p_file.m_eEncoding			= eENCODING_MODE_AUTO;
	p_file.m_strFileFullPath.clear();
	p_file.m_ullCursor			= 0;
	p_file.m_ullLength			= 0;
	p_file.m_pFile				= NULL;
	p_file.m_bOwnFile			= true;
}

File :: ~File()
{
	if( m_pFile != NULL && m_bOwnFile )
	{
		fclose( m_pFile );
	}
}

int File :: Seek( long long p_i64Offset, eOFFSET_MODE p_eOrigin )
{
	CHECK_INVARIANTS();
	int l_iReturn = 0;

	if( m_pFile )
	{
		switch( p_eOrigin )
		{
		case eOFFSET_MODE_BEGINNING:
			l_iReturn = Castor::FSeek( m_pFile, p_i64Offset, SEEK_SET );
			m_ullCursor = p_i64Offset;
			break;

		case eOFFSET_MODE_CURRENT:
			l_iReturn = Castor::FSeek( m_pFile, p_i64Offset, SEEK_CUR );
			m_ullCursor += p_i64Offset;
			break;

		case eOFFSET_MODE_END:
			l_iReturn = Castor::FSeek( m_pFile, p_i64Offset, SEEK_END );
			m_ullCursor = GetLength() - p_i64Offset;
			break;
		}
	}

	CHECK_INVARIANTS();
	return l_iReturn;
}

long long File :: GetLength()
{
	CHECK_INVARIANTS();

	m_ullLength = 0;
	long long l_llPosition = Castor::FTell( m_pFile );
	Castor::FSeek( m_pFile, 0, SEEK_END );
	m_ullLength = Castor::FTell( m_pFile );
	Castor::FSeek( m_pFile, l_llPosition, SEEK_SET );

	CHECK_INVARIANTS();
	return m_ullLength;
}

bool File :: IsOk()const
{
	bool l_bReturn = false;

	if( m_pFile )
	{
		if( ferror( m_pFile ) == 0 )
		{
			if( feof( m_pFile ) == 0 )
			{
				l_bReturn = true;
			}
		}
	}

	return l_bReturn;
}

long long File :: Tell()
{
	CHECK_INVARIANTS();
	long long l_llReturn = 0;

	if( m_pFile )
	{
		l_llReturn = Castor::FTell( m_pFile );
	}

	CHECK_INVARIANTS();
	return l_llReturn;
}

File & File :: operator =( File const & p_file )
{
	m_iMode				= p_file.m_iMode;
	m_eEncoding			= p_file.m_eEncoding;
	m_strFileFullPath	= p_file.m_strFileFullPath;
	m_ullCursor			= p_file.m_ullCursor;
	m_ullLength			= p_file.m_ullLength;
	m_pFile				= p_file.m_pFile;
	m_bOwnFile			= false;
	return * this;
}

File & File :: operator =( File && p_file)
{
	if (this != & p_file)
	{
		m_iMode				= std::move( p_file.m_iMode				);
		m_eEncoding			= std::move( p_file.m_eEncoding			);
		m_strFileFullPath	= std::move( p_file.m_strFileFullPath	);
		m_ullCursor			= std::move( p_file.m_ullCursor			);
		m_ullLength			= std::move( p_file.m_ullLength			);
		m_pFile				= std::move( p_file.m_pFile				);
		m_bOwnFile			= std::move( p_file.m_bOwnFile			);

		p_file.m_iMode				= 0;
		p_file.m_eEncoding			= eENCODING_MODE_AUTO;
		p_file.m_strFileFullPath.clear();
		p_file.m_ullCursor			= 0;
		p_file.m_ullLength			= 0;
		p_file.m_pFile				= NULL;
		p_file.m_bOwnFile			= true;
	}

	return * this;
}

BEGIN_INVARIANT_BLOCK( File )
	CHECK_INVARIANT( m_pFile );
END_INVARIANT_BLOCK()

uint64_t File :: DoWrite( uint8_t const * p_pBuffer, uint64_t p_uiSize )
{
	CHECK_INVARIANTS();
	REQUIRE( IsOk() && (m_iMode | eOPEN_MODE_WRITE) );
	uint64_t l_uiReturn = 0;

	if( IsOk() )
	{
		l_uiReturn = fwrite( p_pBuffer, 1, std::size_t( p_uiSize ), m_pFile );
		m_ullCursor += l_uiReturn;
		ENSURE( l_uiReturn <= p_uiSize );
	}

	CHECK_INVARIANTS();
	return l_uiReturn;
}

uint64_t File :: DoRead( uint8_t * p_pBuffer, uint64_t p_uiSize )
{
	CHECK_INVARIANTS();
	REQUIRE( IsOk() && (m_iMode | eOPEN_MODE_READ) );
	uint64_t l_uiReturn = 0;
	uint64_t l_uiPrev = 1;

	if( IsOk() )
	{
		while( l_uiReturn < p_uiSize && l_uiPrev != l_uiReturn )
		{
			l_uiPrev = l_uiReturn;
			l_uiReturn += fread( p_pBuffer, 1, std::size_t( p_uiSize - l_uiReturn ), m_pFile );
		}
		m_ullCursor += l_uiReturn;
		ENSURE( l_uiReturn <= p_uiSize );
	}

	CHECK_INVARIANTS();
	return l_uiReturn;
}

bool File :: ListDirectoryFiles( Path const & p_folderPath, StringArray & p_files, bool CU_PARAM_UNUSED( p_recursive ) )
{
	REQUIRE( ! p_folderPath.empty() );
	bool l_bReturn = false;
#if defined( _WIN32 )
	WIN32_FIND_DATA l_findData;
	HANDLE l_hHandle = ::FindFirstFile( (p_folderPath / cuT( "*.*" )).c_str(), &l_findData );
	String l_strBuffer;

	if( l_hHandle != INVALID_HANDLE_VALUE )
	{
		l_strBuffer = l_findData.cFileName;

		if( l_strBuffer != cuT( "." ) && l_strBuffer != cuT( ".." ) )
		{
			p_files.push_back( l_strBuffer );
		}

		l_bReturn = true;

		while( l_bReturn )
		{
			if( ::FindNextFile( l_hHandle, &l_findData ) && l_findData.cFileName != l_strBuffer )
			{
				l_strBuffer = l_findData.cFileName;

				if( l_strBuffer != cuT( "." ) && l_strBuffer != cuT( ".." ) )
				{
					p_files.push_back( p_folderPath / l_strBuffer );
				}
			}
			else
			{
				l_bReturn = false;
			}
		}

		l_bReturn = true;
	}
#elif defined( __linux__ )
	DIR * l_pDir;

	if( (l_pDir = opendir( str_utils::to_str( p_folderPath ).c_str() )) == NULL )
	{
		switch( errno )
		{
		case EACCES:
			Logger::LogWarning( cuT( "Can't open dir : Permission denied - Directory : " ) + p_folderPath );
			break;

		case EBADF:
			Logger::LogWarning( cuT( "Can't open dir : Invalid file descriptor - Directory : " ) + p_folderPath );
			break;

		case EMFILE:
			Logger::LogWarning( cuT( "Can't open dir : Too many file descriptor in use - Directory : " ) + p_folderPath );
			break;

		case ENFILE:
			Logger::LogWarning( cuT( "Can't open dir : Too many files currently open - Directory : " ) + p_folderPath );
			break;

		case ENOENT:
			Logger::LogWarning( cuT( "Can't open dir : Directory doesn't exist - Directory : " ) + p_folderPath );
			break;

		case ENOMEM:
			Logger::LogWarning( cuT( "Can't open dir : Insufficient memory - Directory : " ) + p_folderPath );
			break;

		case ENOTDIR:
			Logger::LogWarning( cuT( "Can't open dir : <name> is not a directory - Directory : " ) + p_folderPath );
			break;

		default:
			Logger::LogWarning( cuT( "Can't open dir : Unknown error - Directory : " ) + p_folderPath );
			break;
		}

		l_bReturn = false;
	}
	else
	{
		dirent * l_pDirent;

		while( (l_pDirent = readdir( l_pDir )) != NULL )
		{
			if( strcmp( l_pDirent->d_name, "." ) )
			{
				p_files.push_back( p_folderPath / str_utils::from_str( l_pDirent->d_name ) );
			}
		}

		closedir( l_pDir );
		l_bReturn = true;
	}
#else
#	error "Unsupported platform"
#endif

	return l_bReturn;
}

Path File :: DirectoryGetCurrent()
{
	Path l_pathReturn;

#if defined( _WIN32 )
	xchar l_pPath[FILENAME_MAX];
	DWORD dwResult = GetModuleFileName( NULL, l_pPath, sizeof( l_pPath ) );

	if( dwResult != 0 )
	{
		l_pathReturn = l_pPath;
	}
#elif defined( __linux__ )
	char l_pPath[FILENAME_MAX];
	char l_szTmp[32];
	sprintf( l_szTmp, "/proc/%d/exe", getpid() );
	int l_iBytes = std::min< std::size_t >( readlink( l_szTmp, l_pPath, sizeof( l_pPath ) ), sizeof( l_pPath ) - 1 );

	if( l_iBytes >= 0 )
	{
		l_pPath[l_iBytes] = '\0';
		l_pathReturn = l_pPath;
	}
#else
#	error "Unsupported platform"
#endif

	l_pathReturn = l_pathReturn.GetPath();
	return l_pathReturn;
}

bool File :: FileExists( Path const & p_pathFile )
{
	std::ifstream l_ifile( str_utils::to_str( p_pathFile ).c_str() );
	return l_ifile.is_open();
}

//*************************************************************************************************

TextFile :: TextFile( Path const & p_fileName, int p_iMode, eENCODING_MODE p_eEncodingMode )
	:	File	( p_fileName, p_iMode & (~eOPEN_MODE_BINARY), p_eEncodingMode	)
{
}

TextFile :: TextFile( TextFile const & p_file )
	:	File	( p_file	)
{
}

TextFile :: TextFile( TextFile && p_file )
	:	File	( std::move( p_file )	)
{
}

TextFile :: ~TextFile()
{
}

TextFile & TextFile :: operator =( TextFile const & p_file )
{
	File::operator =( p_file );
	return *this;
}

TextFile & TextFile :: operator =( TextFile && p_file )
{
	File::operator =( std::move( p_file ) );
	return *this;
}

uint64_t TextFile :: ReadLine( String & p_toRead, uint64_t p_size, String p_strSeparators )
{
	CHECK_INVARIANTS();
	REQUIRE( m_iMode & eOPEN_MODE_READ );

	uint64_t l_uiReturn = 0;
	p_toRead.clear();

	if( IsOk() )
	{
		xchar l_cChar;
		String l_strLine;
		bool l_bContinue = true;
		int l_iOrigChar;

		while( l_bContinue && l_uiReturn < p_size )
		{
			if( m_eEncoding == eENCODING_MODE_ASCII )
			{
				l_iOrigChar = getc( m_pFile );
				l_cChar = str_utils::from_char( char( l_iOrigChar ) )[0];
			}
			else
			{
				l_iOrigChar = getwc( m_pFile);
				l_cChar = str_utils::from_wchar( wchar_t( l_iOrigChar ) )[0];
			}

			l_bContinue =  ! feof( m_pFile );

			if( l_bContinue )
			{
				l_bContinue = p_strSeparators.find( l_cChar ) == String::npos;
/*
				if ( ! l_bContinue)
				{
					if (m_eEncoding == eASCII)
					{
						ungetc( int( String( l_cChar).char_str()[0]), m_pFile);
					}
					else
					{
						ungetwc( wint_t( String( l_cChar).wchar_str()[0]), m_pFile);
					}
				}
*/
			}

			if( l_bContinue )
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

uint64_t TextFile :: ReadWord( String & p_toRead )
{
	return ReadLine( p_toRead, 1024, cuT( " \r\n;,.\t" ) );
}

uint64_t TextFile :: ReadChar( xchar & p_toRead )
{
	CHECK_INVARIANTS();
	REQUIRE( m_iMode & eOPEN_MODE_READ );

	uint64_t l_uiReturn = 0;

	if( IsOk() )
	{
		int l_iOrigChar;

		if( m_eEncoding == eENCODING_MODE_ASCII )
		{
			l_iOrigChar = getc( m_pFile );
			p_toRead = str_utils::from_char( char( l_iOrigChar ) )[0];
		}
		else
		{
			l_iOrigChar = getwc( m_pFile );
			p_toRead = str_utils::from_wchar( wchar_t( l_iOrigChar ) )[0];
		}

		l_uiReturn++;
	}

	return l_uiReturn;
}

uint64_t TextFile :: WriteText( String const & p_strLine )
{
	CHECK_INVARIANTS();
	REQUIRE( (m_iMode & eOPEN_MODE_WRITE) || (m_iMode & eOPEN_MODE_APPEND) );

	uint64_t l_uiReturn = 0;

	if( IsOk() )
	{
		if( m_eEncoding != eENCODING_MODE_ASCII )
		{
			l_uiReturn =  DoWrite( reinterpret_cast< uint8_t const * >( str_utils::to_wstr( p_strLine ).c_str() ), sizeof( wchar_t ) * p_strLine.size() );
		}
		else
		{
			l_uiReturn =  DoWrite( reinterpret_cast< uint8_t const * >( str_utils::to_str( p_strLine ).c_str() ), sizeof( char ) * p_strLine.size() );
		}
	}

	CHECK_INVARIANTS();
	return l_uiReturn;
}

uint64_t TextFile :: CopyToString( String & p_strOut )
{
	CHECK_INVARIANTS();
	REQUIRE( m_iMode & eOPEN_MODE_READ );

	uint64_t l_uiReturn = 0;
	p_strOut.clear();
	String l_strLine;

	while( IsOk() )
	{
		l_uiReturn += ReadLine( l_strLine, 1024 );

		p_strOut += l_strLine + cuT( "\n" );
	}

	CHECK_INVARIANTS();
	return l_uiReturn;
}

uint64_t TextFile :: Print( uint64_t p_uiMaxSize, xchar const * p_pFormat, ... )
{
	CHECK_INVARIANTS();
	REQUIRE( (m_iMode & eOPEN_MODE_WRITE) || (m_iMode & eOPEN_MODE_APPEND) );

	uint64_t l_uiReturn = 0;
	xchar * l_pText = new xchar[std::size_t( p_uiMaxSize )];
	va_list l_vaList;

	if( p_pFormat )
	{
		va_start( l_vaList, p_pFormat );
		cvsnprintf( l_pText, std::size_t( p_uiMaxSize ), std::size_t( p_uiMaxSize ), p_pFormat, l_vaList );
		va_end( l_vaList );
		l_uiReturn = WriteText( l_pText );
	}

	delete_array l_pText;

	ENSURE( l_uiReturn <= p_uiMaxSize );
	CHECK_INVARIANTS();
	return l_uiReturn;
}

//*************************************************************************************************

BinaryFile :: BinaryFile( Path const & p_fileName, int p_iMode, eENCODING_MODE p_eEncodingMode )
	:	File	( p_fileName, p_iMode | eOPEN_MODE_BINARY, p_eEncodingMode	)
{
}

BinaryFile :: BinaryFile( BinaryFile const & p_file )
	:	File	( p_file	)
{
}

BinaryFile :: BinaryFile( BinaryFile && p_file )
	:	File	( std::move( p_file )	)
{
}

BinaryFile :: ~BinaryFile()
{
}

BinaryFile & BinaryFile :: operator =( BinaryFile const & p_file )
{
	File::operator =( p_file );
	return *this;
}

BinaryFile & BinaryFile :: operator =( BinaryFile && p_file )
{
	File::operator =( std::move( p_file ) );
	return *this;
}

bool BinaryFile :: Write( String const & p_strToWrite )
{
	CHECK_INVARIANTS();
	REQUIRE( (m_iMode & eOPEN_MODE_WRITE) || (m_iMode & eOPEN_MODE_APPEND) );

	bool l_bReturn = Write( uint32_t( p_strToWrite.size() ) ) == sizeof( uint32_t );

	if( l_bReturn && p_strToWrite.size() > 0 )
	{
		l_bReturn = WriteArray< xchar >( p_strToWrite.c_str(), p_strToWrite.size() ) ==  p_strToWrite.size() * sizeof( xchar );
	}

	CHECK_INVARIANTS();
	return l_bReturn;
}

bool BinaryFile :: Read( String & p_strToRead )
{
	CHECK_INVARIANTS();
	REQUIRE( m_iMode & eOPEN_MODE_READ );

	p_strToRead.clear();
	uint32_t l_uiSize = 0;
	bool l_bReturn = Read( l_uiSize ) == sizeof( uint32_t );

	if( l_bReturn && l_uiSize > 0 )
	{
		std::vector< xchar > l_pTmp( l_uiSize + 1, 0 );
		l_bReturn = ReadArray< xchar >( l_pTmp.data(), l_uiSize ) == l_uiSize * sizeof( xchar );

		if( l_bReturn )
		{
			p_strToRead = l_pTmp.data();
		}
	}

	CHECK_INVARIANTS();
	return l_bReturn;
}

//*************************************************************************************************
