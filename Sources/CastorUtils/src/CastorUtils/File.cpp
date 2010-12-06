#include "PrecompiledHeader.h"

#include "File.h"

#include <sys/stat.h>
#include <boost/filesystem.hpp>
#include <sys/types.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#if CHECK_MEMORYLEAKS
#	include "Memory.h"
using namespace Castor::Utils;
#endif

using namespace boost::filesystem;
using namespace Castor;
using namespace Castor::Utils;

//******************************************************************************************************

File :: File( const String & p_strFileName, int p_iMode)
	:	m_iMode( p_iMode),
		m_strFileFullPath( p_strFileName)
{
	if ( ! FileExists( m_strFileFullPath))
	{
		FILE * l_pFile;
		fopen_s( & l_pFile, m_strFileFullPath.char_str(), "a+");

		if (l_pFile != NULL)
		{
			fclose( l_pFile);
		}
	}

	int l_iBitMask = std::ios_base::in | std::ios_base::out;

	if (p_iMode & eAdd)
	{
		l_iBitMask = l_iBitMask | std::ios_base::app;
	}

	if (p_iMode & eBinary)
	{
		l_iBitMask = l_iBitMask | std::ios_base::binary;
	}

	m_file.open( p_strFileName.c_str(), l_iBitMask);
}

File :: ~File()
{
	m_file.close();
}

unsigned long long File :: GetLength()
{
	unsigned long long l_ullReturn = 0;

	if (IsOk())
	{
		long l_lPosition = Tell();
		Seek( 0);
		Seek( 0, eEnd);
		l_ullReturn = Tell();
		Seek( l_lPosition);
	}

	return l_ullReturn;
}

bool File :: IsOk()
{
	return ! m_file.fail();
}

void File :: Seek( long p_lOffset, OffsetMode p_eOrigin)
{
	if ((m_iMode & eRead) && IsOk())
	{
		switch (p_eOrigin)
		{
		case eBeginning:
			m_file.seekg( p_lOffset, std::ios_base::beg);
			break;

		case eCurrent:
			m_file.seekg( p_lOffset, std::ios_base::cur);
			break;

		case eEnd:
			m_file.seekg( p_lOffset, std::ios_base::end);
			break;
		}
	}
	else
	{
		switch (p_eOrigin)
		{
		case eBeginning:
			m_file.seekp( p_lOffset, std::ios_base::beg);
			break;

		case eCurrent:
			m_file.seekp( p_lOffset, std::ios_base::cur);
			break;

		case eEnd:
			m_file.seekp( p_lOffset, std::ios_base::end);
			break;
		}
	}
}

void File :: Seek( long p_lPosition)
{
	Seek( p_lPosition, eBeginning);
}

long File :: Tell()
{
	if ((m_iMode & eRead) && IsOk())
	{
		return m_file.tellg();
	}
	else
	{
		return m_file.tellp();
	}
}

bool File :: ReadLine( String & p_toRead, size_t p_size)
{
	if ((m_iMode & eRead) && IsOk())
	{
		char * l_line = new char[p_size];
		String l_strLine;
		m_file.getline( l_line, p_size, '\n');
		l_strLine = l_line;

		if (l_strLine.size() > 0)
		{
			p_toRead = l_strLine;
		}
		else
		{
			p_toRead.clear();
		}

		delete [] l_line;
		return IsOk();
	}

	return false;
}

bool File :: ReadWord( String & p_toRead)
{
	if ((m_iMode & eRead) && IsOk())
	{
		char l_word[256];
		m_file >> l_word;
		p_toRead = String( l_word);
		return ! m_file.fail();
	}

	return false;
}

bool File :: WriteLine( const String & p_strLine)
{
	bool l_bReturn = false;

	if (((m_iMode & eWrite) || (m_iMode & eAdd)) && IsOk())
	{
		l_bReturn = WriteArray( p_strLine.char_str(), p_strLine.size()) == p_strLine.size();
	}

	return l_bReturn;
}

void File :: CopyToString( String & p_strOut)
{
	p_strOut.clear();
	String l_strLine;

	while (IsOk())
	{
		ReadLine( l_strLine, 1024);

		p_strOut += l_strLine + CU_T( "\n");
	}
}

bool File :: Print( size_t p_uiMaxSize, const char * p_pFormat, ...)
{
	bool l_bReturn = false;
	char * l_pText = new char[p_uiMaxSize];
	va_list l_vaList;

	if (p_pFormat != NULL)
	{
		va_start( l_vaList, p_pFormat);	
#ifdef WIN32
        vsnprintf_s( l_pText, p_uiMaxSize, p_uiMaxSize, p_pFormat, l_vaList);  
#else
        vsnprintf( l_pText, 256, cFormat_p, ap_l);  
#endif
		va_end( l_vaList);

		WriteArray<char>( l_pText, strnlen( l_pText, p_uiMaxSize));
		l_bReturn = true;
	}

	delete l_pText;

	return l_bReturn;
}
/*
#ifdef _WIN32
int __cdecl _input_l(_Inout_ FILE * _File, _In_z_ __format_string const unsigned char *, _In_opt_ _locale_t _Locale, va_list _ArgList);
int __cdecl _winput_l(_Inout_ FILE * _File, _In_z_ __format_string const wchar_t *, _In_opt_ _locale_t _Locale, va_list _ArgList);

#ifdef _UNICODE
	typedef wchar_t tchar;
#	define _tinput_l _winput_l
#else
	typedef unsigned char tchar;
#	define _tinput_l _input_l
#endif

typedef int (*INPUTFN)(FILE *, const tchar *, _locale_t, va_list);

int vscan_fn( const Char * string, size_t count, const tchar * format, _locale_t plocinfo, va_list arglist)
{
        FILE str;
        FILE * infile = & str;
        int retval = 0;

		if (string != NULL && format != NULL)
		{
			infile->_flag = _IOREAD|_IOSTRG|_IOMYBUF;
			infile->_ptr = infile->_base = (char *) string;

			if(count>(INT_MAX/sizeof(Char)))
			{
				// old-style functions allow any large value to mean unbounded
				infile->_cnt = INT_MAX;
			}
			else
			{
				infile->_cnt = (int)count*sizeof(Char);
			}

			retval = _tinput_l( infile, format, plocinfo, arglist);
		}

		return retval;
}
#endif

int File :: Scanf( const char * p_pFormat, size_t p_uiCount, ...)
{
	va_list l_vaList;
	va_start( l_vaList, p_pFormat);

	String l_strLine;
	ReadLine( l_strLine, p_uiCount);

#ifdef _WIN32
	return vscan_fn( l_strLine.c_str(), p_uiCount, (tchar *)p_pFormat, NULL, l_vaList);
#else
	return vscanf( l_strLine.c_str(), p_uiCount, (tchar *)p_pFormat, NULL, l_vaList);
#endif
}
/**/
int File :: _write( const char * p_pBuffer, size_t p_uiSize)
{
	int l_iReturn = 0;

	if (((m_iMode & eWrite) || (m_iMode & eAdd)) && IsOk())
	{
		for (size_t i = 0 ; i < p_uiSize && IsOk() ; i++)
		{
			m_file.write( p_pBuffer + i, 1);
			l_iReturn++;
		}

		if (m_file.fail() && l_iReturn > 0)
		{
			l_iReturn--;
		}
	}

	return l_iReturn;
}

int File :: _read( char * p_pBuffer, size_t p_uiSize)
{
	int l_iReturn = 0;

	if ((m_iMode & eRead) && IsOk())
	{
		for (size_t i = 0 ; i < p_uiSize && IsOk() ; i++)
		{
			m_file.read( p_pBuffer + i, 1);
			l_iReturn++;
		}

		if (m_file.fail() && l_iReturn > 0)
		{
			l_iReturn--;
		}
	}

	return l_iReturn;
}

bool File :: FileExists( const Char * p_filename)
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