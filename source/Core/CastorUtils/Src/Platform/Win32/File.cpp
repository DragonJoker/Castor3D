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
			bool result = false;
			WIN32_FIND_DATA findData;
			HANDLE handle = ::FindFirstFile( ( p_folderPath / cuT( "*.*" ) ).c_str(), &findData );

			if ( handle != INVALID_HANDLE_VALUE )
			{
				result = true;
				String name = findData.cFileName;

				if ( name != cuT( "." ) && name != cuT( ".." ) )
				{
					if ( ( findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) == FILE_ATTRIBUTE_DIRECTORY )
					{
						result = p_directoryFunction( p_folderPath / name );
					}
					else
					{
						p_fileFunction( p_folderPath / name );
					}
				}

				while ( result && ::FindNextFile( handle, &findData ) == TRUE )
				{
					if ( findData.cFileName != name )
					{
						name = findData.cFileName;

						if ( name != cuT( "." ) && name != cuT( ".." ) )
						{
							if ( ( findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) == FILE_ATTRIBUTE_DIRECTORY )
							{
								result = p_directoryFunction( p_folderPath / name );
							}
							else
							{
								p_fileFunction( p_folderPath / name );
							}
						}
					}
				}

				::FindClose( handle );
			}

			return result;
		}

		bool DeleteEmptyDirectory( Path const & p_path )
		{
#if defined( CASTOR_COMPILER_MSVC )

			bool result = _trmdir( p_path.c_str() ) == 0;

#else

			bool result = rmdir( string::string_cast< char >( p_path ).c_str() ) == 0;

#endif

			if ( !result )
			{
				auto error = errno;

				switch ( error )
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
					Logger::LogError( StringStream() << cuT( "Couldn't remove directory [" ) << p_path << cuT( "], unknown error (" ) << error << cuT( ")." ) );
					break;
				}
			}

			return result;
		}

	}

#if defined( CASTOR_COMPILER_MSVC )

	bool FOpen( FILE *& p_file, char const * p_path, char const * p_mode )
	{
		errno_t err = fopen_s( &p_file, p_path, p_mode );

		if ( err && !p_file )
		{
			Logger::LogError( StringStream() << cuT( "Couldn't open file [" ) << p_path << cuT( "], due to error: " ) << System::GetLastErrorText() );
		}

		return err == 0;
	}

	bool FOpen64( FILE *& p_file, char const * p_path, char const * p_mode )
	{
		errno_t err = fopen_s( &p_file, p_path, p_mode );

		if ( err && !p_file )
		{
			Logger::LogError( StringStream() << cuT( "Couldn't open file [" ) << p_path << cuT( "], due to error: " ) << System::GetLastErrorText() );
		}

		return err == 0;
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
		Path pathReturn;
		xchar path[FILENAME_MAX];
		DWORD result = GetModuleFileName( nullptr, path, _countof( path ) );

		if ( result != 0 )
		{
			pathReturn = Path{ path };
		}

		pathReturn = pathReturn.GetPath();
		return pathReturn;
	}

	Path File::GetUserDirectory()
	{
		Path pathReturn;
		xchar path[FILENAME_MAX];
		HRESULT hr = SHGetFolderPath( nullptr, CSIDL_PROFILE, nullptr, 0, path );

		if ( SUCCEEDED( hr ) )
		{
			pathReturn = Path{ path };
		}

		return pathReturn;
	}

	bool File::DirectoryExists( Path const & p_path )
	{
		struct _stat status = { 0 };
		_stat( string::string_cast< char >( p_path ).c_str(), &status );
		return ( status.st_mode & S_IFDIR ) == S_IFDIR;
	}

	bool File::DirectoryCreate( Path const & p_path, FlagCombination< CreateMode > const & p_flags )
	{
		Path path = p_path.GetPath();

		if ( !path.empty() && !DirectoryExists( path ) )
		{
			DirectoryCreate( path, p_flags );
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

	bool File::DirectoryDelete( Path const & p_path )
	{
		struct FileFunction
		{
			void operator()( Path const & p_path )
			{
				File::DeleteFile( p_path );
			}
		};

		struct DirectoryFunction
		{
			bool operator()( Path const & p_path )
			{
				bool result = TraverseDirectory( p_path, DirectoryFunction(), FileFunction() );

				if ( result )
				{
					result = DeleteEmptyDirectory( p_path );
				}

				return result;
			}
		};

		bool result = TraverseDirectory( p_path, DirectoryFunction(), FileFunction() );

		if ( result )
		{
			result = DeleteEmptyDirectory( p_path );
		}

		return result;
	}
}

#endif
