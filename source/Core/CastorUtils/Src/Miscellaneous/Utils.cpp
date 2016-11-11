#include "Utils.hpp"
#include "Graphics/Size.hpp"

#if defined( _WIN32 )
#	include <Windows.h>
#	include <codecvt>
#else
#	include <X11/Xlib.h>
#	if CASTOR_HAS_XINERAMA
#		include <X11/extensions/Xinerama.h>
#	endif
#endif

#if defined( __GNUG__ )

#define cpuid( func, ax, bx, cx, dx )\
	__asm__ __volatile__ ( "cpuid":\
	"=a" (ax), "=b" (bx), "=c" (cx), "=d" (dx) : "a" (func));

#elif defined( _MSC_VER )

#define cpuid( func, a, b, c, d )\
	asm {\
		mov	eax, func\
		cpuid\
		mov	a, eax\
		mov	b, ebx\
		mov	c, ecx\
		mov	d, edx\
	}
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
#	if defined( _MSC_VER )

		localtime_s( p_tm, p_pTime );

#else

		p_tm = localtime( p_pTime );

#endif
	}

#elif defined( __linux__ )

	namespace System
	{
#	if CASTOR_HAS_XINERAMA

		bool GetScreenSize( uint32_t p_screen, Castor::Size & p_size )
		{
			bool l_return = false;
			auto l_display = XOpenDisplay( nullptr );

			if ( !l_display )
			{
				Logger::LogError( "Failed to open default display." );
			}
			else
			{
				auto l_screenIndex = DefaultScreen( l_display );
				int l_dummy1, l_dummy2;

				if ( XineramaQueryExtension( l_display, &l_dummy1, &l_dummy2 ) )
				{
					if ( XineramaIsActive( l_display ) )
					{
						int l_heads = 0;
						XineramaScreenInfo * l_screenInfo = XineramaQueryScreens( l_display, &l_heads );

						if ( l_heads > 0 && l_screenIndex < l_heads )
						{
							p_size.set( l_screenInfo[l_screenIndex].width, l_screenInfo[l_screenIndex].height );
							l_return = true;
						}
						else
						{
							std::cout << "XineramaQueryScreens says there aren't any" << std::endl;
						}

						XFree( l_screenInfo );
					}
					else
					{
						std::cout << "Xinerama not active" << std::endl;
					}
				}
				else
				{
					std::cout << "No Xinerama extension" << std::endl;
				}

				if ( !l_return )
				{
					auto l_screen = ScreenOfDisplay( l_display, l_screenIndex );

					if ( !l_screen )
					{
						Logger::LogError( "Failed to obtain the default screen of given display." );
					}
					else
					{
						p_size.set( l_screen->width, l_screen->height );
						l_return = true;
					}
				}

				XCloseDisplay( l_display );
			}

			return l_return;
		}

#	else

		bool GetScreenSize( uint32_t p_screen, Castor::Size & p_size )
		{
			bool l_return = false;
			auto l_display = XOpenDisplay( nullptr );

			if ( !l_display )
			{
				Logger::LogError( "Failed to open default display." );
			}
			else
			{
				auto l_screenIndex = DefaultScreen( l_display );
				auto l_screen = ScreenOfDisplay( l_display, l_screenIndex );

				if ( !l_screen )
				{
					Logger::LogError( "Failed to obtain the default screen of given display." );
				}
				else
				{
					p_size.set( l_screen->width, l_screen->height );
					l_return = true;
				}
			}

			XCloseDisplay( l_display );
			return l_return;
		}

#	endif

		String GetLastErrorText()
		{
			String l_strReturn;
			int l_error = errno;
			char * l_szError = nullptr;

			if ( l_error != 0 && ( l_szError = strerror( l_error ) ) != nullptr )
			{
				l_strReturn = string::to_string( l_error ) + cuT( " (" ) + string::string_cast< xchar >( l_szError ) + cuT( ")" );
				string::replace( l_strReturn, cuT( "\n" ), cuT( "" ) );
			}

			return l_strReturn;
		}

		uint8_t GetCPUCount()
		{
			struct ProcessFile
			{
				~ProcessFile()
				{
					pclose( m_file );
				}
				operator FILE * ()const
				{
					return m_file;
				}
				FILE * m_file;
			};

			char l_res[128];
			{
				ProcessFile l_file{ popen( "/bin/cat /proc/cpuinfo | grep -c '^processor'", "r" ) };
				ENSURE( fread( l_res, 1, sizeof( l_res ) - 1, l_file ) < sizeof( l_res ) );
			}
			return l_res[0];
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
