#include "File.h"

#include <sys/stat.h>
#include <boost/filesystem.hpp>
#include <sys/types.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

using namespace boost::filesystem;
using namespace General::Utils;

//******************************************************************************************************

bool FileBase :: FileExists( const Char * p_filename)
{
	struct stat l_stat;
	return (stat( String( p_filename).char_str(), & l_stat) == 0);
}

bool FileBase :: FileExists( const String & p_filename)
{
	return FileExists( p_filename.c_str());
}

bool FileBase :: DirectoryCreate( const String & p_filename)
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

bool FileBase :: DirectoryExists( const String & p_filename)
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

bool FileBase :: FileDelete( const String & p_filename)
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

bool FileBase :: DirectoryDelete( const String & p_dirName)
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

bool FileBase :: ListDirectoryFiles( const String & p_folderPath, StringArray & p_files, bool p_recursive)
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

void FileBase :: CopyToString( String & p_strOut)
{
	p_strOut.clear();
	String l_strLine;

	while (IsOk())
	{
		ReadLine( l_strLine, 1024);

		p_strOut += l_strLine + C3D_T( "\n" );
	}
}

//******************************************************************************************************

FileStream :: FileStream( const String & p_fileName, OpenMode p_mode)
	:	FileBase( p_fileName, p_mode)
{
	FILE * l_pFile;
	fopen_s( & l_pFile, p_fileName.char_str(), "a");

	if (l_pFile != NULL)
	{
		fclose( l_pFile);
	}

	m_file = new std::fstream( p_fileName.c_str(), std::fstream::out | std::fstream::in);
}

FileStream :: ~FileStream()
{
	m_file->close();
	delete m_file;
}

//******************************************************************************************************

File :: File( const String & p_fileName, OpenMode p_mode)
	:	FileBase( p_fileName, p_mode)
{
	if (fopen_s( & m_file, p_fileName.char_str(), p_mode == eRead ? "rb" : "wb") != 0)
	{
		m_file = NULL;
	}
}

File :: ~File()
{
	if (m_file != NULL)
	{
		fclose( m_file);
	}
}

//******************************************************************************************************




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
*/
