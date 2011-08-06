#include "CastorUtils/PrecompiledHeader.hpp"

#include "CastorUtils/File.hpp"
#include "CastorUtils/Math.hpp"

#if CHECK_MEMORYLEAKS
#	include "CastorUtils/Memory.hpp"
using namespace Castor::Utils;
#endif

#ifdef _WIN32
#	include <direct.h>
#	include <Windows.h>
#	define GetCurrentDir _getcwd
#else
#	include <unistd.h>
#	define GetCurrentDir getcwd
#endif

using namespace boost::filesystem;
using namespace Castor;
using namespace Castor::Utils;

//******************************************************************************************************

File :: File( Path const & p_strFileName, int p_iMode, eENCODING_MODE p_eEncoding)
	:	m_iMode( p_iMode)
	,	m_eEncoding( p_eEncoding)
	,	m_strFileFullPath( p_strFileName)
	,	m_ullCursor( 0)
	,	m_ullLength( 0)
{
	REQUIRE( ! p_strFileName.empty());
	String l_strMode;

	switch (p_iMode)
	{
	case eOPEN_MODE_READ:
		l_strMode = cuT( "r");
		break;

	case eOPEN_MODE_WRITE:
		l_strMode = cuT( "w");
		break;

	case eOPEN_MODE_APPEND:
		l_strMode = cuT( "a");
		break;

	case eOPEN_MODE_READ | eOPEN_MODE_BINARY:
		l_strMode = cuT( "rb");
		break;

	case eOPEN_MODE_WRITE | eOPEN_MODE_BINARY:
		l_strMode = cuT( "wb");
		break;

	case eOPEN_MODE_APPEND | eOPEN_MODE_BINARY:
		l_strMode = cuT( "ab");
		break;

	case eOPEN_MODE_READ | eOPEN_MODE_WRITE:
		l_strMode = cuT( "r+");
		break;

	case eOPEN_MODE_READ | eOPEN_MODE_WRITE | eOPEN_MODE_APPEND:
		l_strMode = cuT( "a+");
		break;
	}

	if ((p_iMode & eOPEN_MODE_BINARY) == 0)
	{
		switch (p_eEncoding)
		{
		case eENCODING_MODE_AUTO:
#ifdef _UNICODE
			l_strMode += cuT( ", ccs=UTF-8");
#endif
			break;

		case eENCODING_MODE_UNICODE:
			l_strMode += cuT( ", ccs=ANSI");
			break;

		case eENCODING_MODE_UTF8:
			l_strMode += cuT( ", ccs=UTF-8");
			break;

		case eENCODING_MODE_UTF16:
			l_strMode += cuT( ", ccs=UTF-16LE");
		}
	}

	m_pFile = fopen( m_strFileFullPath.char_str(), l_strMode.char_str());

	if (m_pFile == NULL)
	{
		CASTOR_EXCEPTION( cuT( "Impossible de charger le fichier ") + p_strFileName);
	}

	m_ullLength = 0;
	FSeek( m_pFile, 0, SEEK_END);
	m_ullLength = FTell( m_pFile);
	FSeek( m_pFile, 0, SEEK_SET);
	CHECK_INVARIANTS();
}

File :: ~File()
{
	if (m_pFile)
	{
		fclose( m_pFile);
	}
}

BEGIN_INVARIANT_BLOCK( File)
	CHECK_INVARIANT( m_pFile);
END_INVARIANT_BLOCK()

int File :: Seek( long long p_llOffset, eOFFSET_MODE p_eOrigin)
{
	CHECK_INVARIANTS();
	int l_iReturn = 0;

	switch (p_eOrigin)
	{
	case eOFFSET_MODE_BEGINNING:
		l_iReturn = FSeek( m_pFile, p_llOffset, SEEK_SET);
		m_ullCursor = p_llOffset;
		break;

	case eOFFSET_MODE_CURRENT:
		l_iReturn = FSeek( m_pFile, p_llOffset, SEEK_CUR);
		m_ullCursor += p_llOffset;
		break;

	case eOFFSET_MODE_END:
		l_iReturn = FSeek( m_pFile, p_llOffset, SEEK_END);
		m_ullCursor = GetLength() - p_llOffset;
		break;
	}

	CHECK_INVARIANTS();
	return l_iReturn;
}

int File :: Seek( long long p_llPosition)
{
	return Seek( p_llPosition, eOFFSET_MODE_BEGINNING);
}

size_t File :: ReadLine( String & p_toRead, size_t p_size, String p_strSeparators)
{
	CHECK_INVARIANTS();
	REQUIRE( m_iMode & eOPEN_MODE_READ);

	size_t l_uiReturn = 0;
	p_toRead.clear();

	if (IsOk())
	{
		xchar l_cChar;
		String l_strLine;
		bool l_bContinue = true;
		int l_iOrigChar;

		while (l_bContinue && l_uiReturn < p_size)
		{
			if (m_eEncoding == eENCODING_MODE_ASCII)
			{
				l_iOrigChar = getc( m_pFile);
				l_cChar = String( char( l_iOrigChar))[0];
			}
			else
			{
				l_iOrigChar = getwc( m_pFile);
				l_cChar = String( wchar_t( l_iOrigChar))[0];
			}

			l_bContinue =  ! feof( m_pFile);

			if (l_bContinue)
			{
				l_bContinue = p_strSeparators.find( l_cChar) == String::npos;
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

			if (l_bContinue)
			{
				p_toRead += l_cChar;
			}

			l_uiReturn++;
			m_ullCursor++;
		}
	}

	ENSURE( l_uiReturn <= p_size);
	CHECK_INVARIANTS();
	return l_uiReturn;
}

size_t File :: ReadWord( String & p_toRead)
{
	return ReadLine( p_toRead, 1024, cuT( " \r\n;,.\t"));
}

size_t File :: ReadChar( xchar & p_toRead)
{
	CHECK_INVARIANTS();
	REQUIRE( m_iMode & eOPEN_MODE_READ);

	size_t l_uiReturn = 0;

	if (IsOk())
	{
		int l_iOrigChar;

		if (m_eEncoding == eENCODING_MODE_ASCII)
		{
			l_iOrigChar = getc( m_pFile);
			p_toRead = String( char( l_iOrigChar))[0];
		}
		else
		{
			l_iOrigChar = getwc( m_pFile);
			p_toRead = String( wchar_t( l_iOrigChar))[0];
		}

		l_uiReturn++;
	}

	return l_uiReturn;
}

size_t File :: WriteLine( String const & p_strLine)
{
	CHECK_INVARIANTS();
	REQUIRE( (m_iMode & eOPEN_MODE_WRITE) || (m_iMode & eOPEN_MODE_APPEND));

	size_t l_uiReturn = 0;

	if (IsOk())
	{
#ifdef _UNICODE
		if (m_eEncoding != eENCODING_MODE_ASCII)
		{
			l_uiReturn = WriteArray( p_strLine.c_str(), p_strLine.size());
		}
		else
		{
			l_uiReturn = WriteArray( p_strLine.char_str(), p_strLine.size());
		}
#else
		if (m_eEncoding != eENCODING_MODE_ASCII)
		{
			l_uiReturn = WriteArray( p_strLine.wchar_str(), p_strLine.size());
		}
		else
		{
			l_uiReturn = WriteArray( p_strLine.c_str(), p_strLine.size());
		}
#endif
	}

	CHECK_INVARIANTS();
	return l_uiReturn;
}

size_t File :: CopyToString( String & p_strOut)
{
	CHECK_INVARIANTS();
	REQUIRE( m_iMode & eOPEN_MODE_READ);

	size_t l_uiReturn = 0;
	p_strOut.clear();
	String l_strLine;

	while (IsOk())
	{
		l_uiReturn += ReadLine( l_strLine, 1024);

		p_strOut += l_strLine + cuT( "\n");
	}

	CHECK_INVARIANTS();
	return l_uiReturn;
}

size_t File :: Print( size_t p_uiMaxSize, xchar const * p_pFormat, ...)
{
	CHECK_INVARIANTS();
	REQUIRE( (m_iMode & eOPEN_MODE_WRITE) || (m_iMode & eOPEN_MODE_APPEND));

	size_t l_uiReturn = 0;
	xchar * l_pText = new xchar[p_uiMaxSize];
	va_list l_vaList;

	if (p_pFormat)
	{
		va_start( l_vaList, p_pFormat);
		Vsnprintf( l_pText, p_uiMaxSize, p_uiMaxSize, p_pFormat, l_vaList);
		va_end( l_vaList);
		l_uiReturn = WriteLine( l_pText);
	}

	delete [] l_pText;

	ENSURE( l_uiReturn <= p_uiMaxSize);
	CHECK_INVARIANTS();
	return l_uiReturn;
}

bool File :: Write( String const & p_strToWrite)
{
	CHECK_INVARIANTS();
	REQUIRE( (m_iMode & eOPEN_MODE_WRITE) || (m_iMode & eOPEN_MODE_APPEND));

	bool l_bReturn = Write( p_strToWrite.size()) == sizeof( size_t);

	if (l_bReturn && p_strToWrite.size() > 0)
	{
		l_bReturn = WriteArray<xchar>( p_strToWrite.c_str(), p_strToWrite.size()) ==  p_strToWrite.size() * sizeof( xchar);
	}

	CHECK_INVARIANTS();
	return l_bReturn;
}

bool File :: Read( String & p_strToRead)
{
	CHECK_INVARIANTS();
	REQUIRE( m_iMode & eOPEN_MODE_READ);

	p_strToRead.clear();
	size_t l_uiSize = 0;
	bool l_bReturn = Read( l_uiSize) == sizeof( size_t);

	if (l_bReturn && l_uiSize > 0)
	{
		xchar * l_pTmp = new xchar[l_uiSize + 1];
		memset( l_pTmp, 0, sizeof( xchar) * (l_uiSize + 1));
		l_bReturn = ReadArray<xchar>( l_pTmp, l_uiSize) == l_uiSize * sizeof( xchar);

		if (l_bReturn)
		{
			p_strToRead = l_pTmp;
		}

		delete [] l_pTmp;
	}

	CHECK_INVARIANTS();
	return l_bReturn;
}

long long File :: GetLength()
{
	CHECK_INVARIANTS();

	m_ullLength = 0;
	long long l_llPosition = FTell( m_pFile);
	FSeek( m_pFile, 0, SEEK_END);
	m_ullLength = FTell( m_pFile);
	FSeek( m_pFile, l_llPosition, SEEK_SET);

	CHECK_INVARIANTS();
	return m_ullLength;
}

bool File :: IsOk()const
{
	if (ferror( m_pFile) == 0)
	{
		if (feof( m_pFile) == 0)
		{
			return true;
		}
	}

	return false;
}

long long File :: Tell()
{
	CHECK_INVARIANTS();
	long long l_llReturn = 0;

	if (m_pFile)
	{
		l_llReturn = FTell( m_pFile);
	}

	CHECK_INVARIANTS();
	return l_llReturn;
}

size_t File :: _write( unsigned char const * p_pBuffer, size_t p_uiSize)
{
	CHECK_INVARIANTS();
	REQUIRE( IsOk() && (m_iMode | eOPEN_MODE_WRITE));
	size_t l_uiReturn = 0;
	if (IsOk())
	{
		l_uiReturn = fwrite( p_pBuffer, 1, p_uiSize, m_pFile);
		m_ullCursor += l_uiReturn;
		ENSURE( l_uiReturn <= p_uiSize);
	}

	CHECK_INVARIANTS();
	return l_uiReturn;
}

size_t File :: _read( unsigned char * p_pBuffer, size_t p_uiSize)
{
	CHECK_INVARIANTS();
	REQUIRE( IsOk() && (m_iMode | eOPEN_MODE_READ));
	size_t l_uiReturn = 0;

	if (IsOk())
	{
		l_uiReturn = fread( p_pBuffer, 1, p_uiSize, m_pFile);
		m_ullCursor += l_uiReturn;
		ENSURE( l_uiReturn <= p_uiSize);
	}

	CHECK_INVARIANTS();
	return l_uiReturn;
}

bool File :: FileExists( xchar const * p_filename)
{
	REQUIRE( p_filename);
#ifdef _WIN32
	bool l_bReturn = false;
	HANDLE	hFile = CreateFile( p_filename, FILE_READ_ATTRIBUTES, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle( hFile);
		l_bReturn = true;
	}

	return l_bReturn;
#else
	struct stat l_stat;
	return (stat( String( p_filename).char_str(), & l_stat) == 0);
#endif
}

bool File :: FileExists( String const & p_filename)
{
	REQUIRE( ! p_filename.empty());
	return FileExists( p_filename.c_str());
}

bool File :: DirectoryCreate( String const & p_filename)
{
	REQUIRE( ! p_filename.empty());

	std::string l_filename = p_filename.char_str();

	if (l_filename[l_filename.size() - 1] == '\\')
	{
		l_filename.erase( l_filename.size() - 1,1);
	}

	l_filename = l_filename.substr( 0, l_filename.find_last_of( '\\'));

	if ( ! DirectoryExists( l_filename))
	{
		if ( ! DirectoryCreate( l_filename))
		{
			return false;
		}
	}

	try
	{
		return create_directory( p_filename.c_str());
	}
	catch( std::exception &)
	{
		return false;
	}
}

bool File :: DirectoryExists( String const & p_filename)
{
	REQUIRE( ! p_filename.empty());

	try
	{
		return exists( p_filename.c_str()) && is_directory( p_filename.c_str());
	}
	catch( std::exception &)
	{
		return false;
	}
}

bool File :: FileDelete( String const & p_filename)
{
	REQUIRE( ! p_filename.empty());

	try
	{
		return (remove( p_filename.char_str()) == 0);
	}
	catch( std::exception &)
	{
		return false;
	}
}

bool File :: DirectoryDelete( String const & p_dirName)
{
	REQUIRE( ! p_dirName.empty());

	if ( ! exists( p_dirName.c_str()))
	{
		return false;
	}

	try
	{
		directory_iterator iend;
		directory_iterator i( p_dirName.char_str());
		for ( ;	i != iend ; ++ i)
		{
			if (is_directory( i->status()))
			{
				if ( ! DirectoryDelete( i->path().string()))
				{
					return false;
				}
			}
			else if ( ! (FileDelete( i->path().string())))
			{
				//ERROR : could not delete a file ?
				return false;
			}
		}
		remove( p_dirName.c_str());
		return true;
	}
	catch( ... )
	{
		//ERROR : could not delete a directory ?
		return false;
	}
}

bool File :: ListDirectoryFiles( String const & p_folderPath, StringArray & p_files, bool p_recursive)
{
	REQUIRE( ! p_folderPath.empty());

	if ( ! exists( p_folderPath.c_str()))
	{
		return false;
	}
	try
	{
		std::string l_strPath( p_folderPath.char_str());
		path l_path( l_strPath.begin(), l_strPath.end());
		directory_iterator iend;
		directory_iterator i( l_path);

		for ( ;	i != iend ; ++ i)
		{
			if (is_directory( i->status() ) && p_recursive)
			{
				if ( ! ListDirectoryFiles( i->path().string(), p_files, p_recursive))
				{
					return false;
				}
			}
			else
			{
				p_files.push_back( String( i->path().string()));
			}
		}
		return true;
	}
	catch( ... )
	{
		//ERROR : could not delete a directory ?
		return false;
	}
}

Path File :: DirectoryGetCurrent()
{
	Path l_pathReturn;
#ifdef _WIN32
	xchar l_pPath[FILENAME_MAX];
	DWORD dwResult = GetModuleFileName( NULL, l_pPath, sizeof( l_pPath));

	if (dwResult != 0)
	{
		l_pathReturn = l_pPath;
	}
#else
	char l_pPath[FILENAME_MAX];
	char l_szTmp[32];
	sprintf( l_szTmp, "/proc/%d/exe", getpid());
	int l_iBytes = Math::min( size_t( readlink( l_szTmp, l_pPath, sizeof( l_pPath))), size_t( sizeof( l_pPath) - 1));

	if (l_iBytes >= 0)
	{
		l_pPath[l_iBytes] = '\0';
		l_pathReturn = l_pPath;
	}
#endif
/*
	char l_pPath[FILENAME_MAX];
	GetCurrentDir( l_pPath, sizeof( l_pPath));
	l_pathReturn = l_pPath;
*/
	l_pathReturn = l_pathReturn.GetPath();
	return l_pathReturn;
}
