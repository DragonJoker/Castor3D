#include "CastorUtils/Config/PlatformConfig.hpp"

#if defined( CU_PlatformWindows )

#include "CastorUtils/Miscellaneous/Utils.hpp"
#include "CastorUtils/Graphics/Size.hpp"

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
			stSCREEN screen{ index, 0, size };
			BOOL bRet = ::EnumDisplayMonitors( nullptr, nullptr, MonitorEnum, LPARAM( &screen ) );
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
	}

	void getLocaltime( std::tm * p_tm, time_t const * p_pTime )
	{
#	if defined( CU_CompilerMSVC )

		localtime_s( p_tm, p_pTime );

#else

		*p_tm = *localtime( p_pTime );

#endif
	}
}

#endif
