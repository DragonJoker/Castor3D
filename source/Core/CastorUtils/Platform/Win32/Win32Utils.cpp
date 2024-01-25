#include "CastorUtils/Config/PlatformConfig.hpp"

#if defined( CU_PlatformWindows )

#include "CastorUtils/Miscellaneous/Utils.hpp"
#include "CastorUtils/Graphics/Size.hpp"

#include <Windows.h>
#include <VersionHelpers.h>

namespace castor
{
	namespace system
	{
		namespace util
		{
			struct ScreenData
			{
				ScreenData( uint32_t wanted
					, uint32_t current
					, Size & size )
					: wanted{ wanted }
					, current{ current }
					, size{ size }
				{
				}

				uint32_t wanted;
				uint32_t current;
				Size & size;
				bool retrieved{};
			};
			using ScreenDataPtr = ScreenData *;

			BOOL CALLBACK MonitorEnum( CU_UnusedParam( HMONITOR, hMonitor )
				, CU_UnusedParam( HDC, hdcMonitor )
				, LPCRECT lprcMonitor
				, LPARAM dwData )
			{
				BOOL result{ TRUE };

				if ( auto screen = ScreenDataPtr( dwData ) )
				{
					if ( screen->current == screen->wanted )
					{
						screen->size.set( uint32_t( lprcMonitor->right - lprcMonitor->left )
							, uint32_t( lprcMonitor->bottom - lprcMonitor->top ) );
						screen->retrieved = true;
						result = FALSE;
					}

					++screen->current;
				}

				return result;
			}
		}

		bool getScreenSize( uint32_t index, Size & size )
		{
			util::ScreenData screen{ index, 0, size };
			BOOL bRet = ::EnumDisplayMonitors( nullptr, nullptr, MONITORENUMPROC( util::MonitorEnum ), LPARAM( &screen ) );
			return bRet != FALSE || screen.retrieved;
		}

		String getLastErrorText()
		{
			uint32_t errorCode = ::GetLastError();
			StringStream stream = makeStringStream();
			stream << cuT( "0x" ) << std::hex << errorCode;

			if ( errorCode != ERROR_SUCCESS )
			{
				if ( LPWSTR errorText = nullptr;
					::FormatMessageW( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM
						, nullptr
						, errorCode
						, 0
						, LPWSTR( &errorText )
						, 0
						, nullptr ) != 0 )
				{
					if constexpr ( std::is_same_v< xchar, char > )
					{
						if ( int length = WideCharToMultiByte( CP_UTF8, 0u, errorText, -1, nullptr, 0u, nullptr, nullptr );
							length > 0 )
						{
							MbString converted( size_t( length ), 0 );
							WideCharToMultiByte( CP_UTF8, 0u, errorText, -1, converted.data(), length, nullptr, nullptr );
							auto convResult = makeString( converted );
							string::replace( convResult, cuT( "\r" ), cuT( "" ) );
							string::replace( convResult, cuT( "\n" ), cuT( "" ) );
							stream << cuT( " (" ) << converted.c_str() << cuT( ")" );
						}
					}
					else if constexpr ( std::is_same_v< xchar, wchar_t > )
					{
						stream << cuT( " (" ) << makeString( errorText ) << cuT( ")" );
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

		static constexpr BYTE getLoByte( WORD w )
		{
			return ( ( BYTE )( ( ( DWORD_PTR )( w ) ) & 0xff ) );
		}

		static constexpr BYTE getHiByte( WORD w )
		{
			return ( ( BYTE )( ( ( ( DWORD_PTR )( w ) ) >> 8 ) & 0xff ) );
		}

		static WORD constexpr Win32WinNTWin11 = 0x0B00;
		static BYTE constexpr LoWin32WinNTWin11 = getLoByte( Win32WinNTWin11 );
		static BYTE constexpr HiWin32WinNTWin11 = getHiByte( Win32WinNTWin11 );

		BOOL IsWindows11OrGreater()
		{
			return IsWindowsVersionOrGreater( HiWin32WinNTWin11, LoWin32WinNTWin11, 0 );
		}

		String getOSName()
		{
			String result = cuT( "Windows" );

			if ( IsWindows11OrGreater() )
			{
				result += cuT( "11 or greater" );
			}
			else if ( IsWindows10OrGreater() )
			{
				result += cuT( " 10 or greater" );
			}
			else if ( IsWindows8Point1OrGreater() )
			{
				result += cuT( " 8.1" );
			}
			else if ( IsWindows8OrGreater() )
			{
				result += cuT( " 8" );
			}
			else if ( IsWindows7SP1OrGreater() )
			{
				result += cuT( " 7 SP 1" );
			}
			else if ( IsWindows7OrGreater() )
			{
				result += cuT( " 7" );
			}
			else if ( IsWindowsVistaSP2OrGreater() )
			{
				result += cuT( "Vista SP 2" );
			}
			else if ( IsWindowsVistaSP1OrGreater() )
			{
				result += cuT( "Vista SP 1" );
			}
			else if ( IsWindowsVistaOrGreater() )
			{
				result += cuT( "Vista" );
			}
			else if ( IsWindowsXPSP3OrGreater() )
			{
				result += cuT( "XP SP 3" );
			}
			else if ( IsWindowsXPSP2OrGreater() )
			{
				result += cuT( "XP SP 2" );
			}
			else if ( IsWindowsXPSP1OrGreater() )
			{
				result += cuT( "XP SP 1" );
			}
			else if ( IsWindowsXPOrGreater() )
			{
				result += cuT( "XP" );
			}

			if ( IsWindowsServer() )
			{
				result += cuT( " Server" );
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
