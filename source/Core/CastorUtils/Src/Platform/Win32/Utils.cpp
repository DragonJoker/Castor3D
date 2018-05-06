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

		bool getScreenSize( uint32_t p_screen, Size & p_size )
		{
			stSCREEN screen = { p_screen, 0, p_size };
			BOOL bRet = ::EnumDisplayMonitors( nullptr, nullptr, MonitorEnum, WPARAM( &screen ) );
			return true;
		}

		String getLastErrorText()
		{
			uint32_t errorCode = ::GetLastError();
			StringStream stream = makeStringStream();
			stream << cuT( "0x" ) << std::hex << errorCode;

			if ( errorCode != ERROR_SUCCESS )
			{
				LPWSTR errorText = nullptr;

				if ( ::FormatMessageW( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM
					, nullptr
					, errorCode
					, 0
					, LPWSTR( &errorText )
					, 0
					, nullptr ) != 0 )
				{
					int length = WideCharToMultiByte( CP_UTF8, 0u, errorText, -1, nullptr, 0u, 0u, 0u );

					if ( length > 0 )
					{
						std::vector< char > buffer( size_t( length + 1 ), char{} );
						WideCharToMultiByte( CP_UTF8, 0u, errorText, -1, buffer.data(), length, 0u, 0u );
						String converted{ buffer.begin(), buffer.end() };
						stream << cuT( " (" ) << converted << cuT( ")" );
					}
				}
				else
				{
					stream << cuT( " (Unable to retrieve error text)" );
				}
			}
			else
			{
				stream << cuT( " (No error)" );
			}

			auto result = stream.str();
			string::replace( result, cuT( "\r" ), cuT( "" ) );
			string::replace( result, cuT( "\n" ), cuT( "" ) );
			return result;
		}
	}

	void getLocaltime( std::tm * p_tm, time_t const * p_pTime )
	{
#	if defined( CASTOR_COMPILER_MSVC )

		localtime_s( p_tm, p_pTime );

#else

		*p_tm = *localtime( p_pTime );

#endif
	}
}

#endif
