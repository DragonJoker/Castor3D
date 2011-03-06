#include "CastorUtils/PrecompiledHeader.h"

#include "CastorUtils/File.h"

#if CHECK_MEMORYLEAKS
#	include "CastorUtils/Memory.h"
using namespace Castor::Utils;
#endif

using namespace boost::filesystem;
using namespace Castor;
using namespace Castor::Utils;

//******************************************************************************************************

File :: File( const Path & p_strFileName, int p_iMode, eENCODING_MODE p_eEncoding)
	:	m_iMode( p_iMode)
	,	m_eEncoding( p_eEncoding)
	,	m_strFileFullPath( p_strFileName)
{
	String l_strMode;

	switch (p_iMode)
	{
	case eRead:
		l_strMode = CU_T( "r");
		break;

	case eWrite:
		l_strMode = CU_T( "w");
		break;

	case eAppend:
		l_strMode = CU_T( "a");
		break;

	case eRead | eBinary:
		l_strMode = CU_T( "rb");
		break;

	case eWrite | eBinary:
		l_strMode = CU_T( "wb");
		break;

	case eAppend | eBinary:
		l_strMode = CU_T( "ab");
		break;

	case eRead | eWrite:
		l_strMode = CU_T( "r+");
		break;

	case eRead | eWrite | eAppend:
		l_strMode = CU_T( "a+");
		break;
	}

	switch (p_eEncoding)
	{
	case eAuto:
#ifdef _UNICODE
		l_strMode += CU_T( ", ccs=UTF-8");
#endif
		break;

	case eUNICODE:
		l_strMode += CU_T( ", ccs=ANSI");
		break;

	case eUTF8:
		l_strMode += CU_T( ", ccs=UTF-8");
		break;

	case eUTF16:
		l_strMode += CU_T( ", ccs=UTF-16LE");
	}

	FOpen64( m_pFile, m_strFileFullPath.c_str(), l_strMode.c_str());

	if (m_pFile == NULL)
	{
		CASTOR_EXCEPTION( CU_T( "Impossible de charger le fichier ") + p_strFileName);
	}
}

File :: ~File()
{
	if (m_pFile != NULL)
	{
		fclose( m_pFile);
	}
}

void File :: Seek( long long p_llOffset, eOFFSET_MODE p_eOrigin)
{
	if (m_pFile != NULL)
	{
		switch (p_eOrigin)
		{
		case eBeginning:
			FSeek( m_pFile, p_llOffset, SEEK_SET);
			break;

		case eCurrent:
			FSeek( m_pFile, p_llOffset, SEEK_CUR);
			break;

		case eEnd:
			FSeek( m_pFile, p_llOffset, SEEK_END);
			break;
		}
	}
}

void File :: Seek( long long p_llPosition)
{
	Seek( p_llPosition, eBeginning);
}

size_t File :: ReadLine( String & p_toRead, size_t p_size, String p_strSeparators)
{
	size_t l_uiReturn = 0;
	p_toRead.clear();

	if ((m_iMode & eRead) && IsOk())
	{
		xchar l_cChar;
		String l_strLine;
		bool l_bContinue = true;

		while (l_bContinue && l_uiReturn < p_size)
		{
			if (m_eEncoding == eASCII)
			{
				l_cChar = String( char( getc( m_pFile)))[0];
			}
			else
			{
				l_cChar = String( wchar_t( getwc( m_pFile)))[0];
			}

			l_bContinue =  ! feof( m_pFile) && p_strSeparators.find( l_cChar) == String::npos;

			if (l_bContinue)
			{
				p_toRead += l_cChar;
			}

			l_uiReturn++;
		}
	}

	return l_uiReturn;
}

size_t File :: ReadWord( String & p_toRead)
{
	return ReadLine( p_toRead, 1024, CU_T( " "));
}

size_t File :: WriteLine( const String & p_strLine)
{
	size_t l_uiReturn = 0;

	if (((m_iMode & eWrite) || (m_iMode & eAppend)) && IsOk())
	{
		l_uiReturn = WriteArray( p_strLine.c_str(), p_strLine.size());
	}

	return l_uiReturn;
}

size_t File :: CopyToString( String & p_strOut)
{
	size_t l_uiReturn = 0;
	p_strOut.clear();
	String l_strLine;

	while (IsOk())
	{
		l_uiReturn += ReadLine( l_strLine, 1024);

		p_strOut += l_strLine + CU_T( "\n");
	}

	return l_uiReturn;
}

size_t File :: Print( size_t p_uiMaxSize, const xchar * p_pFormat, ...)
{
	size_t l_uiReturn = 0;
	xchar * l_pText = new xchar[p_uiMaxSize];
	va_list l_vaList;

	if (p_pFormat != NULL)
	{
		va_start( l_vaList, p_pFormat);
		Vsnprintf( l_pText, p_uiMaxSize, p_uiMaxSize, p_pFormat, l_vaList);
		va_end( l_vaList);

		l_uiReturn = WriteArray<xchar>( l_pText, strlen( l_pText));
	}

	delete [] l_pText;

	return l_uiReturn;
}

bool File :: Write( const String & p_strToWrite)
{
	bool l_bReturn = Write( p_strToWrite.size()) == sizeof( size_t);

	if (l_bReturn && p_strToWrite.size() > 0)
	{
		l_bReturn = WriteArray<xchar>( p_strToWrite.c_str(), p_strToWrite.size()) ==  p_strToWrite.size() * sizeof( xchar);
	}

	return l_bReturn;
}

bool File :: Read( String & p_strToRead)
{
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

	return l_bReturn;
}

long long File :: GetLength()
{
	long long l_llReturn = 0;
	long long l_llPosition = Tell();
	Seek( 0, eEnd);
	l_llReturn = Tell();
	Seek( l_llPosition);
	return l_llReturn;
}

bool File :: IsOk()const
{
	return (m_pFile != NULL) && (ferror( m_pFile) == 0) && (feof( m_pFile) == 0);
}

long long File :: Tell()
{
	long long l_llReturn = 0;

	if (m_pFile != NULL)
	{
		l_llReturn = FTell( m_pFile);
	}

	return l_llReturn;
}

size_t File :: _write( const unsigned char * p_pBuffer, size_t p_uiSize)
{
	CASTOR_ASSERT( IsOk());
	return fwrite( p_pBuffer, sizeof( unsigned char), p_uiSize, m_pFile);
}

size_t File :: _read( unsigned char * p_pBuffer, size_t p_uiSize)
{
	CASTOR_ASSERT( IsOk());
	return fread( p_pBuffer, sizeof( unsigned char), p_uiSize, m_pFile);
}

bool File :: FileExists( const xchar * p_filename)
{
	struct stat l_stat;
	return (stat( String( p_filename).char_str(), & l_stat) == 0);
}

bool File :: FileExists( const String & p_filename)
{
	return FileExists( p_filename.c_str());
}

bool File :: DirectoryCreate( const String & p_filename)
{
	if (p_filename.empty())
	{
		return false;
	}

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
		return create_directory( p_filename);
	}
	catch( std::exception &)
	{
		return false;
	}
}

bool File :: DirectoryExists( const String & p_filename)
{
	try
	{
		return exists( p_filename) && is_directory( p_filename);
	}
	catch( std::exception &)
	{
		return false;
	}
}

bool File :: FileDelete( const String & p_filename)
{
	try
	{
		return (remove( p_filename.char_str()) == 0);
	}
	catch( std::exception &)
	{
		return false;
	}
}

bool File :: DirectoryDelete( const String & p_dirName)
{
	if ( ! exists( p_dirName))
	{
		return false;
	}

	try
	{
		directory_iterator iend;
		directory_iterator i( p_dirName.char_str());
		for ( ;	i != iend ; ++ i)
		{
			if (is_directory( i->status() ))
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
		remove( p_dirName);
		return true;
	}
	catch( ... )
	{
		//ERROR : could not delete a directory ?
		return false;
	}
}

bool File :: ListDirectoryFiles( const String & p_folderPath, StringArray & p_files, bool p_recursive)
{
	if ( ! exists( p_folderPath))
	{
		return false;
	}
	try
	{
		directory_iterator iend;
		directory_iterator i( p_folderPath.char_str());
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
