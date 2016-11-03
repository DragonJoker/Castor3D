#include "File.hpp"
#include "Math/Math.hpp"
#include "Miscellaneous/Utils.hpp"
#include "Log/Logger.hpp"
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

				l_return = false;
			}
			else
			{
				l_return = true;
				dirent * l_dirent;

				while ( l_return && ( l_dirent = readdir( l_dir ) ) != nullptr )
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

#if defined( _MSC_VER)

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

#	if !defined( _WIN32 )
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
#	else
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

#	endif
#endif

	File::File( Path const & p_fileName, OpenMode p_mode, EncodingMode p_encoding )
		: File{ p_fileName, uint32_t( p_mode ), p_encoding }
	{
	}

	File::File( Path const & p_strFileName, uint32_t p_mode, EncodingMode p_encoding )
		: m_mode{ p_mode }
		, m_encoding{ p_encoding }
		, m_strFileFullPath{ p_strFileName }
	{
		REQUIRE( !p_strFileName.empty() );
		String l_mode;

		switch ( p_mode )
		{
		case uint32_t( OpenMode::Read ):
			l_mode = cuT( "r" );
			break;

		case uint32_t( OpenMode::Write ):
			l_mode = cuT( "w" );
			break;

		case uint32_t( OpenMode::Append ):
			l_mode = cuT( "a" );
			break;

		case uint32_t( OpenMode::Read ) | uint32_t( OpenMode::Binary ):
			l_mode = cuT( "rb" );
			break;

		case uint32_t( OpenMode::Write ) | uint32_t( OpenMode::Binary ):
			l_mode = cuT( "wb" );
			break;

		case uint32_t( OpenMode::Append ) | uint32_t( OpenMode::Binary ):
			l_mode = cuT( "ab" );
			break;

		case uint32_t( OpenMode::Read ) | uint32_t( OpenMode::Write ):
			l_mode = cuT( "r+" );
			break;

		case uint32_t( OpenMode::Read ) | uint32_t( OpenMode::Write ) | uint32_t( OpenMode::Append ):
			l_mode = cuT( "a+" );
			break;

		default:
			FAILURE( "Unsupported file opening mode" );
			break;
		}

		if ( !CheckFlag( p_mode, OpenMode::Binary ) )
		{
			switch ( p_encoding )
			{
			case EncodingMode::Auto:
				m_encoding = EncodingMode::UTF8;
				l_mode += cuT( ", ccs=UTF-8" );
				break;

			case EncodingMode::UTF8:
				l_mode += cuT( ", ccs=UTF-8" );
				break;

			case EncodingMode::UTF16:
				l_mode += cuT( ", ccs=UTF-16LE" );
			}
		}

		FOpen( m_pFile, string::string_cast< char >( m_strFileFullPath ).c_str(), string::string_cast< char >( l_mode ).c_str() );

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
		if ( m_pFile != nullptr )
		{
			fclose( m_pFile );
		}
	}

	int File::Seek( long long p_offset, OffsetMode p_origin )
	{
		CHECK_INVARIANTS();
		int l_iReturn = 0;

		if ( m_pFile )
		{
			switch ( p_origin )
			{
			case OffsetMode::Beginning:
				l_iReturn = Castor::FSeek( m_pFile, p_offset, SEEK_SET );
				m_ullCursor = p_offset;
				break;

			case OffsetMode::Current:
				l_iReturn = Castor::FSeek( m_pFile, p_offset, SEEK_CUR );
				m_ullCursor += p_offset;
				break;

			case OffsetMode::End:
				l_iReturn = Castor::FSeek( m_pFile, p_offset, SEEK_END );
				m_ullCursor = GetLength() - p_offset;
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
		REQUIRE( IsOk() && ( CheckFlag( m_mode, OpenMode::Write ) || CheckFlag( m_mode, OpenMode::Append ) ) );
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
		REQUIRE( IsOk() && CheckFlag( m_mode, OpenMode::Read ) );
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

		xchar l_path[FILENAME_MAX];
		DWORD l_result = GetModuleFileName( nullptr, l_path, _countof( l_path ) );

		if ( l_result != 0 )
		{
			l_pathReturn = Path{ l_path };
		}

#elif defined( __linux__ )

		char l_path[FILENAME_MAX];
		char l_buffer[32];
		sprintf( l_buffer, "/proc/%d/exe", getpid() );
		int l_bytes = std::min< std::size_t >( readlink( l_buffer, l_path, sizeof( l_path ) ), sizeof( l_path ) - 1 );

		if ( l_bytes > 0 )
		{
			l_path[l_bytes] = '\0';
			l_pathReturn = Path{ string::string_cast< xchar >( l_path ) };
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
		HRESULT l_hr = SHGetFolderPath( nullptr, CSIDL_PROFILE, nullptr, 0, l_path );

		if ( SUCCEEDED( l_hr ) )
		{
			l_pathReturn = Path{ l_path };
		}

#elif defined( __linux__ )

		struct passwd * l_pw = getpwuid( getuid() );
		const char * l_homedir = l_pw->pw_dir;
		l_pathReturn = Path{ string::string_cast< xchar >( l_homedir ) };

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

		if ( CheckFlag( p_flag, CreateMode::UserRead ) )
		{
			l_mode |= S_IRUSR;
		}

		if ( CheckFlag( p_flag, CreateMode::UserWrite ) )
		{
			l_mode |= S_IWUSR;
		}

		if ( CheckFlag( p_flag, CreateMode::UserExec ) )
		{
			l_mode |= S_IXUSR;
		}

		if ( CheckFlag( p_flag, CreateMode::GroupRead ) )
		{
			l_mode |= S_IRGRP;
		}

		if ( CheckFlag( p_flag, CreateMode::GroupWrite ) )
		{
			l_mode |= S_IWGRP;
		}

		if ( CheckFlag( p_flag, CreateMode::GroupExec ) )
		{
			l_mode |= S_IXGRP;
		}

		if ( CheckFlag( p_flag, CreateMode::OthersRead ) )
		{
			l_mode |= S_IROTH;
		}

		if ( CheckFlag( p_flag, CreateMode::OthersWrite ) )
		{
			l_mode |= S_IWOTH;
		}

		if ( CheckFlag( p_flag, CreateMode::OthersExec ) )
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
		Path l_file{ p_folder / p_file.GetFileName() + cuT( "." ) + p_file.GetExtension() };
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
