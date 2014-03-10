#include "CastorUtils/PrecompiledHeader.hpp"

#if defined( _MSC_VER )
#	pragma warning( push )
#	pragma warning( disable:4311 )
#	pragma warning( disable:4312 )
#endif

#include "CastorUtils/Point.hpp"

#if defined( __BORLANDC__)
#	pragma message( "Compiler : Borland C++")
#elif defined( __clang__)
#	pragma message( "Compiler : Clang")
#elif defined( __COMO__)
#	pragma message( "Compiler : Comeau C++")
#elif defined( __DECC)
#	pragma message( "Compiler : Compaq C/C++")
#elif defined( __CYGWIN32__)
#	pragma message( "Compiler : Cygwin")
#elif defined( __DCC__)
#	pragma message( "Compiler : Diab C/C++")
#elif defined( __DMC__)
#	pragma message( "Compiler : Digital Mars")
#elif defined( __SYSC_)
#	pragma message( "Compiler : Dignus Systems/C and Systems/C++")
#elif defined( __DJGPP__)
#	pragma message( "Compiler : DJGPP")
#elif defined( __PATHCC__)
#	pragma message( "Compiler : EKOPath")
#elif defined( __EDG__)
#	pragma message( "Compiler : EDG C++ Front End")
#elif defined( __GNUC__)
#	pragma message( "Compiler : GNU C/C++")
#elif defined( __ghs__)
#	pragma message( "Compiler : Green Hill C/C++")
#elif defined( __HP_aCC)
#	pragma message( "Compiler : HP aCC")
#elif defined( __IAR_SYSTEMS_ICC__)
#	pragma message( "Compiler : IAR C/C++")
#elif defined( __IBMCPP__)
#	pragma message( "Compiler : IBM XL C/C++")
#elif defined( __IAR_SYSTEMS_ICC__)
#	pragma message( "Compiler : IBM z/OS")
#elif defined( __INTEL_COMPILER) || defined( __ICC) || defined( __ECC) || defined( __ICL)
#	pragma message( "Compiler : Intel C/C++")
#elif defined( __KCC)
#	pragma message( "Compiler : KAI C++")
#elif defined( __CA__) || defined( __KEIL__)
#	pragma message( "Compiler : Keil CARM")
#elif defined( __C166__)
#	pragma message( "Compiler : Keil C166")
#elif defined( __C51__) || defined( __CX51__)
#	pragma message( "Compiler : Keil C51")
#elif defined( __LCC__)
#	pragma message( "Compiler : LCC")
#elif defined( __llvm__)
#	pragma message( "Compiler : LLVM")
#elif defined( __HIGHC__)
#	pragma message( "Compiler : MetaWare High C/C++")
#elif defined( __MWERKS__)
#	pragma message( "Compiler : Metrowerks CodeWarrior")
#elif defined( __MINGW32__)
#	pragma message( "Compiler : MinGW32")
#elif defined( __sgi) || defined( sgi)
#	pragma message( "Compiler : MIPSpro")
#elif defined( __MRC__)
#	pragma message( "Compiler : MPW C++")
#elif defined( _MSC_VER)
#	pragma message( "Compiler : Microsoft Visual C++")
#elif defined( _MRI)
#	pragma message( "Compiler : Microtec C/C++")
#elif defined( _PACC_VER)
#	pragma message( "Compiler : Palm C/C++")
#elif defined( __PGI)
#	pragma message( "Compiler : Portland Group C/C++")
#elif defined( __RENESAS__) || defined( __HITACHI__)
#	pragma message( "Compiler : Renesas C/C++")
#elif defined( _SCO_DS)
#	pragma message( "Compiler : SCO OpenServer")
#elif defined( __SUNPRO_C) || defined( __SUNPRO_CC)
#	pragma message( "Compiler : Sun Studio")
#elif defined( __TenDRA__)
#	pragma message( "Compiler : TenDRA C/C++")
#elif defined( _UCC)
#	pragma message( "Compiler : Ultimate C/C++")
#elif defined( __WATCOMC__)
#	pragma message( "Compiler : Watcom C++")
#else
#	pragma error( "Compiler : Unknown")
#endif

#if defined( _WIN64)
#	pragma message( "Target OS : Microsoft Windows 64bits")
#elif defined( _WIN32)
#	pragma message( "Target OS : Microsoft Windows 32bits")
#elif defined( __linux__) || defined( __linux) || defined( linux) || defined( __gnu_linux__)
#	pragma message( "Target OS : Linux")
#elif defined( __unix__) || defined( __unix) || defined( unix)
#	pragma message( "Target OS : Unix")
#elif defined( __APPLE__) || defined( __MACH__)
#	pragma message( "Target OS : Mac OS X")
#else
#	pragma message( "Target OS : Unknown")
#endif

#if defined( __i386__) || defined( __i386) || defined( i386)
#	pragma message( "Target architecture : IA-32")
#elif defined( __ia64__) || defined( __ia64) || defined( ia64)
#	pragma message( "Target architecture : Itanium")
#elif defined( __x86_64__) || defined( __x86_64)
#	pragma message( "Target architecture : Intel EM64T")
#endif

#if CASTOR_HAS_VARIADIC_TEMPLATES
#   pragma message( "variadic templates available")
#else
#   pragma message( "variadic templates not available")
#endif

#if CASTOR_HAS_NULLPTR
#   pragma message( "null_ptr available")
#else
#   pragma message( "null_ptr not available")
#endif

#if CASTOR_HAS_STDSMARTPTR
#   pragma message( "shared_ptr available from std")
#else
#   if CASTOR_HAS_TR1SMARTPTR
#       pragma message( "shared_ptr available from tr1")
#   else
#       pragma message( "shared_ptr available from boost")
#   endif
#endif

#if CASTOR_HAS_STDARRAY
#   pragma message( "array available from std")
#else
#   if CASTOR_HAS_TR1ARRAY
#       pragma message( "array available from tr1")
#   else
#       pragma message( "array available from boost")
#   endif
#endif

#if CASTOR_HAS_STDFUNCTIONAL
#   pragma message( "function available from std")
#else
#   if CASTOR_HAS_TR1FUNCTIONAL
#       pragma message( "function available from tr1")
#   else
#       pragma message( "function available from boost")
#   endif
#endif

#if CASTOR_HAS_DEFAULTED_FUNC
#   pragma message( "default function attributes available")
#else
#   pragma message( "default function attributes not available")
#endif

#if CASTOR_HAS_DELETED_FUNC
#   pragma message( "delete function attributes available")
#else
#   pragma message( "delete function attributes not available")
#endif

namespace Castor
{
	dummy_dtor g_dummyDtor;
}

extern "C" void tss_cleanup_implemented() { }

#if defined( _MSC_VER)
	bool Castor::FOpen( FILE *& p_pFile, char const * p_pszPath, char const * p_pszMode )
	{
		return fopen_s( &p_pFile, p_pszPath, p_pszMode ) == 0;
	}

	bool Castor::FOpen64( FILE *& p_pFile, char const * p_pszPath, char const * p_pszMode )
	{
		return fopen_s( &p_pFile, p_pszPath, p_pszMode ) == 0;
	}

	bool Castor::FSeek( FILE * p_pFile, int64_t p_i64Offset, int p_iOrigin )
	{
		return _fseeki64( p_pFile, p_i64Offset, p_iOrigin ) == 0;
	}

	int64_t Castor::FTell( FILE * p_pFile )
	{
		return _ftelli64( p_pFile );
	}

	void Castor::Localtime( std::tm *& p_tm, time_t const * p_pTime )
	{
		p_tm = localtime( p_pTime );
	}

	int Castor::StrnCpy( xchar * p_pszDst, std::size_t p_uiSize, xchar const * p_pszSrc )
	{
		return _tcscpy_s( p_pszDst, p_uiSize, p_pszSrc );
	}

	std::size_t Castor::StrLen( xchar const * p_pszStr )
	{
		return _tcslen( p_pszStr );
	}

	std::size_t Castor::StrFTime( xchar * p_pszBuf, std::size_t p_uiSize, xchar const * p_pszFormat, std::tm const * p_pTm )
	{
		return _tcsftime( p_pszBuf, p_uiSize, p_pszFormat, p_pTm );
	}
#else
	bool Castor::FOpen( FILE *& p_pFile, char const * p_pszPath, char const * p_pszMode )
	{
		p_pFile = fopen( p_pszPath, p_pszMode );
		return p_pFile != NULL;
	}

	void Castor::Localtime( std::tm *& p_tm, time_t const * p_pTime )
	{
		p_tm = localtime( p_pTime );
	}
#	if !defined( _WIN32 )
		bool Castor::FOpen64( FILE *& p_pFile, char const * p_pszPath, char const * p_pszMode )
		{
			p_pFile = fopen64( p_pszPath, p_pszMode );
			return p_pFile != NULL;
		}

		bool Castor::FSeek( FILE * p_pFile, int64_t p_i64Offset, int p_iOrigin )
		{
			return fseeko64( p_pFile, p_i64Offset, p_iOrigin ) == 0;
		}

		int64_t Castor::FTell( FILE * p_pFile )
		{
			return ftello64( p_pFile );
		}
#	else
		bool Castor::FOpen64( FILE *& p_pFile, char const * p_pszPath, char const * p_pszMode )
		{
			p_pFile = fopen( p_pszPath, p_pszMode );
			return p_pFile != NULL;
		}

		bool Castor::FSeek( FILE * p_pFile, int64_t p_i64Offset, int p_iOrigin )
		{
			return fseek( p_pFile, p_i64Offset, p_iOrigin ) == 0;
		}

		int64_t Castor::FTell( FILE * p_pFile )
		{
			return ftell( p_pFile );
		}
#	endif
#	if !defined( _UNICODE )
		int Castor::StrnCpy( xchar * p_pszDst, std::size_t p_uiSize, xchar const * p_pszSrc )
		{
			return strncpy( p_pszDst, p_uiSize, p_pszSrc );
		}

		std::size_t Castor::StrLen( xchar const * p_pszStr )
		{
			return strlen( p_pszStr );
		}

		std::size_t Castor::StrFTime( xchar * p_pszBuf, std::size_t p_uiSize, xchar const * p_pszFormat, std::tm const * p_pTm )
		{
			return strftime( p_pszBuf, p_uiSize, p_pszFormat, p_pTm );
		}
#	elif !defined( __GNUG__ )
		int Castor::StrnCpy( xchar * p_pszDst, std::size_t p_uiSize, xchar const * p_pszSrc )
		{
			return int( wcslen( wcsncpy( p_pszDst, p_pszSrc, p_uiSize ) ) );
		}

		std::size_t Castor::StrLen( xchar const * p_pszStr )
		{
			return wcslen( p_pszStr );
		}

		std::size_t Castor::StrFTime( xchar * p_pszBuf, std::size_t p_uiSize, xchar const * p_pszFormat, std::tm const * p_pTm )
		{
			return wcsftime( p_pszBuf, p_uiSize, p_pszFormat, p_pTm );
		}
#	else
#		include <cwchar>
		int Castor::StrnCpy( xchar * p_pszDst, std::size_t p_uiSize, xchar const * p_pszSrc )
		{
			return int( wcslen( wcsncpy( p_pszDst, p_pszSrc, p_uiSize ) ) );
		}

		std::size_t Castor::StrLen( xchar const * p_pszStr )
		{
			return wcslen( p_pszStr );
		}

		std::size_t Castor::StrFTime( xchar * p_pszBuf, std::size_t p_uiSize, xchar const * p_pszFormat, std::tm const * p_pTm )
		{
			return wcsftime( p_pszBuf, p_uiSize, p_pszFormat, p_pTm );
		}
#	endif
#endif

#if defined( _WIN32 )
#	include <Windows.h>
	namespace
	{
		BOOL CALLBACK MonitorEnum( _In_ HMONITOR hMonitor, _In_ HDC hdcMonitor, _In_ LPRECT lprcMonitor, _In_ LPARAM dwData )
		{
			Castor::Size * l_pSize = reinterpret_cast< Castor::Size* >( dwData );
			l_pSize->set( lprcMonitor->right - lprcMonitor->left, lprcMonitor->bottom - lprcMonitor->top );
			return FALSE;
		}
	}

	bool Castor::GetScreenSize( Castor::Size & p_size )
	{
		BOOL bRet = ::EnumDisplayMonitors( NULL, NULL, MonitorEnum, WPARAM( &p_size ) );
		return true;
	}

	Castor::String Castor::GetSystemError()
	{
		String l_strReturn;
		DWORD l_dwError = GetLastError();
		LPTSTR l_szError = NULL;

		if( l_dwError != ERROR_SUCCESS && ::FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, l_dwError, 0, LPTSTR( &l_szError ), 0, NULL ) != 0 )
		{
			l_strReturn = str_utils::to_string( l_dwError ) + cuT( " (" ) + l_szError + cuT( ")" );
			str_utils::replace( l_strReturn, cuT( "\r" ), cuT( "" ) );
			str_utils::replace( l_strReturn, cuT( "\n" ), cuT( "" ) );
			LocalFree( l_szError );
		}

		return l_strReturn;
	}
#elif defined( __linux__ )
#	include <X11/Xlib.h>
#	include <cerrno>

	bool Castor::GetScreenSize( Castor::Size & p_size )
	{
		bool l_bReturn = false;
		Display * pdsp = NULL;
		Screen * pscr = NULL;

		pdsp = XOpenDisplay( NULL );

		if ( !pdsp )
		{
			fprintf( stderr, "Failed to open default display.\n" );
		}
		else
		{
			pscr = DefaultScreenOfDisplay( pdsp );

			if ( !pscr )
			{
				fprintf( stderr, "Failed to obtain the default screen of given display.\n" );
			}
			else
			{
				p_size.set( pscr->width, pscr->height );
				l_bReturn = true;
			}

			XCloseDisplay( pdsp );
		}

		return l_bReturn;
	}

	Castor::String Castor::GetSystemError()
	{
		String l_strReturn;
		errno_t l_error = errno;
		char l_szError = NULL;

		if (l_error != 0 && (l_szError = strerror( l_error )) != NULL )
		{
			l_strReturn = str_utils::to_string( l_error ) + cuT( " (" ) + l_szError + cuT( ")" );
			str_utils::replace( l_strReturn, cuT( "\n" ), cuT( "" ) );
			LocalFree( l_szError );
		}

		return l_strReturn;
	}
#else
#	error "Yet unsupported OS"
#endif

#if defined( _MSC_VER )
#	pragma warning( pop )
#endif
