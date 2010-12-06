/*
This source file is part of Castor3D (http://dragonjoker.co.cc

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___Castor_File___
#define ___Castor_File___

#include <fstream>
#include "Module_Utils.h"
#include "Path.h"

#pragma warning( push)
#pragma warning( disable : 4996)

namespace Castor
{	namespace Utils
{
/*
		Tests on linux : 
		existence test x 100000:
		not exists:

		iostream 750 ms
		fopen 419ms
		boost 524 ms
		stat 230 ms

		exists

		iostream 1.5 sec
		fopen 900 msec
		stat 450 ms
		boost 870 msec
*/
	class File
	{
	public:
		typedef enum OpenMode
		{
			eRead	= 0x00000001,
			eWrite	= 0x00000010,
			eAdd	= 0x00000100,
			eBinary = 0x00001000,
		} OpenMode;

		typedef enum OffsetMode
		{
			eBeginning,
			eCurrent,
			eEnd
		} OffsetMode;

	private:
		int m_iMode;
		Path m_strFileFullPath;
		typedef std::basic_fstream<char, std::char_traits<char> > stream_file;
		stream_file m_file;

	public:
		File( const String & p_fileName, int p_iMode);
		virtual ~File();

		unsigned long long GetLength();
		bool IsOk();
		void Seek( long p_lOffset, OffsetMode p_eOrigin);
		void Seek( long p_lPosition);
		long Tell();
		bool ReadLine( String & p_toRead, size_t p_size);
		bool ReadWord( String & p_toRead);
		bool WriteLine( const String & p_strLine);
		void CopyToString( String & p_strOut);

		bool Print( size_t p_uiMaxSize, const char * p_pFormat, ...);
//		int Scanf( const char * p_pFormat, size_t p_uiCount, ...);

		template <typename T> File & operator <<( const T & p_toWrite)
		{
			m_file << p_toWrite;
			return *this;
		}

		template <typename T> File & operator >>( T & p_toRead)
		{
			m_file >> p_toWrite;
			return *this;
		}

		template <typename T> int Write( const T & p_toWrite)
		{
			return _write( reinterpret_cast <const char *>( & p_toWrite), sizeof( T));
/*
			size_t l_uiSize = sizeof( T);
			char * l_pBuffer = new char[l_uiSize];
			memccpy( l_pBuffer, & p_toWrite, 1, l_uiSize);
			return _write( l_pBuffer, l_uiSize);
*/
		}

		template <typename T> int Read( T & p_toRead)
		{
			return _read( reinterpret_cast <char *>( & p_toRead), sizeof( T));
/*
			size_t l_uiSize = sizeof( T);
			char * l_pBuffer = new char[l_uiSize];
			int l_iReturn = _read( l_pBuffer, l_uiSize);
			memccpy( & p_toRead, l_pBuffer, 1, l_iReturn);
			return l_iReturn;
*/
		}

		template <typename T> int WriteArray( const T * p_toWrite, size_t p_count)
		{
			return _write( reinterpret_cast <const char *>( p_toWrite), sizeof( T) * p_count);
/*
			size_t l_uiSize = sizeof( T) * p_count;
			char * l_pBuffer = new char[l_uiSize];
			memccpy( l_pBuffer, p_toWrite, 1, l_uiSize);
			return _write( l_pBuffer, l_uiSize);
*/
		}

		template <typename T> int ReadArray( T * p_toRead, size_t p_count)
		{
			return _read( reinterpret_cast <char *>( & p_toRead), sizeof( T) * p_count);
/*
			size_t l_uiSize = sizeof( T) * p_count;
			char * l_pBuffer = new char[l_uiSize];
			int l_iReturn = _read( l_pBuffer, l_uiSize);
			memccpy( & p_toRead, l_pBuffer, 1, l_iReturn);
			return l_iReturn;
*/
		}

	private:
		int _write( const char * p_pBuffer, size_t p_uiSize);
		int _read( char * p_pBuffer, size_t p_uiSize);

	public:
		static bool FileExists( const Char * p_filename);
		static bool FileExists( const String & p_filename);
		static bool DirectoryCreate( const String & p_filename);
		static bool DirectoryExists( const String & p_filename);
		static bool DirectoryDelete( const String & p_dirName);
		static bool FileDelete( const String & p_filename);
		static bool ListDirectoryFiles( const String & p_folderPath, StringArray & p_files, bool p_recursive = false);

		inline const Path &	GetFileFullPath	()const { return m_strFileFullPath; }
		inline String		GetFilePath		()const { return m_strFileFullPath.GetPath(); }
		inline String		GetFileName		()const { return m_strFileFullPath.GetLeaf(); }
	};
}
}

#pragma warning( pop)

#endif