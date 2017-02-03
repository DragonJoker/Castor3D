#include "Config/PlatformConfig.hpp"

#if defined( CASTOR_PLATFORM_WINDOWS )

#include "Miscellaneous/Utils.hpp"
#include "Graphics/Size.hpp"

#include <Windows.h>
#include <codecvt>

namespace Castor
{
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
			BOOL bRet = ::EnumDisplayMonitors( nullptr, nullptr, MonitorEnum, WPARAM( &l_screen ) );
			return true;
		}

		Castor::String GetLastErrorText()
		{
			DWORD l_dwError = ::GetLastError();
			String l_strReturn = string::to_string( l_dwError );;

			if ( l_dwError != ERROR_SUCCESS )
			{
				LPWSTR l_szError = nullptr;

				if ( ::FormatMessageW( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, nullptr, l_dwError, 0, LPWSTR( &l_szError ), 0, nullptr ) != 0 )
				{
					std::wstring_convert< std::codecvt_utf8_utf16< wchar_t >, wchar_t > l_conversion;
					String l_converted = l_conversion.to_bytes( l_szError );
					l_strReturn += cuT( " (" ) + l_converted + cuT( ")" );
					string::replace( l_strReturn, cuT( "\r" ), cuT( "" ) );
					string::replace( l_strReturn, cuT( "\n" ), cuT( "" ) );
					::LocalFree( l_szError );
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
#	if defined( CASTOR_COMPILER_MSVC )

		localtime_s( p_tm, p_pTime );

#else

		p_tm = localtime( p_pTime );

#endif
	}
}

#endif
