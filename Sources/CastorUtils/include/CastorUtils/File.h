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
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___GENERAL_FILE_H___
#define ___GENERAL_FILE_H___

#include "Module_Utils.h"

#include <fstream>

namespace General
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

	class FileBase
	{
	public:
		typedef enum OpenMode
		{
			eRead,
			eWrite,
			eAdd
		} OpenMode;

		typedef enum OffsetMode
		{
			eBeginning,
			eCurrent,
			eEnd
		} OffsetMode;

	protected:
		OpenMode m_eMode;
		String m_strFileFullPath;
		String m_strFilePath;
		String m_strFileName;

	public:
		FileBase( const String & p_strFileName, OpenMode p_eMode)
			:	m_eMode( p_eMode),
				m_strFileFullPath( p_strFileName)
		{
			m_strFileFullPath.Replace( "\\", "/");
			size_t l_uiIndex = m_strFileFullPath.find_last_of( '/');
			m_strFilePath = m_strFileFullPath.substr( 0, l_uiIndex);
			m_strFileName = m_strFileFullPath.substr( l_uiIndex + 1, String::npos);
		}

		virtual ~FileBase(){}

		virtual bool IsOk()=0;
		virtual void CopyToString( String & p_strOut);
		virtual void Seek( long p_lOffset, OffsetMode p_eOrigin)=0;
		void Seek( long p_lPosition)
		{
			Seek( p_lPosition, eBeginning);
		}
		virtual long Tell()=0;
		virtual bool ReadLine( String & p_toRead, size_t p_size)=0;
		virtual bool ReadWord( String & p_toRead)=0;
		virtual bool WriteLine( const String & p_strLine)=0;
		virtual bool Print( size_t p_uiMaxSize, const char * p_pFormat, ...)=0;

//		virtual int Scanf( const char * p_pFormat, size_t p_uiCount, ...)=0;

	public:
		static bool FileExists( const Char * p_filename);
		static bool FileExists( const String & p_filename);
		static bool DirectoryCreate( const String & p_filename);
		static bool DirectoryExists( const String & p_filename);
		static bool DirectoryDelete( const String & p_dirName);
		static bool FileDelete( const String & p_filename);
		static bool ListDirectoryFiles( const String & p_folderPath, StringArray & p_files, bool p_recursive = false);

		inline const String & GetFileFullPath()const { return m_strFileFullPath; }
		inline const String & GetFilePath()const { return m_strFilePath; }
		inline const String & GetFileName()const { return m_strFileName; }
	};

	class FileIO : public FileBase
	{
	private:
		FILE * m_file;

	public:
		FileIO( const String & p_fileName, OpenMode p_mode);
		virtual ~FileIO();

		template <typename T>
		int Write( const T & p_toWrite)
		{
			int l_iReturn = 0;

			if (m_eMode == eWrite && IsOk())
			{
				l_iReturn = fwrite( & p_toWrite, sizeof( T), 1, m_file) * sizeof( T);
			}

			return l_iReturn;
		}

		template <typename T>
		int Read( T & p_toRead)
		{
			int l_iReturn = 0;

			if (m_eMode == eRead && IsOk())
			{
				size_t l_uiSize = sizeof( T);
				l_iReturn = fread( & p_toRead, sizeof( T), 1, m_file) * sizeof( T);
			}

			return l_iReturn;
		}

		template <typename T>
		int WriteArray( const T * p_toWrite, size_t p_count)
		{
			int l_iReturn = 0;

			if (m_eMode == eWrite && IsOk())
			{
				l_iReturn = fwrite( p_toWrite, sizeof( T), p_count, m_file) * sizeof( T);
			}

			return l_iReturn;
		}

		template <typename T>
		int ReadArray( T * p_toRead, size_t p_count)
		{
			int l_iReturn = 0;

			if (m_eMode == eRead && IsOk())
			{
				l_iReturn = fread( p_toRead, sizeof( T), p_count, m_file) * sizeof( T);
			}

			return l_iReturn;
		}

		template <typename T>
		FileIO & operator << ( const T & p_toWrite)
		{
			String l_strLine;
			l_strLine << p_toWrite;
			WriteArray<char>( l_strLine.char_str(), l_strLine.size());
			return *this;
		}

		template <typename T>
		FileIO & operator >> ( T & p_toRead)
		{
			size_t l_uiSize = sizeof( T);
			char l_pBuffer[l_uiSize];
			ReadArray<char>( l_pBuffer, l_uiSize);
			memccpy( & p_toRead, l_pBuffer, 1, sizeof( T));
			return * this;
		}

		virtual bool IsOk()
		{
			return (m_file != NULL) && (ferror( m_file) == 0) && (feof( m_file) == 0);
		}

		virtual void Seek( long p_lOffset, OffsetMode p_eOrigin=eBeginning)
		{
			if (IsOk())
			{
				switch (p_eOrigin)
				{
				case eBeginning:
					fseek( m_file, p_lOffset, SEEK_SET);
					break;

				case eCurrent:
					fseek( m_file, p_lOffset, SEEK_CUR);
					break;

				case eEnd:
					fseek( m_file, p_lOffset, SEEK_END);
					break;
				}
			}
		}

		virtual long Tell()
		{
			if (IsOk())
			{
				return ftell( m_file);
			}

			return 0;
		}

		virtual bool ReadLine( String & p_toRead, size_t p_size)
		{
			if (m_eMode == eRead && IsOk())
			{
				char * l_line = new char[p_size];
				fgets( l_line, p_size, m_file);

				if (strnlen( l_line, p_size) > 0)
				{
					p_toRead = String( l_line);

					if (p_toRead.find( String( (Char)10)) != String::npos)
					{
						p_toRead = p_toRead.substr( 0, p_toRead.find( String( (Char)10)));
					}

					if (p_toRead.find( String( (Char)13)) != String::npos)
					{
						p_toRead = p_toRead.substr( 0, p_toRead.find( String( (Char)13)));
					}
				}
				else
				{
					p_toRead.clear();
				}

				delete l_line;
				return IsOk();
			}

			return false;
		}

		virtual bool ReadWord( String & p_toRead)
		{
			if (m_eMode == eRead && IsOk())
			{
				p_toRead.clear();
				char l_cChar = ' ';

				while ( ! feof( m_file) && (l_cChar == ' ' || l_cChar == '\n' || l_cChar == '\r' || l_cChar == '\t'))
				{
					l_cChar = getc( m_file);
				}

				while ( ! feof( m_file) && l_cChar != ' ' && l_cChar != '\n' && l_cChar != '\r' && l_cChar != '\t')
				{
					p_toRead += l_cChar;
					l_cChar = getc( m_file);
				}

				if ( ! feof( m_file))
				{
					ungetc( l_cChar, m_file);
				}
			}

			return false;
		}

		virtual bool WriteLine( const String & p_strLine)
		{
			bool l_bReturn = false;

			if (m_eMode == eWrite && IsOk())
			{
				l_bReturn = WriteArray<char>( p_strLine.char_str(), p_strLine.size()) == p_strLine.size();
			}

			return l_bReturn;
		}

		virtual bool Print( size_t p_uiMaxSize, const char * p_pFormat, ...);

//		inline int Scanf( const char * p_pFormat, size_t p_uiCount, ...);
	};

	class FileStream : public FileBase
	{
	private:
		std::fstream * m_file;

	public:
		FileStream( const String & p_fileName, OpenMode p_mode);
		virtual ~FileStream();

		template <typename T>
		int Write( const T & p_toWrite)
		{
			int l_iReturn = 0;

			if (m_mode == eWrite && IsOk())
			{
				const char * l_pBuffer = reinterpret_cast<const char *>( & p_toWrite);
				size_t l_uiSize = sizeof( T);

				for (size_t i = 0 ; i < l_uiSize && ! m_file->fail() ; i++)
				{
					m_file->write( l_pBuffer + i, 1);
					l_iReturn++;
				}

				if (m_file->fail() && l_iReturn > 0)
				{
					l_iReturn--;
				}
			}

			return l_iReturn;
		}

		template <typename T>
		int Read( T & p_toRead)
		{
			int l_iReturn = 0;

			if (m_mode == eRead && IsOk())
			{
				char * l_pBuffer = reinterpret_cast<char *>( & p_toRead);
				size_t l_uiSize = sizeof( T);

				for (size_t i = 0 ; i < l_uiSize && ! m_file->fail() ; i++)
				{
					m_file->read( l_pBuffer + i, 1);
					l_iReturn++;
				}

				if (m_file->fail() && l_iReturn > 0)
				{
					l_iReturn--;
				}
			}
			return l_iReturn;
		}

		template <typename T>
		int WriteArray( const T * p_toWrite, size_t p_count)
		{
			int l_iReturn = 0;

			if (m_eMode == eWrite && IsOk())
			{
				const char * l_pBuffer = reinterpret_cast<const char *>( p_toWrite);
				size_t l_uiSize = sizeof( T) * p_count;

				for (size_t i = 0 ; i < l_uiSize && ! m_file->fail() ; i++)
				{
					m_file->write( l_pBuffer + i, 1);
					l_iReturn++;
				}

				if (m_file->fail() && l_iReturn > 0)
				{
					l_iReturn--;
				}
			}
			return l_iReturn;
		}

		template <typename T>
		 int ReadArray( T * p_toRead, size_t p_count)
		{
			int l_iReturn = 0;

			if (m_eMode == eRead && IsOk())
			{
				char * l_pBuffer = reinterpret_cast<char *>( p_toRead);
				size_t l_uiSize = sizeof( T) * p_count;

				for (size_t i = 0 ; i < l_uiSize && ! m_file->fail() ; i++)
				{
					m_file->read( l_pBuffer + i, 1);
					l_iReturn++;
				}

				if (m_file->fail() && l_iReturn > 0)
				{
					l_iReturn--;
				}
			}
			return l_iReturn;
		}

		template <typename T>
		FileStream & operator << ( const T & p_toWrite)
		{
			*m_file << p_toWrite;
			return *this;
		}

		template <typename T>
		FileStream & operator >> ( T & p_toRead)
		{
			*m_file >> p_toRead;
			return * this;
		}

		virtual bool IsOk()
		{
			return ! m_file->fail();
		}

		virtual void Seek( long p_lOffset, OffsetMode p_eOrigin=eBeginning)
		{
			if (m_eMode == eRead && IsOk())
			{
				switch (p_eOrigin)
				{
				case eBeginning:
					m_file->seekg( p_lOffset, std::ios_base::beg);
					break;

				case eCurrent:
					m_file->seekg( p_lOffset, std::ios_base::cur);
					break;

				case eEnd:
					m_file->seekg( p_lOffset, std::ios_base::end);
					break;
				}
			}
			else
			{
				switch (p_eOrigin)
				{
				case eBeginning:
					m_file->seekp( p_lOffset, std::ios_base::beg);
					break;

				case eCurrent:
					m_file->seekp( p_lOffset, std::ios_base::cur);
					break;

				case eEnd:
					m_file->seekp( p_lOffset, std::ios_base::end);
					break;
				}
			}
		}

		virtual long Tell()
		{
			if (m_eMode == eRead && IsOk())
			{
				return m_file->tellg();
			}
			else
			{
				return m_file->tellp();
			}
		}

		virtual bool ReadLine( String & p_toRead, size_t p_size)
		{
			if (m_eMode == eRead && IsOk())
			{
				char * l_line = new char[p_size];
				m_file->getline( l_line, p_size, '\n');

				if (strnlen( l_line, p_size) > 0)
				{
					p_toRead = String( l_line);
				}
				else
				{
					p_toRead.clear();
				}

				delete l_line;
				return ! m_file->fail();
			}

			return false;
		}

		virtual bool ReadWord( String & p_toRead)
		{
			if (m_eMode == eRead && IsOk())
			{
				char l_word[256];
				*m_file >> l_word;
				p_toRead = String( l_word);
				return ! m_file->fail();
			}

			return false;
		}

		virtual bool WriteLine( const String & p_strLine)
		{
			bool l_bReturn = false;

			if (m_eMode == eWrite && IsOk())
			{
				l_bReturn = WriteArray<char>( p_strLine.char_str(), p_strLine.size()) == p_strLine.size();
			}

			return l_bReturn;
		}

		virtual bool Print( size_t p_uiMaxSize, const char * p_pFormat, ...);

//		virtual int Scanf( const char * p_pFormat, size_t p_uiCount, ...);
	};
}
}

#endif