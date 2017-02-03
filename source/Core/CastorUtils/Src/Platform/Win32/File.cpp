#include "Config/PlatformConfig.hpp"

#if defined( CASTOR_PLATFORM_WINDOWS )

#include "Data/File.hpp"

#include "Math/Math.hpp"
#include "Miscellaneous/Utils.hpp"
#include "Log/Logger.hpp"
#include <cstdio>
#include <cstring>
#include <sys/types.h>
#include <sys/stat.h>
#include <iostream>

#include <direct.h>
#include <Shlobj.h>
#include <windows.h>
#define GetCurrentDir _getcwd
#undef CopyFile
#undef DeleteFile

namespace Castor
{
	namespace
	{
		template< typename DirectoryFuncType, typename FileFuncType >
		bool TraverseDirectory( Path const & p_folderPath, DirectoryFuncType p_directoryFunction, FileFuncType p_fileFunction )
		{
			REQUIRE( !p_folderPath.empty() );
			bool l_return = false;
			WIN32_FIND_DATA l_findData;
			HANDLE l_handle = ::FindFirstFile( ( p_folderPath / cuT( "*.*" ) ).c_str(), &l_findData );

			if ( l_handle != INVALID_HANDLE_VALUE )
			{
				l_return = true;
				String l_name = l_findData.cFileName;

				if ( l_name != cuT( "." ) && l_name != cuT( ".." ) )
				{
					if ( ( l_findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) == FILE_ATTRIBUTE_DIRECTORY )
					{
						l_return = p_directoryFunction( p_folderPath / l_name );
					}
					else
					{
						p_fileFunction( p_folderPath / l_name );
					}
				}

				while ( l_return && ::FindNextFile( l_handle, &l_findData ) == TRUE )
				{
					if ( l_findData.cFileName != l_name )
					{
						l_name = l_findData.cFileName;

						if ( l_name != cuT( "." ) && l_name != cuT( ".." ) )
						{
							if ( ( l_findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) == FILE_ATTRIBUTE_DIRECTORY )
							{
								l_return = p_directoryFunction( p_folderPath / l_name );
							}
							else
							{
								p_fileFunction( p_folderPath / l_name );
							}
						}
					}
				}

				::FindClose( l_handle );
			}

			return l_return;
		}

		bool DeleteEmptyDirectory( Path const & p_path )
		{
#if defined( CASTOR_COMPILER_MSVC )

			bool l_return = _trmdir( p_path.c_str() ) == 0;

#else

			bool l_return = rmdir( string::string_cast< char >( p_path ).c_str() ) == 0;

#endif

			if ( !l_return )
			{
				auto l_error = errno;

				switch ( l_error )
				{
				case ENOTEMPTY:
					Logger::LogError( StringStream() << cuT( "Couldn't remove directory [" ) << p_path << cuT( "], it is not empty." ) );
					break;

				case ENOENT:
					Logger::LogError( StringStream() << cuT( "Couldn't remove directory [" ) << p_path << cuT( "], the path is invalid." ) );
					break;

				case EACCES:
					Logger::LogError( StringStream() << cuT( "Couldn't remove directory [" ) << p_path << cuT( "], a program has an open handle to this directory." ) );
					break;

				default:
					Logger::LogError( StringStream() << cuT( "Couldn't remove directory [" ) << p_path << cuT( "], unknown error (" ) << l_error << cuT( ")." ) );
					break;
				}
			}

			return l_return;
		}

	}

#if defined( CASTOR_COMPILER_MSVC )

	bool FOpen( FILE *& p_file, char const * p_path, char const * p_mode )
	{
		errno_t l_err = fopen_s( &p_file, p_path, p_mode );

		if ( l_err && !p_file )
		{
			Logger::LogError( StringStream() << cuT( "Couldn't open file [" ) << p_path << cuT( "], due to error: " ) << System::GetLastErrorText() );
		}

		return l_err == 0;
	}

	bool FOpen64( FILE *& p_file, char const * p_path, char const * p_mode )
	{
		errno_t l_err = fopen_s( &p_file, p_path, p_mode );

		if ( l_err && !p_file )
		{
			Logger::LogError( StringStream() << cuT( "Couldn't open file [" ) << p_path << cuT( "], due to error: " ) << System::GetLastErrorText() );
		}

		return l_err == 0;
	}

	bool FSeek( FILE * p_file, int64_t p_offset, int p_iOrigin )
	{
		return _fseeki64( p_file, p_offset, p_iOrigin ) == 0;
	}

	int64_t FTell( FILE * p_file )
	{
		return _ftelli64( p_file );
	}

#else

	bool FOpen( FILE *& p_file, char const * p_path, char const * p_mode )
	{
		p_file = fopen( p_path, p_mode );
		return p_file != nullptr;
	}

	bool FOpen64( FILE *& p_file, char const * p_path, char const * p_mode )
	{
		p_file = fopen( p_path, p_mode );
		return p_file != nullptr;
	}

	bool FSeek( FILE * p_file, int64_t p_offset, int p_iOrigin )
	{
		return fseek( p_file, p_offset, p_iOrigin ) == 0;
	}

	int64_t FTell( FILE * p_file )
	{
		return ftell( p_file );
	}

#endif

	Path File::GetExecutableDirectory()
	{
		Path l_pathReturn;
		xchar l_path[FILENAME_MAX];
		DWORD l_result = GetModuleFileName( nullptr, l_path, _countof( l_path ) );

		if ( l_result != 0 )
		{
			l_pathReturn = Path{ l_path };
		}

		l_pathReturn = l_pathReturn.GetPath();
		return l_pathReturn;
	}

	Path File::GetUserDirectory()
	{
		Path l_pathReturn;
		xchar l_path[FILENAME_MAX];
		HRESULT l_hr = SHGetFolderPath( nullptr, CSIDL_PROFILE, nullptr, 0, l_path );

		if ( SUCCEEDED( l_hr ) )
		{
			l_pathReturn = Path{ l_path };
		}

		return l_pathReturn;
	}

	bool File::DirectoryExists( Path const & p_path )
	{
		struct _stat status = { 0 };
		_stat( string::string_cast< char >( p_path ).c_str(), &status );
		return ( status.st_mode & S_IFDIR ) == S_IFDIR;
	}

	bool File::DirectoryCreate( Path const & p_path, FlagCombination< CreateMode > const & p_flags )
	{
		Path l_path = p_path.GetPath();

		if ( !l_path.empty() && !DirectoryExists( l_path ) )
		{
			DirectoryCreate( l_path, p_flags );
		}

#if defined( CASTOR_COMPILER_MSVC )

		return _tmkdir( p_path.c_str() ) == 0;

#else

		return mkdir( string::string_cast< char >( p_path ).c_str() ) == 0;

#endif
	}

	bool File::ListDirectoryFiles( Path const & p_folderPath, PathArray & p_files, bool p_recursive )
	{
		struct FileFunction
		{
			FileFunction( PathArray & p_files )
				: m_files( p_files )
			{
			}
			void operator()( Path const & p_path )
			{
				m_files.push_back( p_path );
			}
			PathArray & m_files;
		};

		if ( p_recursive )
		{
			struct DirectoryFunction
			{
				DirectoryFunction( PathArray & p_files )
					: m_files( p_files )
				{
				}
				bool operator()( Path const & p_path )
				{
					return TraverseDirectory( p_path, DirectoryFunction( m_files ), FileFunction( m_files ) );
				}
				PathArray & m_files;
			};

			return TraverseDirectory( p_folderPath, DirectoryFunction( p_files ), FileFunction( p_files ) );
		}
		else
		{
			struct DirectoryFunction
			{
				DirectoryFunction()
				{
				}
				bool operator()( Path const & p_path )
				{
					return true;
				}
			};

			return TraverseDirectory( p_folderPath, DirectoryFunction(), FileFunction( p_files ) );
		}
	}
}

#endif
