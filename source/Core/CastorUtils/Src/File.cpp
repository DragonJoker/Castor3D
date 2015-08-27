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
#	if defined( _MSC_VER )
#		pragma warning( push )
#		pragma warning( disable:4311 )
#		pragma warning( disable:4312 )
#	endif
#	include <windows.h>
#	if defined( _MSC_VER )
#		pragma warning( pop )
#	endif
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
			CASTOR_ASSERT( false );
			break;
		}

		if ( ( p_iMode & eOPEN_MODE_BINARY ) == 0 )
		{
			switch ( p_eEncoding )
			{
			case eENCODING_MODE_AUTO:
#ifdef _UNICODE
				m_eEncoding = eENCODING_MODE_UTF8;
				l_strMode += cuT( ", ccs=UTF-8" );
#else
				m_eEncoding = eENCODING_MODE_ASCII;
#endif
				break;

			case eENCODING_MODE_UTF8:
				l_strMode += cuT( ", ccs=UTF-8" );
				break;

			case eENCODING_MODE_UTF16:
				l_strMode += cuT( ", ccs=UTF-16LE" );
			}
		}

		FOpen( m_pFile, str_utils::to_str( m_strFileFullPath ).c_str(), str_utils::to_str( l_strMode ).c_str() );

		if ( m_pFile )
		{
			m_ullLength = 0;
			Castor::FSeek( m_pFile, 0, SEEK_END );
			m_ullLength = Castor::FTell( m_pFile );
			Castor::FSeek( m_pFile, 0, SEEK_SET );
		}
		else
		{
			CASTOR_EXCEPTION( str_utils::to_str( System::GetLastErrorText() ) );
		}

		CHECK_INVARIANTS();
	}

	File::File( File const & p_file )
		:	m_iMode( p_file.m_iMode	)
		,	m_eEncoding( p_file.m_eEncoding	)
		,	m_strFileFullPath( p_file.m_strFileFullPath	)
		,	m_ullCursor( p_file.m_ullCursor	)
		,	m_ullLength( p_file.m_ullLength	)
		,	m_pFile( p_file.m_pFile	)
		,	m_bOwnFile( false	)
	{
	}

	File::File( File && p_file )
		:	m_iMode( 0	)
		,	m_eEncoding( eENCODING_MODE_AUTO	)
		,	m_strFileFullPath(	)
		,	m_ullCursor( 0	)
		,	m_ullLength( 0	)
		,	m_pFile( NULL	)
		,	m_bOwnFile( true	)
	{
		m_iMode				= std::move( p_file.m_iMode	);
		m_eEncoding			= std::move( p_file.m_eEncoding	);
		m_strFileFullPath	= std::move( p_file.m_strFileFullPath	);
		m_ullCursor			= std::move( p_file.m_ullCursor	);
		m_ullLength			= std::move( p_file.m_ullLength	);
		m_pFile				= std::move( p_file.m_pFile	);
		m_bOwnFile			= std::move( p_file.m_bOwnFile	);
		p_file.m_iMode				= 0;
		p_file.m_eEncoding			= eENCODING_MODE_AUTO;
		p_file.m_strFileFullPath.clear();
		p_file.m_ullCursor			= 0;
		p_file.m_ullLength			= 0;
		p_file.m_pFile				= NULL;
		p_file.m_bOwnFile			= true;
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
		bool l_bReturn = false;

		if ( m_pFile )
		{
			if ( ferror( m_pFile ) == 0 )
			{
				if ( feof( m_pFile ) == 0 )
				{
					l_bReturn = true;
				}
			}
		}

		return l_bReturn;
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

	File & File::operator =( File const & p_file )
	{
		m_iMode				= p_file.m_iMode;
		m_eEncoding			= p_file.m_eEncoding;
		m_strFileFullPath	= p_file.m_strFileFullPath;
		m_ullCursor			= p_file.m_ullCursor;
		m_ullLength			= p_file.m_ullLength;
		m_pFile				= p_file.m_pFile;
		m_bOwnFile			= false;
		return * this;
	}

	File & File::operator =( File && p_file )
	{
		if ( this != & p_file )
		{
			m_iMode				= std::move( p_file.m_iMode	);
			m_eEncoding			= std::move( p_file.m_eEncoding	);
			m_strFileFullPath	= std::move( p_file.m_strFileFullPath	);
			m_ullCursor			= std::move( p_file.m_ullCursor	);
			m_ullLength			= std::move( p_file.m_ullLength	);
			m_pFile				= std::move( p_file.m_pFile	);
			m_bOwnFile			= std::move( p_file.m_bOwnFile	);
			p_file.m_iMode				= 0;
			p_file.m_eEncoding			= eENCODING_MODE_AUTO;
			p_file.m_strFileFullPath.clear();
			p_file.m_ullCursor			= 0;
			p_file.m_ullLength			= 0;
			p_file.m_pFile				= NULL;
			p_file.m_bOwnFile			= true;
		}

		return * this;
	}

	BEGIN_INVARIANT_BLOCK( File )
	CHECK_INVARIANT( m_pFile );
	END_INVARIANT_BLOCK()

	uint64_t File::DoWrite( uint8_t const * p_pBuffer, uint64_t p_uiSize )
	{
		CHECK_INVARIANTS();
		REQUIRE( IsOk() && ( m_iMode | eOPEN_MODE_WRITE ) );
		uint64_t l_uiReturn = 0;

		if ( IsOk() )
		{
			l_uiReturn = fwrite( p_pBuffer, 1, std::size_t( p_uiSize ), m_pFile );
			m_ullCursor += l_uiReturn;
			ENSURE( l_uiReturn <= p_uiSize );
		}

		CHECK_INVARIANTS();
		return l_uiReturn;
	}

	uint64_t File::DoRead( uint8_t * p_pBuffer, uint64_t p_uiSize )
	{
		CHECK_INVARIANTS();
		REQUIRE( IsOk() && ( m_iMode | eOPEN_MODE_READ ) );
		uint64_t l_uiReturn = 0;
		uint64_t l_uiPrev = 1;

		if ( IsOk() )
		{
			while ( l_uiReturn < p_uiSize && l_uiPrev != l_uiReturn )
			{
				l_uiPrev = l_uiReturn;
				l_uiReturn += fread( p_pBuffer, 1, std::size_t( p_uiSize - l_uiReturn ), m_pFile );
			}

			m_ullCursor += l_uiReturn;
			ENSURE( l_uiReturn <= p_uiSize );
		}

		CHECK_INVARIANTS();
		return l_uiReturn;
	}

	bool File::ListDirectoryFiles( Path const & p_folderPath, StringArray & p_files, bool CU_PARAM_UNUSED( p_recursive ) )
	{
		REQUIRE( ! p_folderPath.empty() );
		bool l_bReturn = false;
#if defined( _WIN32 )
		WIN32_FIND_DATA l_findData;
		HANDLE l_hHandle = ::FindFirstFile( ( p_folderPath / cuT( "*.*" ) ).c_str(), &l_findData );
		String l_strBuffer;

		if ( l_hHandle != INVALID_HANDLE_VALUE )
		{
			l_strBuffer = l_findData.cFileName;

			if ( l_strBuffer != cuT( "." ) && l_strBuffer != cuT( ".." ) )
			{
				p_files.push_back( l_strBuffer );
			}

			l_bReturn = true;

			while ( l_bReturn )
			{
				if ( ::FindNextFile( l_hHandle, &l_findData ) && l_findData.cFileName != l_strBuffer )
				{
					l_strBuffer = l_findData.cFileName;

					if ( l_strBuffer != cuT( "." ) && l_strBuffer != cuT( ".." ) )
					{
						p_files.push_back( p_folderPath / l_strBuffer );
					}
				}
				else
				{
					l_bReturn = false;
				}
			}

			l_bReturn = true;
		}

#elif defined( __linux__ )
		DIR * l_pDir;

		if ( ( l_pDir = opendir( str_utils::to_str( p_folderPath ).c_str() ) ) == NULL )
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

			l_bReturn = false;
		}
		else
		{
			dirent * l_pDirent;

			while ( ( l_pDirent = readdir( l_pDir ) ) != NULL )
			{
				if ( strcmp( l_pDirent->d_name, "." ) )
				{
					p_files.push_back( p_folderPath / str_utils::from_str( l_pDirent->d_name ) );
				}
			}

			closedir( l_pDir );
			l_bReturn = true;
		}

#else
#	error "Unsupported platform"
#endif
		return l_bReturn;
	}

	Path File::DirectoryGetCurrent()
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
		l_pathReturn = str_utils::from_str( l_homedir );

#else
#	error "Unsupported platform"
#endif
		return l_pathReturn;
	}

	bool File::DirectoryExists( Path const & p_path )
	{
#if defined( _WIN32 )
		struct _stat status = { 0 };
		_stat( str_utils::to_str( p_path ).c_str(), &status );
#else
		struct stat status = { 0 };
		stat( str_utils::to_str( p_path ).c_str(), &status );
#endif
		return ( status.st_mode & S_IFDIR ) == S_IFDIR;
	}

	bool File::DirectoryCreate( Path const & p_file )
	{
#if defined( _MSC_VER )
		return _tmkdir( p_file.c_str() ) == 0;
#elif defined( _WIN32 )
		return mkdir( str_utils::to_str( p_file ).c_str() ) == 0;
#else
		return mkdir( str_utils::to_str( p_file ).c_str(), 777 ) == 0;
#endif
	}

	bool File::FileExists( Path const & p_pathFile )
	{
		std::ifstream l_ifile( str_utils::to_str( p_pathFile ).c_str() );
		return l_ifile.is_open();
	}

	bool File::DeleteFile( Path const & p_file )
	{
		bool l_return = false;

		if ( FileExists( p_file ) )
		{
			l_return = std::remove( str_utils::to_str( p_file ).c_str() ) == 0;
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
		std::ifstream l_src( str_utils::to_str( p_file ), std::ios::binary );

		if ( l_src.is_open() )
		{
			std::ofstream l_dst( str_utils::to_str( l_file ), std::ios::binary );

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
