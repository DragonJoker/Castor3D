#include "Config/PlatformConfig.hpp"

#if defined( CASTOR_PLATFORM_LINUX )

#include "Miscellaneous/Utils.hpp"
#include "Graphics/Size.hpp"

#include <X11/Xlib.h>
#if CASTOR_HAS_XINERAMA
#	include <X11/extensions/Xinerama.h>
#endif

#define cpuid( func, ax, bx, cx, dx )\
	__asm__ __volatile__ ( "cpuid":\
	"=a" (ax), "=b" (bx), "=c" (cx), "=d" (dx) : "a" (func));

namespace Castor
{
	namespace System
	{
#	if CASTOR_HAS_XINERAMA

		bool GetScreenSize( uint32_t p_screen, Castor::Size & p_size )
		{
			bool l_result = false;
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
							l_result = true;
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

				if ( !l_result )
				{
					auto l_screen = ScreenOfDisplay( l_display, l_screenIndex );

					if ( !l_screen )
					{
						Logger::LogError( "Failed to obtain the default screen of given display." );
					}
					else
					{
						p_size.set( l_screen->width, l_screen->height );
						l_result = true;
					}
				}

				XCloseDisplay( l_display );
			}

			return l_result;
		}

#	else

		bool GetScreenSize( uint32_t p_screen, Castor::Size & p_size )
		{
			bool l_result = false;
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
					l_result = true;
				}
			}

			XCloseDisplay( l_display );
			return l_result;
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
}

#endif

