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
#include "Exception.h"
#include "Assertion.h"

#pragma warning( push)
#pragma warning( disable : 4996)

namespace Castor
{	namespace Utils
{
	//! File class
	/*!
	User friendly File class. Adds some functions to check file/directory validity, file/directory creation...
	\author Sylvain DOREMUS
	\version 0.6.1.0
	\date 03/01/2011
	*/
	class File
	{
	public:
		typedef enum
		{
			eRead	= 0x00000001,
			eWrite	= 0x00000010,
			eAppend	= 0x00000100,
			eBinary = 0x00001000,
		}
		eOPEN_MODE;

		typedef enum
		{
			eAuto,
			eASCII,
			eUNICODE,
			eUTF8,
			eUTF16
		}
		eENCODING_MODE;

		typedef enum
		{
			eBeginning,
			eCurrent,
			eEnd
		}
		eOFFSET_MODE;

	private:
		int m_iMode;
		eENCODING_MODE m_eEncoding;
		Path m_strFileFullPath;
		FILE * m_pFile;

	public:
		/**@name Construction / Destruction */
		//@{
		File( const Path & p_fileName, int p_iMode, eENCODING_MODE p_eEncoding=eASCII);
		virtual ~File();
		//@}

		/**@name Seek functions */
		//@{
		void Seek( long long p_lOffset, eOFFSET_MODE p_eOrigin);
		void Seek( long long p_lPosition);
		//@}

		/**@name OpenMode dependant read and write functions */
		//@{
		template <typename T> File & operator <<( const T & p_toWrite);
		template <typename T> File & operator >>( T & p_toRead);
		//@}

		/**@name Text read and write functions */
		//@{
		size_t ReadLine( String & p_toRead, size_t p_size, String p_strSeparators=CU_T( "\r\n"));
		size_t ReadWord( String & p_toRead);
		size_t WriteLine( const String & p_strLine);
		size_t CopyToString( String & p_strOut);
		size_t Print( size_t p_uiMaxSize, const xchar * p_pFormat, ...);
		//@}

		/**@name Binary read and write functions */
		//@{
		bool Write( const String & p_strToWrite);
		bool Read( String & p_strToRead);
		template <typename T> size_t Write( const T & p_toWrite);
		template <typename T> size_t Read( T & p_toRead);
		template <typename T> size_t WriteArray( const T * p_toWrite, size_t p_count);
		template <typename T> size_t ReadArray( T * p_toRead, size_t p_count);
		//@}

		/**@name Accessors */
		//@{
				long long			GetLength		();
				bool 				IsOk			()const;
				long long			Tell			();
		inline 	const Path &		GetFileFullPath	()const { return m_strFileFullPath; }
		inline 	Path				GetFilePath		()const { return m_strFileFullPath.GetPath(); }
		inline 	Path				GetFileName		()const { return m_strFileFullPath.GetFullFileName(); }
		//@}

	public:
		/**@name General File / Directory functions */
		//@{
		static bool FileExists( const xchar * p_filename);
		static bool FileExists( const String & p_filename);
		static bool DirectoryCreate( const String & p_filename);
		static bool DirectoryExists( const String & p_filename);
		static bool DirectoryDelete( const String & p_dirName);
		static bool FileDelete( const String & p_filename);
		static bool ListDirectoryFiles( const String & p_folderPath, StringArray & p_files, bool p_recursive = false);
		//@}

	private:
		size_t _write( const unsigned char * p_pBuffer, size_t p_uiSize);
		size_t _read( unsigned char * p_pBuffer, size_t p_uiSize);
	};

#	include "File.inl"
}
}

#pragma warning( pop)

#endif
