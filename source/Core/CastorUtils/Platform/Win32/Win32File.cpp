#include "CastorUtils/Config/PlatformConfig.hpp"

#if defined( CU_PlatformWindows )

#include "CastorUtils/Data/File.hpp"
#include "CastorUtils/Math/Math.hpp"
#include "CastorUtils/Miscellaneous/Utils.hpp"
#include "CastorUtils/Log/Logger.hpp"

#include <cstdio>
#include <cstring>
#include <filesystem>
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
	bool File::traverseDirectory( Path const & folderPath
		, TraverseDirFunction directoryFunction
		, HitFileFunction fileFunction )
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
					fileFunction( folderPath, name );
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
							fileFunction( folderPath, name );
						}
					}
				}
			}

			::FindClose( handle );
		}

		return result;
	}

#if defined( CU_CompilerMSVC )

	bool fileOpen( FILE *& p_file, std::filesystem::path const & p_path, char const * p_mode )
	{
		auto mode = string::stringCast< wchar_t >( std::string{ p_mode } );
		errno_t err = _wfopen_s( &p_file, p_path.c_str(), mode.c_str() );

		if ( err && !p_file )
		{
			Logger::logError( makeStringStream() << cuT( "Couldn't open file [" ) << p_path << cuT( "], due to error: " ) << System::getLastErrorText() );
		}

		return err == 0;
	}

	bool fileOpen64( FILE *& p_file, std::filesystem::path const & p_path, char const * p_mode )
	{
		auto mode = string::stringCast< wchar_t >( std::string{ p_mode } );
		errno_t err = _wfopen_s( &p_file, p_path.c_str(), mode.c_str() );

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

	bool fileOpen( FILE *& p_file, std::filesystem::path const & p_path, char const * p_mode )
	{
		p_file = fopen( p_path, p_mode );
		return p_file != nullptr;
	}

	bool fileOpen64( FILE *& p_file, std::filesystem::path const & p_path, char const * p_mode )
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

	bool File::deleteEmptyDirectory( Path const & p_path )
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

#endif
