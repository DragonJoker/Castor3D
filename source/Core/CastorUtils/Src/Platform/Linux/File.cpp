#include "Config/PlatformConfig.hpp"

#if defined( CASTOR_PLATFORM_LINUX )

#include "Data/File.hpp"

#include "Math/Math.hpp"
#include "Miscellaneous/Utils.hpp"
#include "Log/Logger.hpp"
#include <cstdio>
#include <cstring>
#include <sys/types.h>
#include <sys/stat.h>
#include <iostream>

#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <pwd.h>
#define GetCurrentDir getcwd

namespace Castor
{
	namespace
	{
		template< typename DirectoryFuncType, typename FileFuncType >
		bool TraverseDirectory( Path const & p_folderPath, DirectoryFuncType p_directoryFunction, FileFuncType p_fileFunction )
		{
			REQUIRE( !p_folderPath.empty() );
			bool l_result = false;
			DIR * l_dir;

			if ( ( l_dir = opendir( string::string_cast< char >( p_folderPath ).c_str() ) ) == nullptr )
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

				l_result = false;
			}
			else
			{
				l_result = true;
				dirent * l_dirent;

				while ( l_result && ( l_dirent = readdir( l_dir ) ) != nullptr )
				{
					String l_name = string::string_cast< xchar >( l_dirent->d_name );

					if ( l_name != cuT( "." ) && l_name != cuT( ".." ) )
					{
						if ( l_dirent->d_type == DT_DIR )
						{
							l_result = p_directoryFunction( p_folderPath / l_name );
						}
						else
						{
							p_fileFunction( p_folderPath / l_name );
						}
					}
				}

				closedir( l_dir );
			}

			return l_result;
		}

		bool DeleteEmptyDirectory( Path const & p_path )
		{
			bool l_result = rmdir( string::string_cast< char >( p_path ).c_str() ) == 0;

			if ( !l_result )
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

			return l_result;
		}

	}

	bool FOpen( FILE *& p_file, char const * p_path, char const * p_mode )
	{
		p_file = fopen( p_path, p_mode );
		return p_file != nullptr;
	}

	bool FOpen64( FILE *& p_file, char const * p_path, char const * p_mode )
	{
		p_file = fopen64( p_path, p_mode );
		return p_file != nullptr;
	}

	bool FSeek( FILE * p_file, int64_t p_offset, int p_iOrigin )
	{
		return fseeko64( p_file, p_offset, p_iOrigin ) == 0;
	}

	int64_t FTell( FILE * p_file )
	{
		return ftello64( p_file );
	}

	Path File::GetExecutableDirectory()
	{
		Path l_pathReturn;
		char l_path[FILENAME_MAX];
		char l_buffer[32];
		sprintf( l_buffer, "/proc/%d/exe", getpid() );
		int l_bytes = std::min< std::size_t >( readlink( l_buffer, l_path, sizeof( l_path ) ), sizeof( l_path ) - 1 );

		if ( l_bytes > 0 )
		{
			l_path[l_bytes] = '\0';
			l_pathReturn = Path{ string::string_cast< xchar >( l_path ) };
		}

		l_pathReturn = l_pathReturn.GetPath();
		return l_pathReturn;
	}

	Path File::GetUserDirectory()
	{
		Path l_pathReturn;
		struct passwd * l_pw = getpwuid( getuid() );
		const char * l_homedir = l_pw->pw_dir;
		l_pathReturn = Path{ string::string_cast< xchar >( l_homedir ) };
		return l_pathReturn;
	}

	bool File::DirectoryExists( Path const & p_path )
	{
		struct stat status = { 0 };
		stat( string::string_cast< char >( p_path ).c_str(), &status );
		return ( status.st_mode & S_IFDIR ) == S_IFDIR;
	}

	bool File::DirectoryCreate( Path const & p_path, FlagCombination< CreateMode > const & p_flags )
	{
		Path l_path = p_path.GetPath();

		if ( !l_path.empty() && !DirectoryExists( l_path ) )
		{
			DirectoryCreate( l_path, p_flags );
		}

		mode_t l_mode = 0;

		if ( CheckFlag( p_flags, CreateMode::eUserRead ) )
		{
			l_mode |= S_IRUSR;
		}

		if ( CheckFlag( p_flags, CreateMode::eUserWrite ) )
		{
			l_mode |= S_IWUSR;
		}

		if ( CheckFlag( p_flags, CreateMode::eUserExec ) )
		{
			l_mode |= S_IXUSR;
		}

		if ( CheckFlag( p_flags, CreateMode::eGroupRead ) )
		{
			l_mode |= S_IRGRP;
		}

		if ( CheckFlag( p_flags, CreateMode::eGroupWrite ) )
		{
			l_mode |= S_IWGRP;
		}

		if ( CheckFlag( p_flags, CreateMode::eGroupExec ) )
		{
			l_mode |= S_IXGRP;
		}

		if ( CheckFlag( p_flags, CreateMode::eOthersRead ) )
		{
			l_mode |= S_IROTH;
		}

		if ( CheckFlag( p_flags, CreateMode::eOthersWrite ) )
		{
			l_mode |= S_IWOTH;
		}

		if ( CheckFlag( p_flags, CreateMode::eOthersExec ) )
		{
			l_mode |= S_IXOTH;
		}

		return mkdir( string::string_cast< char >( p_path ).c_str(), l_mode ) == 0;
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
				bool l_result = TraverseDirectory( p_path, DirectoryFunction(), FileFunction() );

				if ( l_result )
				{
					l_result = DeleteEmptyDirectory( p_path );
				}

				return l_result;
			}
		};

		bool l_result = TraverseDirectory( p_path, DirectoryFunction(), FileFunction() );

		if ( l_result )
		{
			l_result = DeleteEmptyDirectory( p_path );
		}

		return l_result;
	}
}

#endif
