/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

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
#include "CastorUtils.hpp"
#include "Path.hpp"
#include "Exception.hpp"
#include "Assertion.hpp"

#pragma warning( push)
#pragma warning( disable : 4996)

namespace Castor
{	namespace Utils
{
	/**
	 * Convert from little or big endian to the other.
	 *\param p_tValue : [in/out] Data to be converted.
	 */
	template <class T> inline void SwitchEndianness( T & p_tValue)
	{
		T l_tTmp;
		unsigned char * l_pToConvert = reinterpret_cast<unsigned char *>( & p_tValue);
		unsigned char * l_pConverted = reinterpret_cast<unsigned char *>( & l_tTmp);
 
		for (size_t i = 0 ; i < sizeof( T) ; ++i)
		{
			l_pConverted[i] = l_pToConvert[sizeof( T) - i - 1];
		}

		p_tValue = l_tTmp;
	}
	//! File class
	/*!
	User friendly File class. Adds some static functions to check file/directory validity, file/directory creation...
	\author Sylvain DOREMUS
	\version 0.6.1.0
	\date 03/01/2011
	*/
	class File
	{
	public:
		typedef enum
		{	eOPEN_MODE_READ		= 0x00000001
		,	eOPEN_MODE_WRITE	= 0x00000010
		,	eOPEN_MODE_APPEND	= 0x00000100
		,	eOPEN_MODE_BINARY	= 0x00001000
		,	eOPEN_MODE_COUNT
		}	eOPEN_MODE;

		typedef enum
		{	eENCODING_MODE_AUTO
		,	eENCODING_MODE_ASCII
		,	eENCODING_MODE_UNICODE
		,	eENCODING_MODE_UTF8
		,	eENCODING_MODE_UTF16
		}	eENCODING_MODE;

		typedef enum
		{	eOFFSET_MODE_BEGINNING
		,	eOFFSET_MODE_CURRENT
		,	eOFFSET_MODE_END
		}	eOFFSET_MODE;

	private:
		int m_iMode;
		eENCODING_MODE m_eEncoding;
		Path m_strFileFullPath;
		FILE * m_pFile;
		unsigned long long m_ullCursor;
		unsigned long long m_ullLength;

	public:
		/**@name Construction / Destruction */
		//@{
		File( Path const & p_fileName, int p_iMode, eENCODING_MODE p_eEncoding=eENCODING_MODE_ASCII);
		virtual ~File();
		//@}

		DECLARE_INVARIANT_BLOCK()

		/**@name Seek functions */
		//@{
		int Seek( long long p_lOffset, eOFFSET_MODE p_eOrigin);
		int Seek( long long p_lPosition);
		//@}

		/**@name OpenMode dependant read and write functions */
		//@{
		template <typename T> File & operator <<( T const & p_toWrite);
		template <typename T> File & operator >>( T & p_toRead);
		//@}

		/**@name Text read and write functions */
		//@{
		size_t ReadLine		( String & p_toRead, size_t p_size, String p_strSeparators=cuT( "\r\n"));
		size_t ReadWord		( String & p_toRead);
		size_t ReadChar		( xchar & p_toRead);
		size_t WriteLine	( String const & p_strLine);
		size_t CopyToString	( String & p_strOut);
		size_t Print		( size_t p_uiMaxSize, xchar const * p_pFormat, ...);
		//@}

		/**@name Binary read and write functions */
		//@{
		bool Write								( String const & p_strToWrite);
		bool Read								( String & p_strToRead);
		template <typename T> size_t Write		( T const & p_toWrite);
		template <typename T> size_t Read		( T & p_toRead);
		template <typename T> size_t WriteArray	( T const * p_toWrite, size_t p_count);
		template <typename T> size_t ReadArray	( T * p_toRead, size_t p_count);
		//@}

		/**@name Accessors */
		//@{
				long long			GetLength		();
				bool 				IsOk			()const;
				long long			Tell			();
		inline 	Path const &		GetFileFullPath	()const { return m_strFileFullPath; }
		inline 	Path				GetFilePath		()const { return m_strFileFullPath.GetPath(); }
		inline 	Path				GetFileName		()const { return m_strFileFullPath.GetFullFileName(); }
		//@}

	public:
		/**@name General File / Directory functions */
		//@{
		static bool FileExists			( xchar const * p_filename);
		static bool FileExists			( String const & p_filename);
		static bool DirectoryCreate		( String const & p_filename);
		static bool DirectoryExists		( String const & p_filename);
		static bool DirectoryDelete		( String const & p_dirName);
		static bool FileDelete			( String const & p_filename);
		static bool ListDirectoryFiles	( String const & p_folderPath, StringArray & p_files, bool p_recursive = false);
		static Path	DirectoryGetCurrent	();
		//@}

	private:
		size_t _write	( unsigned char const * p_pBuffer, size_t p_uiSize);
		size_t _read	( unsigned char * p_pBuffer, size_t p_uiSize);
	};

#	include "File.inl"
}
}

#pragma warning( pop)

#endif
