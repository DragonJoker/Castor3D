#include "CastorUtils/Config/PlatformConfig.hpp"

#if defined( CU_PlatformLinux )

#include "CastorUtils/Data/File.hpp"
#include "CastorUtils/Log/Logger.hpp"
#include "CastorUtils/Miscellaneous/Utils.hpp"

#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <pwd.h>

namespace castor
{
	namespace
	{
		void printErrnoName( String const & type
			, Path const & path )
		{
			switch ( errno )
			{
			case EACCES:
				Logger::logWarning( cuT( "Can't open " ) + type + cuT( " [" ) + path + cuT( "]: Permission denied." ) );
				break;

			case EBADF:
				Logger::logWarning( cuT( "Can't open " ) + type + cuT( "[" ) + path + cuT( "]: Invalid file descriptor." ) );
				break;

			case EFAULT:
				Logger::logWarning( cuT( "Can't open " ) + type + cuT( "[" ) + path + cuT( "]: Bad address." ) );
				break;

			case ELOOP:
				Logger::logWarning( cuT( "Can't open " ) + type + cuT( "[" ) + path + cuT( "]: Too many symbolic links encountered while traversing the path." ) );
				break;

			case ENAMETOOLONG:
				Logger::logWarning( cuT( "Can't open " ) + type + cuT( "[" ) + path + cuT( "]: <name> is too long." ) );
				break;

			case EMFILE:
				Logger::logWarning( cuT( "Can't open " ) + type + cuT( "[" ) + path + cuT( "]: Too many file descriptor in use." ) );
				break;

			case ENFILE:
				Logger::logWarning( cuT( "Can't open " ) + type + cuT( "[" ) + path + cuT( "]: Too many files currently open." ) );
				break;

			case ENOENT:
				Logger::logWarning( cuT( "Can't open " ) + type + cuT( "[" ) + path + cuT( "]: <name> doesn't exist." ) );
				break;

			case ENOMEM:
				Logger::logWarning( cuT( "Can't open " ) + type + cuT( "[" ) + path + cuT( "]: Insufficient memory." ) );
				break;

			case ENOTDIR:
				Logger::logWarning( cuT( "Can't open " ) + type + cuT( "[" ) + path + cuT( "]: <name> is not a directory." ) );
				break;

			case EOVERFLOW:
				Logger::logWarning( cuT( "Can't open " ) + type + cuT( "[" ) + path + cuT( "]: <name> refers to a file whose size, inode number, or number of blocks cannot be represented in, respectively, the types off_t, ino_t, or blkcnt_t." ) );
				break;

			default:
				Logger::logWarning( cuT( "Can't open " ) + type + cuT( "[" ) + path + cuT( "]: Unknown error." ) );
				break;
			}
		}
	}

	bool File::traverseDirectory( Path const & folderPath
		, TraverseDirFunction directoryFunction
		, HitFileFunction fileFunction )
	{
		CU_Require( !folderPath.empty() );
		bool result = false;
		DIR * dir;

		if ( ( dir = opendir( string::stringCast< char >( folderPath ).c_str() ) ) == nullptr )
		{
			printErrnoName( cuT( "folder" ), folderPath );
			result = false;
		}
		else
		{
			result = true;
			dirent * dirent;

			while ( result && ( dirent = readdir( dir ) ) != nullptr )
			{
				String name = string::stringCast< xchar >( dirent->d_name );

				if ( name != cuT( "." ) && name != cuT( ".." ) )
				{
					if ( dirent->d_type == DT_DIR )
					{
						result = directoryFunction( folderPath / name );
					}
					else
					{
						fileFunction( folderPath, name );
					}
				}
			}

			closedir( dir );
		}

		return result;
	}

	bool fileOpen( FILE *& p_file, char const * p_path, char const * p_mode )
	{
		p_file = fopen( p_path, p_mode );
		return p_file != nullptr;
	}

	bool fileOpen64( FILE *& p_file, char const * p_path, char const * p_mode )
	{
		p_file = fopen64( p_path, p_mode );
		return p_file != nullptr;
	}

	bool fileSeek( FILE * p_file, int64_t p_offset, int p_iOrigin )
	{
		return fseeko64( p_file, p_offset, p_iOrigin ) == 0;
	}

	int64_t fileTell( FILE * p_file )
	{
		return ftello64( p_file );
	}

	Path File::getExecutableDirectory()
	{
		Path pathReturn;
		char path[FILENAME_MAX];
		char buffer[32];
		sprintf( buffer, "/proc/%d/exe", getpid() );
		int bytes = std::min( int( readlink( buffer, path, sizeof( path ) ) ), int( sizeof( path ) ) - 1 );

		if ( bytes > 0 )
		{
			path[bytes] = '\0';
			pathReturn = Path{ string::stringCast< xchar >( path ) };
		}

		pathReturn = pathReturn.getPath();
		return pathReturn;
	}

	Path File::getUserDirectory()
	{
		Path pathReturn;
		struct passwd * pw = getpwuid( getuid() );
		const char * homedir = pw->pw_dir;
		pathReturn = Path{ string::stringCast< xchar >( homedir ) };
		return pathReturn;
	}

	bool File::directoryExists( Path const & p_path )
	{
		struct stat status{};
		stat( string::stringCast< char >( p_path ).c_str(), &status );
		return ( status.st_mode & S_IFDIR ) == S_IFDIR;
	}

	bool File::directoryCreate( Path const & p_path, FlagCombination< CreateMode > const & p_flags )
	{
		Path path = p_path.getPath();

		if ( !path.empty() && !directoryExists( path ) )
		{
			directoryCreate( path, p_flags );
		}

		mode_t mode = 0;

		if ( checkFlag( p_flags, CreateMode::eUserRead ) )
		{
			mode |= S_IRUSR;
		}

		if ( checkFlag( p_flags, CreateMode::eUserWrite ) )
		{
			mode |= S_IWUSR;
		}

		if ( checkFlag( p_flags, CreateMode::eUserExec ) )
		{
			mode |= S_IXUSR;
		}

		if ( checkFlag( p_flags, CreateMode::eGroupRead ) )
		{
			mode |= S_IRGRP;
		}

		if ( checkFlag( p_flags, CreateMode::eGroupWrite ) )
		{
			mode |= S_IWGRP;
		}

		if ( checkFlag( p_flags, CreateMode::eGroupExec ) )
		{
			mode |= S_IXGRP;
		}

		if ( checkFlag( p_flags, CreateMode::eOthersRead ) )
		{
			mode |= S_IROTH;
		}

		if ( checkFlag( p_flags, CreateMode::eOthersWrite ) )
		{
			mode |= S_IWOTH;
		}

		if ( checkFlag( p_flags, CreateMode::eOthersExec ) )
		{
			mode |= S_IXOTH;
		}

		return mkdir( string::stringCast< char >( p_path ).c_str(), mode ) == 0;
	}

	bool File::deleteEmptyDirectory( Path const & p_path )
	{
		bool result = rmdir( string::stringCast< char >( p_path ).c_str() ) == 0;

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
