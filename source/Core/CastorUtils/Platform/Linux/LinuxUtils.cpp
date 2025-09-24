#include "CastorUtils/Config/PlatformConfig.hpp"

#if defined( CU_PlatformLinux )

#include "CastorUtils/Miscellaneous/Utils.hpp"
#include "CastorUtils/Miscellaneous/StringUtils.hpp"
#include "CastorUtils/Graphics/Size.hpp"

#include <X11/Xlib.h>
#if CU_HasXinerama
#	include <X11/extensions/Xinerama.h>
#endif

namespace c3d
{
	namespace system
	{
#	if CU_HasXinerama

		bool getScreenSize( uint32_t /*p_screen*/, Size & size )
		{
			bool result = false;

			if ( auto display = XOpenDisplay( nullptr );
				!display )
			{
				Logger::logError( "Failed to open default display." );
			}
			else
			{
				auto screenIndex = DefaultScreen( display );
				int dummy1{};
				int dummy2{};

				if ( XineramaQueryExtension( display, &dummy1, &dummy2 ) )
				{
					if ( XineramaIsActive( display ) )
					{
						int heads = 0;
						XineramaScreenInfo * screenInfo = XineramaQueryScreens( display, &heads );

						if ( heads > 0 && screenIndex < heads )
						{
							size.set( screenInfo[screenIndex].width, screenInfo[screenIndex].height );
							result = true;
						}
						else
						{
							std::cout << "XineramaQueryScreens says there aren't any" << std::endl;
						}

						XFree( screenInfo );
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

				if ( !result )
				{
					auto screen = ScreenOfDisplay( display, screenIndex );

					if ( !screen )
					{
						Logger::logError( "Failed to obtain the default screen of given display." );
					}
					else
					{
						size.set( screen->width, screen->height );
						result = true;
					}
				}

				XCloseDisplay( display );
			}

			return result;
		}

#	else

		bool getScreenSize( uint32_t /*p_screen*/, Size & size )
		{
			bool result = false;
			auto display = XOpenDisplay( nullptr );

			if ( !display )
			{
				Logger::logError( "Failed to open default display." );
			}
			else
			{
				auto screenIndex = DefaultScreen( display );
				auto screen = ScreenOfDisplay( display, screenIndex );

				if ( !screen )
				{
					Logger::logError( "Failed to obtain the default screen of given display." );
				}
				else
				{
					size.set( screen->width, screen->height );
					result = true;
				}
			}

			XCloseDisplay( display );
			return result;
		}

#	endif

		String getLastErrorText()
		{
			String result;
			int error = errno;

			if ( char const * szError = nullptr;
				error != 0 && ( szError = strerror( error ) ) != nullptr )
			{
				result = string::toString( error ) + cuT( " (" ) + makeString( szError ) + cuT( ")" );
				string::replace( result, cuT( "\n" ), cuT( "" ) );
			}

			return result;
		}

		String getOSName()
		{
			MbString line;
			std::ifstream finfo( "/etc/os-release" );
			MbString result;

			while ( std::getline( finfo, line ) )
			{
				if ( line.substr( 0, 11 ) == "PRETTY_NAME" )
				{
					result = line.substr( 13 );
					result = result.substr( 0, result.size() - 1u );
					break;
				}
			}

			return makeString( result );
		}
	}

	void getLocaltime( std::tm * p_tm, time_t const * p_pTime )
	{
		*p_tm = *localtime( p_pTime );
	}
}

#endif
