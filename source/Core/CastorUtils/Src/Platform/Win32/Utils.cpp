#include "Config/PlatformConfig.hpp"

#if defined( CASTOR_PLATFORM_WINDOWS )

#include "Miscellaneous/Utils.hpp"
#include "Graphics/Size.hpp"

#include <Windows.h>
#include <codecvt>

namespace castor
{
	namespace System
	{
		namespace
		{
			struct stSCREEN
			{
				uint32_t m_wanted;
				uint32_t m_current;
				castor::Size & m_size;
			};

			BOOL CALLBACK MonitorEnum( HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData )
			{
				stSCREEN * screen = reinterpret_cast< stSCREEN * >( dwData );

				if ( screen && screen->m_current++ == screen->m_wanted )
				{
					screen->m_size.set( lprcMonitor->right - lprcMonitor->left, lprcMonitor->bottom - lprcMonitor->top );
				}

				return FALSE;
			}
		}

		bool getScreenSize( uint32_t p_screen, castor::Size & p_size )
		{
			stSCREEN screen = { p_screen, 0, p_size };
			BOOL bRet = ::EnumDisplayMonitors( nullptr, nullptr, MonitorEnum, WPARAM( &screen ) );
			return true;
		}

		castor::String getLastErrorText()
		{
			DWORD dwError = ::GetLastError();
			String strReturn = string::toString( dwError );;

			if ( dwError != ERROR_SUCCESS )
			{
				LPWSTR szError = nullptr;

				if ( ::FormatMessageW( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, nullptr, dwError, 0, LPWSTR( &szError ), 0, nullptr ) != 0 )
				{
					std::wstring_convert< std::codecvt_utf8_utf16< wchar_t >, wchar_t > conversion;
					String converted = conversion.to_bytes( szError );
					strReturn += cuT( " (" ) + converted + cuT( ")" );
					string::replace( strReturn, cuT( "\r" ), cuT( "" ) );
					string::replace( strReturn, cuT( "\n" ), cuT( "" ) );
					::LocalFree( szError );
				}
				else
				{
					strReturn += cuT( " (Unable to retrieve error text)" );
				}
			}
			else
			{
				strReturn += cuT( " (No error)" );
			}

			return strReturn;
		}
	}

	void getLocaltime( std::tm * p_tm, time_t const * p_pTime )
	{
#	if defined( CASTOR_COMPILER_MSVC )

		localtime_s( p_tm, p_pTime );

#else

		p_tm = localtime( p_pTime );

#endif
	}
}

#endif
