#include "CastorUtils/Config/PlatformConfig.hpp"

#if defined( CU_PlatformWindows )

#include "CastorUtils/Data/File.hpp"
#include "CastorUtils/Math/Math.hpp"
#include "CastorUtils/Miscellaneous/Utils.hpp"
#include "CastorUtils/Log/Logger.hpp"

#include <cstdio>
#include <cstring>
#include <sys/types.h>
#include <sys/stat.h>
#include <iostream>

#include <direct.h>
#include <Shlobj.h>
#include <windows.h>
#define getCurrentDir _getcwd
#undef copyFile
#undef deleteFile

namespace castor
{
	namespace
	{
		template< typename DirectoryFuncType, typename FileFuncType >
		bool TraverseDirectory( Path const & folderPath, DirectoryFuncType directoryFunction, FileFuncType fileFunction )
		{
			CU_Require( !folderPath.empty() );
			bool result = false;
			WIN32_FIND_DATAA findData;
			HANDLE handle = ::FindFirstFileA( ( folderPath / cuT( "*.*" ) ).c_str(), &findData );

			if ( handle != INVALID_HANDLE_VALUE )
			{
				result = true;
				String name = findData.cFileName;

				if ( name != cuT( "." ) && name != cuT( ".." ) )
				{
					if ( ( findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) == FILE_ATTRIBUTE_DIRECTORY )
					{
						result = directoryFunction( folderPath / name );
					}
					else
					{
						fileFunction( folderPath / name );
					}
				}

				while ( result && ::FindNextFileA( handle, &findData ) == TRUE )
				{
					if ( findData.cFileName != name )
					{
						name = findData.cFileName;

						if ( name != cuT( "." ) && name != cuT( ".." ) )
						{
							if ( ( findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) == FILE_ATTRIBUTE_DIRECTORY )
							{
								result = directoryFunction( folderPath / name );
							}
							else
							{
								fileFunction( folderPath / name );
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
#if defined( CU_CompilerMSVC )

			bool result = _rmdir( p_path.c_str() ) == 0;

#else

			bool result = rmdir( string::stringCast< char >( p_path ).c_str() ) == 0;

#endif

			if ( !result )
			{
				auto error = errno;

				switch ( error )
				{
				case ENOTEMPTY:
					Logger::logError( makeStringStream() << cuT( "Couldn't remove directory [" ) << p_path << cuT( "], it is not empty." ) );
					break;

				case ENOENT:
					Logger::logError( makeStringStream() << cuT( "Couldn't remove directory [" ) << p_path << cuT( "], the path is invalid." ) );
					break;

				case EACCES:
					Logger::logError( makeStringStream() << cuT( "Couldn't remove directory [" ) << p_path << cuT( "], a program has an open handle to this directory." ) );
					break;

				default:
					Logger::logError( makeStringStream() << cuT( "Couldn't remove directory [" ) << p_path << cuT( "], unknown error (" ) << error << cuT( ")." ) );
					break;
				}
			}

			return result;
		}

	}

#if defined( CU_CompilerMSVC )

	bool fileOpen( FILE *& p_file, char const * p_path, char const * p_mode )
	{
		errno_t err = fopen_s( &p_file, p_path, p_mode );

		if ( err && !p_file )
		{
			Logger::logError( makeStringStream() << cuT( "Couldn't open file [" ) << p_path << cuT( "], due to error: " ) << System::getLastErrorText() );
		}

		return err == 0;
	}

	bool fileOpen64( FILE *& p_file, char const * p_path, char const * p_mode )
	{
		errno_t err = fopen_s( &p_file, p_path, p_mode );

		if ( err && !p_file )
		{
			Logger::logError( makeStringStream() << cuT( "Couldn't open file [" ) << p_path << cuT( "], due to error: " ) << System::getLastErrorText() );
		}

		return err == 0;
	}

	bool fileSeek( FILE * p_file, int64_t p_offset, int p_iOrigin )
	{
		return _fseeki64( p_file, p_offset, p_iOrigin ) == 0;
	}

	int64_t fileTell( FILE * p_file )
	{
		return _ftelli64( p_file );
	}

#else

	bool fileOpen( FILE *& p_file, char const * p_path, char const * p_mode )
	{
		p_file = fopen( p_path, p_mode );
		return p_file != nullptr;
	}

	bool fileOpen64( FILE *& p_file, char const * p_path, char const * p_mode )
	{
		p_file = fopen( p_path, p_mode );
		return p_file != nullptr;
	}

	bool fileSeek( FILE * p_file, int64_t p_offset, int p_iOrigin )
	{
		return fseek( p_file, p_offset, p_iOrigin ) == 0;
	}

	int64_t fileTell( FILE * p_file )
	{
		return ftell( p_file );
	}

#endif

	Path File::getExecutableDirectory()
	{
		Path pathReturn;
		xchar path[FILENAME_MAX];
		DWORD result = ::GetModuleFileNameA( nullptr, path, _countof( path ) );

		if ( result != 0 )
		{
			pathReturn = Path{ path };
		}

		pathReturn = pathReturn.getPath();
		return pathReturn;
	}

	Path File::getUserDirectory()
	{
		Path pathReturn;
		xchar path[FILENAME_MAX];
		HRESULT hr = SHGetFolderPathA( nullptr, CSIDL_PROFILE, nullptr, 0, path );

		if ( SUCCEEDED( hr ) )
		{
			pathReturn = Path{ path };
		}

		return pathReturn;
	}

	bool File::directoryExists( Path const & p_path )
	{
		struct _stat status = { 0 };
		_stat( string::stringCast< char >( p_path ).c_str(), &status );
		return ( status.st_mode & S_IFDIR ) == S_IFDIR;
	}

	bool File::directoryCreate( Path const & p_path, FlagCombination< CreateMode > const & p_flags )
	{
		Path path = p_path.getPath();

		if ( !path.empty() && !directoryExists( path ) )
		{
			directoryCreate( path, p_flags );
		}

#if defined( CU_CompilerMSVC )

		return _mkdir( p_path.c_str() ) == 0;

#else

		return mkdir( string::stringCast< char >( p_path ).c_str() ) == 0;

#endif
	}

	bool File::listDirectoryFiles( Path const & folderPath, PathArray & p_files, bool p_recursive )
	{
		struct FileFunction
		{
			explicit FileFunction( PathArray & p_files )
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
				explicit DirectoryFunction( PathArray & p_files )
					: m_files( p_files )
				{
				}
				bool operator()( Path const & p_path )
				{
					return TraverseDirectory( p_path, DirectoryFunction( m_files ), FileFunction( m_files ) );
				}
				PathArray & m_files;
			};

			return TraverseDirectory( folderPath, DirectoryFunction( p_files ), FileFunction( p_files ) );
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

			return TraverseDirectory( folderPath, DirectoryFunction(), FileFunction( p_files ) );
		}
	}

	bool File::directoryDelete( Path const & p_path )
	{
		struct FileFunction
		{
			void operator()( Path const & p_path )
			{
				File::deleteFile( p_path );
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
