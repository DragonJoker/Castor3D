#include "CastorUtils/Config/PlatformConfig.hpp"

#if defined( CU_PlatformWindows )

#include "CastorUtils/Miscellaneous/Utils.hpp"
#include "CastorUtils/Graphics/Size.hpp"

#include <Windows.h>
#include <VersionHelpers.h>
#include <codecvt>

namespace castor
{
	namespace System
	{
		namespace util
		{
			struct stSCREEN
			{
				stSCREEN( uint32_t wanted
					, uint32_t current
					, castor::Size & size )
					: wanted{ wanted }
					, current{ current }
					, size{ size }
				{
				}

				uint32_t wanted;
				uint32_t current;
				castor::Size & size;
			};

			BOOL CALLBACK MonitorEnum( HMONITOR CU_UnusedParam( hMonitor )
				, HDC CU_UnusedParam( hdcMonitor )
				, LPRECT lprcMonitor
				, LPARAM dwData )
			{
				stSCREEN * screen = reinterpret_cast< stSCREEN * >( dwData );

				if ( screen && screen->current++ == screen->wanted )
				{
					screen->size.set( uint32_t( lprcMonitor->right - lprcMonitor->left )
						, uint32_t( lprcMonitor->bottom - lprcMonitor->top ) );
				}

				return FALSE;
			}
		}

		bool getScreenSize( uint32_t index, Size & size )
		{
			util::stSCREEN screen{ index, 0, size };
			BOOL bRet = ::EnumDisplayMonitors( nullptr, nullptr, util::MonitorEnum, LPARAM( &screen ) );
			return bRet != FALSE;
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
						String converted( size_t( length ), 0 );
						WideCharToMultiByte( CP_UTF8, 0u, errorText, -1, converted.data(), length, 0u, 0u );
						string::replace( converted, "\r", String{} );
						string::replace( converted, "\n", String{} );
						stream << cuT( " (" ) << converted.c_str() << cuT( ")" );
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

#define _WIN32_WINNT_WIN11                  0x0B00

		BOOL IsWindows11OrGreater()
		{
			return IsWindowsVersionOrGreater( HIBYTE( _WIN32_WINNT_WIN11 ), LOBYTE( _WIN32_WINNT_WIN11 ), 0 );
		}

		String getOSName()
		{
			String result = cuT( "Windows" );

			if ( IsWindows11OrGreater() )
			{
				result += " 11 or greater";
			}
			else if ( IsWindows10OrGreater() )
			{
				result += " 10 or greater";
			}
			else if ( IsWindows8Point1OrGreater() )
			{
				result += " 8.1";
			}
			else if ( IsWindows8OrGreater() )
			{
				result += " 8";
			}
			else if ( IsWindows7SP1OrGreater() )
			{
				result += " 7 SP 1";
			}
			else if ( IsWindows7OrGreater() )
			{
				result += " 7";
			}
			else if ( IsWindowsVistaSP2OrGreater() )
			{
				result += "Vista SP 2";
			}
			else if ( IsWindowsVistaSP1OrGreater() )
			{
				result += "Vista SP 1";
			}
			else if ( IsWindowsVistaOrGreater() )
			{
				result += "Vista";
			}
			else if ( IsWindowsXPSP3OrGreater() )
			{
				result += "XP SP 3";
			}
			else if ( IsWindowsXPSP2OrGreater() )
			{
				result += "XP SP 2";
			}
			else if ( IsWindowsXPSP1OrGreater() )
			{
				result += "XP SP 1";
			}
			else if ( IsWindowsXPOrGreater() )
			{
				result += "XP";
			}

			if ( IsWindowsServer() )
			{
				result += " Server";
			}

			return result;
		}
	}

	void getLocaltime( std::tm * tm, time_t const * pTime )
	{
#if defined( CU_CompilerMSVC )

		localtime_s( tm, pTime );

#else

		*tm = *localtime( pTime );

#endif
	}
}

#endif
