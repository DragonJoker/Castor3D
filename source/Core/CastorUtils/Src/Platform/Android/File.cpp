#include "Config/PlatformConfig.hpp"

#if defined( CASTOR_PLATFORM_ANDROID )

#include "Data/File.hpp"

#include "Math/Math.hpp"
#include "Miscellaneous/Utils.hpp"
#include "Log/Logger.hpp"
#include <cstdio>
#include <cstring>
#include <sys/types.h>
#include <sys/stat.h>
#include <iostream>

#	include <unistd.h>
#	include <sys/types.h>
#	include <dirent.h>
#	include <errno.h>
#	include <pwd.h>
#	define GetCurrentDir getcwd

namespace Castor
{
	namespace
	{
		template< typename DirectoryFuncType, typename FileFuncType >
		bool TraverseDirectory( Path const & p_folderPath, DirectoryFuncType p_directoryFunction, FileFuncType p_fileFunction )
		{
			REQUIRE( !p_folderPath.empty() );
			bool result = false;
			DIR * dir;

			if ( ( dir = opendir( string::string_cast< char >( p_folderPath ).c_str() ) ) == nullptr )
			{
				switch ( errno )
				{
				case EACCES:
					Logger::LogWarning( cuT( "Can't open dir : Permission denied - Directory : " ) + p_folderPath );
					break;

				case EBADF:
					Logger::LogWarning( cuT( "Can't open dir : Invalid file descriptor - Directory : " ) + p_folderPath );
					break;

				case EMFILE:
					Logger::LogWarning( cuT( "Can't open dir : Too many file descriptor in use - Directory : " ) + p_folderPath );
					break;

				case ENFILE:
					Logger::LogWarning( cuT( "Can't open dir : Too many files currently open - Directory : " ) + p_folderPath );
					break;

				case ENOENT:
					Logger::LogWarning( cuT( "Can't open dir : Directory doesn't exist - Directory : " ) + p_folderPath );
					break;

				case ENOMEM:
					Logger::LogWarning( cuT( "Can't open dir : Insufficient memory - Directory : " ) + p_folderPath );
					break;

				case ENOTDIR:
					Logger::LogWarning( cuT( "Can't open dir : <name> is not a directory - Directory : " ) + p_folderPath );
					break;

				default:
					Logger::LogWarning( cuT( "Can't open dir : Unknown error - Directory : " ) + p_folderPath );
					break;
				}

				result = false;
			}
			else
			{
				result = true;
				dirent * dirent;

				while ( result && ( dirent = readdir( dir ) ) != nullptr )
				{
					String name = string::string_cast< xchar >( dirent->d_name );

					if ( name != cuT( "." ) && name != cuT( ".." ) )
					{
						if ( dirent->d_type == DT_DIR )
						{
							result = p_directoryFunction( p_folderPath / name );
						}
						else
						{
							p_fileFunction( p_folderPath / name );
						}
					}
				}

				closedir( dir );
			}

			return result;
		}

		bool DeleteEmptyDirectory( Path const & p_path )
		{
			bool result = rmdir( string::string_cast< char >( p_path ).c_str() ) == 0;

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

	Path File::GetExecutableDirectory()
	{
		Path pathReturn;
		char path[FILENAME_MAX];
		char buffer[32];
		sprintf( buffer, "/proc/%d/exe", getpid() );
		int bytes = std::min< std::size_t >( readlink( buffer, path, sizeof( path ) ), sizeof( path ) - 1 );

		if ( bytes > 0 )
		{
			path[bytes] = '\0';
			pathReturn = Path{ string::string_cast< xchar >( path ) };
		}

		pathReturn = pathReturn.GetPath();
		return pathReturn;
	}

	Path File::GetUserDirectory()
	{
		Path pathReturn;
		struct passwd * pw = getpwuid( getuid() );
		const char * homedir = pw->pw_dir;
		pathReturn = Path{ string::string_cast< xchar >( homedir ) };

		return pathReturn;
	}

	bool File::DirectoryExists( Path const & p_path )
	{
		struct stat status = { 0 };
		stat( string::string_cast< char >( p_path ).c_str(), &status );
		return ( status.st_mode & S_IFDIR ) == S_IFDIR;
	}

	bool File::DirectoryCreate( Path const & p_path, FlagCombination< CreateMode > const & p_flags )
	{
		Path path = p_path.GetPath();

		if ( !path.empty() && !DirectoryExists( path ) )
		{
			DirectoryCreate( path, p_flags );
		}

		mode_t mode = 0;

		if ( CheckFlag( p_flags, CreateMode::eUserRead ) )
		{
			mode |= S_IRUSR;
		}

		if ( CheckFlag( p_flags, CreateMode::eUserWrite ) )
		{
			mode |= S_IWUSR;
		}

		if ( CheckFlag( p_flags, CreateMode::eUserExec ) )
		{
			mode |= S_IXUSR;
		}

		if ( CheckFlag( p_flags, CreateMode::eGroupRead ) )
		{
			mode |= S_IRGRP;
		}

		if ( CheckFlag( p_flags, CreateMode::eGroupWrite ) )
		{
			mode |= S_IWGRP;
		}

		if ( CheckFlag( p_flags, CreateMode::eGroupExec ) )
		{
			mode |= S_IXGRP;
		}

		if ( CheckFlag( p_flags, CreateMode::eOthersRead ) )
		{
			mode |= S_IROTH;
		}

		if ( CheckFlag( p_flags, CreateMode::eOthersWrite ) )
		{
			mode |= S_IWOTH;
		}

		if ( CheckFlag( p_flags, CreateMode::eOthersExec ) )
		{
			mode |= S_IXOTH;
		}

		return mkdir( string::string_cast< char >( p_path ).c_str(), mode ) == 0;
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
