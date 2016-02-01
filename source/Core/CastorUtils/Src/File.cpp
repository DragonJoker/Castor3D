#include "File.hpp"
#include "Math.hpp"
#include "Utils.hpp"
#include "Logger.hpp"
#include <cstdio>
#include <cstring>
#include <sys/types.h>
#include <sys/stat.h>
#include <iostream>

#if defined( _WIN32 )
#	include <direct.h>
#	include <Shlobj.h>
#	include <windows.h>
#	define GetCurrentDir _getcwd
#	undef CopyFile
#	undef DeleteFile
#else
#	include <unistd.h>
#	include <sys/types.h>
#	include <dirent.h>
#	include <errno.h>
#	include <pwd.h>
#	define GetCurrentDir getcwd
#endif

namespace Castor
{
	namespace
	{
		template< typename DirectoryFuncType, typename FileFuncType >
		bool TraverseDirectory( Path const & p_folderPath, DirectoryFuncType p_directoryFunction, FileFuncType p_fileFunction )
		{
			REQUIRE( !p_folderPath.empty() );
			bool l_return = false;

#if defined( _WIN32 )

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

#elif defined( __linux__ )

			DIR * l_dir;

			if ( ( l_dir = opendir( string::string_cast< char >( p_folderPath ).c_str() ) ) == NULL )
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

				l_return = false;
			}
			else
			{
				l_return = true;
				dirent * l_dirent;

				while ( l_return && ( l_dirent = readdir( l_dir ) ) != NULL )
				{
					String l_name = string::string_cast< xchar >( l_dirent->d_name );

					if ( l_name != cuT( "." ) && l_name != cuT( ".." ) )
					{
						if ( l_dirent->d_type == DT_DIR )
						{
							l_return = p_directoryFunction( p_folderPath / l_name );
						}
						else
						{
							p_fileFunction( p_folderPath / l_name );
						}
					}
				}

				closedir( l_dir );
			}

#else
#	error "Unsupported platform"
#endif
			return l_return;
		}

		bool DeleteEmptyDirectory( Path const & p_path )
		{
#if defined( _MSC_VER )

			bool l_return = _trmdir( p_path.c_str() ) == TRUE;

#else

			bool l_return = rmdir( string::string_cast< char >( p_path ).c_str() ) == 0;

#endif

			if ( !l_return )
			{
				switch ( errno )
				{
				case ENOTEMPTY:
					Logger::LogError( cuT( "Couldn't remove directory [" ) + p_path + cuT( "], it is not empty." ) );
					break;

				case ENOENT:
					Logger::LogError( cuT( "Couldn't remove directory [" ) + p_path + cuT( "], the path is invalid." ) );
					break;

				case EACCES:
					Logger::LogError( cuT( "Couldn't remove directory [" ) + p_path + cuT( "], a program has an open handle to this directory." ) );
					break;

				default:
					Logger::LogError( cuT( "Couldn't remove directory [" ) + p_path + cuT( "], unknown error." ) );
					break;
				}
			}

			return l_return;
		}

	}

#if defined( _MSC_VER)

	bool FOpen( FILE *& p_pFile, char const * p_pszPath, char const * p_pszMode )
	{
		errno_t l_err = fopen_s( &p_pFile, p_pszPath, p_pszMode );

		if ( l_err && !p_pFile )
		{
			Logger::LogError( System::GetLastErrorText() );
		}

		return l_err == 0;
	}

	bool FOpen64( FILE *& p_pFile, char const * p_pszPath, char const * p_pszMode )
	{
		return fopen_s( &p_pFile, p_pszPath, p_pszMode ) == 0;
	}

	bool FSeek( FILE * p_pFile, int64_t p_i64Offset, int p_iOrigin )
	{
		return _fseeki64( p_pFile, p_i64Offset, p_iOrigin ) == 0;
	}

	int64_t FTell( FILE * p_pFile )
	{
		return _ftelli64( p_pFile );
	}

#else

	bool FOpen( FILE *& p_pFile, char const * p_pszPath, char const * p_pszMode )
	{
		p_pFile = fopen( p_pszPath, p_pszMode );
		return p_pFile != NULL;
	}

#	if !defined( _WIN32 )
	bool FOpen64( FILE *& p_pFile, char const * p_pszPath, char const * p_pszMode )
	{
		p_pFile = fopen64( p_pszPath, p_pszMode );
		return p_pFile != NULL;
	}

	bool FSeek( FILE * p_pFile, int64_t p_i64Offset, int p_iOrigin )
	{
		return fseeko64( p_pFile, p_i64Offset, p_iOrigin ) == 0;
	}

	int64_t FTell( FILE * p_pFile )
	{
		return ftello64( p_pFile );
	}
#	else
	bool FOpen64( FILE *& p_pFile, char const * p_pszPath, char const * p_pszMode )
	{
		p_pFile = fopen( p_pszPath, p_pszMode );
		return p_pFile != NULL;
	}

	bool FSeek( FILE * p_pFile, int64_t p_i64Offset, int p_iOrigin )
	{
		return fseek( p_pFile, p_i64Offset, p_iOrigin ) == 0;
	}

	int64_t FTell( FILE * p_pFile )
	{
		return ftell( p_pFile );
	}

#	endif
#endif

	File::File( Path const & p_strFileName, int p_iMode, eENCODING_MODE p_eEncoding )
		:	m_iMode( p_iMode	)
		,	m_eEncoding( p_eEncoding	)
		,	m_strFileFullPath( p_strFileName	)
		,	m_ullCursor( 0	)
		,	m_ullLength( 0	)
		,	m_pFile( NULL	)
		,	m_bOwnFile( true	)
	{
		REQUIRE( ! p_strFileName.empty() );
		String l_strMode;

		switch ( p_iMode )
		{
		case eOPEN_MODE_READ:
			l_strMode = cuT( "r" );
			break;

		case eOPEN_MODE_WRITE:
			l_strMode = cuT( "w" );
			break;

		case eOPEN_MODE_APPEND:
			l_strMode = cuT( "a" );
			break;

		case eOPEN_MODE_READ | eOPEN_MODE_BINARY:
			l_strMode = cuT( "rb" );
			break;

		case eOPEN_MODE_WRITE | eOPEN_MODE_BINARY:
			l_strMode = cuT( "wb" );
			break;

		case eOPEN_MODE_APPEND | eOPEN_MODE_BINARY:
			l_strMode = cuT( "ab" );
			break;

		case eOPEN_MODE_READ | eOPEN_MODE_WRITE:
			l_strMode = cuT( "r+" );
			break;

		case eOPEN_MODE_READ | eOPEN_MODE_WRITE | eOPEN_MODE_APPEND:
			l_strMode = cuT( "a+" );
			break;

		default:
			FAILURE( "Unsupported file opening mode" );
			break;
		}

		if ( ( p_iMode & eOPEN_MODE_BINARY ) == 0 )
		{
			switch ( p_eEncoding )
			{
			case eENCODING_MODE_AUTO:
				m_eEncoding = eENCODING_MODE_UTF8;
				l_strMode += cuT( ", ccs=UTF-8" );
				break;

			case eENCODING_MODE_UTF8:
				l_strMode += cuT( ", ccs=UTF-8" );
				break;

			case eENCODING_MODE_UTF16:
				l_strMode += cuT( ", ccs=UTF-16LE" );
			}
		}

		FOpen( m_pFile, string::string_cast< char >( m_strFileFullPath ).c_str(), string::string_cast< char >( l_strMode ).c_str() );

		if ( m_pFile )
		{
			m_ullLength = 0;
			Castor::FSeek( m_pFile, 0, SEEK_END );
			m_ullLength = Castor::FTell( m_pFile );
			Castor::FSeek( m_pFile, 0, SEEK_SET );
		}
		else
		{
			CASTOR_EXCEPTION( "Couldn't open file " + string::string_cast< char >( m_strFileFullPath ) + " : " + string::string_cast< char >( System::GetLastErrorText() ) );
		}

		CHECK_INVARIANTS();
	}

	File::~File()
	{
		if ( m_pFile != NULL && m_bOwnFile )
		{
			fclose( m_pFile );
		}
	}

	int File::Seek( long long p_i64Offset, eOFFSET_MODE p_eOrigin )
	{
		CHECK_INVARIANTS();
		int l_iReturn = 0;

		if ( m_pFile )
		{
			switch ( p_eOrigin )
			{
			case eOFFSET_MODE_BEGINNING:
				l_iReturn = Castor::FSeek( m_pFile, p_i64Offset, SEEK_SET );
				m_ullCursor = p_i64Offset;
				break;

			case eOFFSET_MODE_CURRENT:
				l_iReturn = Castor::FSeek( m_pFile, p_i64Offset, SEEK_CUR );
				m_ullCursor += p_i64Offset;
				break;

			case eOFFSET_MODE_END:
				l_iReturn = Castor::FSeek( m_pFile, p_i64Offset, SEEK_END );
				m_ullCursor = GetLength() - p_i64Offset;
				break;
			}
		}

		CHECK_INVARIANTS();
		return l_iReturn;
	}

	long long File::GetLength()
	{
		CHECK_INVARIANTS();
		m_ullLength = 0;
		long long l_llPosition = Castor::FTell( m_pFile );
		Castor::FSeek( m_pFile, 0, SEEK_END );
		m_ullLength = Castor::FTell( m_pFile );
		Castor::FSeek( m_pFile, l_llPosition, SEEK_SET );
		CHECK_INVARIANTS();
		return m_ullLength;
	}

	bool File::IsOk()const
	{
		bool l_return = false;

		if ( m_pFile )
		{
			if ( ferror( m_pFile ) == 0 )
			{
				if ( feof( m_pFile ) == 0 )
				{
					l_return = true;
				}
			}
		}

		return l_return;
	}

	long long File::Tell()
	{
		CHECK_INVARIANTS();
		long long l_llReturn = 0;

		if ( m_pFile )
		{
			l_llReturn = Castor::FTell( m_pFile );
		}

		CHECK_INVARIANTS();
		return l_llReturn;
	}

	BEGIN_INVARIANT_BLOCK( File )
	CHECK_INVARIANT( m_pFile );
	END_INVARIANT_BLOCK()

	uint64_t File::DoWrite( uint8_t const * p_buffer, uint64_t p_uiSize )
	{
		CHECK_INVARIANTS();
		REQUIRE( IsOk() && ( m_iMode & eOPEN_MODE_WRITE ) );
		uint64_t l_uiReturn = 0;

		if ( IsOk() )
		{
			l_uiReturn = fwrite( p_buffer, 1, std::size_t( p_uiSize ), m_pFile );
			m_ullCursor += l_uiReturn;
			ENSURE( l_uiReturn <= p_uiSize );
		}

		CHECK_INVARIANTS();
		return l_uiReturn;
	}

	uint64_t File::DoRead( uint8_t * p_buffer, uint64_t p_uiSize )
	{
		CHECK_INVARIANTS();
		REQUIRE( IsOk() && ( m_iMode & eOPEN_MODE_READ ) );
		uint64_t l_uiReturn = 0;
		uint64_t l_uiPrev = 1;

		if ( IsOk() )
		{
			while ( l_uiReturn < p_uiSize && l_uiPrev != l_uiReturn )
			{
				l_uiPrev = l_uiReturn;
				l_uiReturn += fread( p_buffer, 1, std::size_t( p_uiSize - l_uiReturn ), m_pFile );
			}

			m_ullCursor += l_uiReturn;
			ENSURE( l_uiReturn <= p_uiSize );
		}

		CHECK_INVARIANTS();
		return l_uiReturn;
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

	Path File::GetExecutableDirectory()
	{
		Path l_pathReturn;

#if defined( _WIN32 )

		xchar l_pPath[FILENAME_MAX];
		DWORD dwResult = GetModuleFileName( NULL, l_pPath, _countof( l_pPath ) );

		if ( dwResult != 0 )
		{
			l_pathReturn = l_pPath;
		}

#elif defined( __linux__ )

		char l_pPath[FILENAME_MAX];
		char l_szTmp[32];
		sprintf( l_szTmp, "/proc/%d/exe", getpid() );
		int l_iBytes = std::min< std::size_t >( readlink( l_szTmp, l_pPath, sizeof( l_pPath ) ), sizeof( l_pPath ) - 1 );

		if ( l_iBytes > 0 )
		{
			l_pPath[l_iBytes] = '\0';
			l_pathReturn = l_pPath;
		}

#else
#	error "Unsupported platform"
#endif

		l_pathReturn = l_pathReturn.GetPath();
		return l_pathReturn;
	}

	Path File::GetUserDirectory()
	{
		Path l_pathReturn;

#if defined( _WIN32 )

		xchar l_path[FILENAME_MAX];
		HRESULT l_hr = SHGetFolderPath( NULL, CSIDL_PROFILE, NULL, 0, l_path );

		if ( SUCCEEDED( l_hr ) )
		{
			l_pathReturn = l_path;
		}

#elif defined( __linux__ )

		struct passwd * l_pw = getpwuid( getuid() );
		const char * l_homedir = l_pw->pw_dir;
		l_pathReturn = string::string_cast< xchar >( l_homedir );

#else
#	error "Unsupported platform"
#endif

		return l_pathReturn;
	}

	bool File::DirectoryExists( Path const & p_path )
	{
#if defined( _WIN32 )

		struct _stat status = { 0 };
		_stat( string::string_cast< char >( p_path ).c_str(), &status );

#else

		struct stat status = { 0 };
		stat( string::string_cast< char >( p_path ).c_str(), &status );

#endif

		return ( status.st_mode & S_IFDIR ) == S_IFDIR;
	}

	bool File::DirectoryCreate( Path const & p_path, uint32_t p_flags )
	{
		Path l_path = p_path.GetPath();

		if ( !l_path.empty() && !DirectoryExists( l_path ) )
		{
			DirectoryCreate( l_path, p_flags );
		}

#if defined( _MSC_VER )

		return _tmkdir( p_path.c_str() ) == 0;

#elif defined( _WIN32 )

		return mkdir( string::string_cast< char >( p_path ).c_str() ) == 0;

#else
		mode_t l_mode = 0;

		if ( ( p_flags & eCREATE_MODE_USER_READ ) == eCREATE_MODE_USER_READ )
		{
			l_mode |= S_IRUSR;
		}

		if ( ( p_flags & eCREATE_MODE_USER_WRITE ) == eCREATE_MODE_USER_WRITE )
		{
			l_mode |= S_IWUSR;
		}

		if ( ( p_flags & eCREATE_MODE_USER_EXEC ) == eCREATE_MODE_USER_EXEC )
		{
			l_mode |= S_IXUSR;
		}

		if ( ( p_flags & eCREATE_MODE_GROUP_READ ) == eCREATE_MODE_GROUP_READ )
		{
			l_mode |= S_IRGRP;
		}

		if ( ( p_flags & eCREATE_MODE_GROUP_WRITE ) == eCREATE_MODE_GROUP_WRITE )
		{
			l_mode |= S_IWGRP;
		}

		if ( ( p_flags & eCREATE_MODE_GROUP_EXEC ) == eCREATE_MODE_GROUP_EXEC )
		{
			l_mode |= S_IXGRP;
		}

		if ( ( p_flags & eCREATE_MODE_OTHERS_READ ) == eCREATE_MODE_OTHERS_READ )
		{
			l_mode |= S_IROTH;
		}

		if ( ( p_flags & eCREATE_MODE_OTHERS_WRITE ) == eCREATE_MODE_OTHERS_WRITE )
		{
			l_mode |= S_IWOTH;
		}

		if ( ( p_flags & eCREATE_MODE_OTHERS_EXEC ) == eCREATE_MODE_OTHERS_EXEC )
		{
			l_mode |= S_IXOTH;
		}

		return mkdir( string::string_cast< char >( p_path ).c_str(), l_mode ) == 0;

#endif
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
				bool l_return = TraverseDirectory( p_path, DirectoryFunction(), FileFunction() );

				if ( l_return )
				{
					l_return = DeleteEmptyDirectory( p_path );
				}

				return l_return;
			}
		};

		bool l_return = TraverseDirectory( p_path, DirectoryFunction(), FileFunction() );

		if ( l_return )
		{
			l_return = DeleteEmptyDirectory( p_path );
		}

		return l_return;
	}

	bool File::FileExists( Path const & p_pathFile )
	{
		std::ifstream l_ifile( string::string_cast< char >( p_pathFile ).c_str() );
		return l_ifile.is_open();
	}

	bool File::DeleteFile( Path const & p_file )
	{
		bool l_return = false;

		if ( FileExists( p_file ) )
		{
			l_return = std::remove( string::string_cast< char >( p_file ).c_str() ) == 0;
		}
		else
		{
			l_return = true;
		}

		return l_return;
	}

	bool File::CopyFile( Path const & p_file, Path const & p_folder )
	{
		bool l_return = false;
		Path l_file = p_folder / p_file.GetFileName() + cuT( "." ) + p_file.GetExtension();
		std::ifstream l_src( string::string_cast< char >( p_file ), std::ios::binary );

		if ( l_src.is_open() )
		{
			std::ofstream l_dst( string::string_cast< char >( l_file ), std::ios::binary );

			if ( l_src.is_open() )
			{
				l_dst << l_src.rdbuf();
				l_return = true;
			}
			else
			{
				Logger::LogWarning( cuT( "CopyFile - Can't open destination file : " ) + p_file );
			}
		}
		else
		{
			Logger::LogWarning( cuT( "CopyFile - Can't open source file : " ) + p_file );
		}

		return l_return;
	}
}
