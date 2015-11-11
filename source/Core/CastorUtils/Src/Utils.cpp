#include "Utils.hpp"
#include "Size.hpp"

#if defined( _WIN32 )
#	include <Windows.h>
#endif

namespace Castor
{
	namespace System
	{
		void Sleep( uint32_t p_uiTime )
		{
			std::this_thread::sleep_for( std::chrono::milliseconds( p_uiTime ) );
		}
	}

#if defined( _WIN32 )
	namespace System
	{
		namespace
		{
			struct stSCREEN
			{
				uint32_t m_wanted;
				uint32_t m_current;
				Castor::Size & m_size;
			};

			BOOL CALLBACK MonitorEnum( HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData )
			{
				stSCREEN * l_screen = reinterpret_cast< stSCREEN * >( dwData );

				if ( l_screen && l_screen->m_current++ == l_screen->m_wanted )
				{
					l_screen->m_size.set( lprcMonitor->right - lprcMonitor->left, lprcMonitor->bottom - lprcMonitor->top );
				}

				return FALSE;
			}
		}

		bool GetScreenSize( uint32_t p_screen, Castor::Size & p_size )
		{
			stSCREEN l_screen = { p_screen, 0, p_size };
			BOOL bRet = ::EnumDisplayMonitors( NULL, NULL, MonitorEnum, WPARAM( &l_screen ) );
			return true;
		}

		Castor::String GetLastErrorText()
		{
			DWORD l_dwError = GetLastError();
			String l_strReturn = string::to_string( l_dwError );;

			if ( l_dwError != ERROR_SUCCESS )
			{
				LPTSTR l_szError = NULL;

				if ( ::FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, l_dwError, 0, LPTSTR( &l_szError ), 0, NULL ) != 0 )
				{
					l_strReturn += cuT( " (" ) + String( l_szError ) + cuT( ")" );
					string::replace( l_strReturn, cuT( "\r" ), cuT( "" ) );
					string::replace( l_strReturn, cuT( "\n" ), cuT( "" ) );
					LocalFree( l_szError );
				}
				else
				{
					l_strReturn += cuT( " (Unable to retrieve error text)" );
				}
			}
			else
			{
				l_strReturn += cuT( " (No error)" );
			}

			return l_strReturn;
		}

		uint8_t GetCPUCount()
		{
			SYSTEM_INFO sysinfo = { 0 };
			::GetSystemInfo( &sysinfo );
			return uint8_t( sysinfo.dwNumberOfProcessors );
		}
	}

	void Localtime( std::tm * p_tm, time_t const * p_pTime )
	{
#	if defined( _MSC_VER )
		localtime_s( p_tm, p_pTime );
#else
		p_tm = localtime( p_pTime );
#endif
	}
#elif defined( __linux__ )
	namespace System
	{
		bool GetScreenSize( uint32_t p_screen, Castor::Size & p_size )
		{
			bool l_return = false;
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
					l_return = true;
				}

				XCloseDisplay( pdsp );
			}

			return l_return;
		}

		String GetLastErrorText()
		{
			String l_strReturn;
			int l_error = errno;
			char * l_szError = NULL;

			if ( l_error != 0 && ( l_szError = strerror( l_error ) ) != NULL )
			{
				l_strReturn = string::to_string( l_error ) + cuT( " (" ) + string::string_cast< xchar >( l_szError ) + cuT( ")" );
				string::replace( l_strReturn, cuT( "\n" ), cuT( "" ) );
			}

			return l_strReturn;
		}

		uint8_t GetCPUCount()
		{
			char res[128];
			FILE * fp = popen( "/bin/cat /proc/cpuinfo |grep -c '^processor'", "r" );
			ENSURE( fread( res, 1, sizeof( res ) - 1, fp ) < sizeof( res ) );
			pclose( fp );
			return res[0];
		}
	}

	void Localtime( std::tm * p_tm, time_t const * p_pTime )
	{
		p_tm = localtime( p_pTime );
	}
#else
#	error "Yet unsupported OS"
#endif
}
