#include "Config/PlatformConfig.hpp"

#if defined( CASTOR_PLATFORM_ANDROID )

#include "Miscellaneous/Utils.hpp"
#include "Graphics/Size.hpp"

#pragma GCC diagnostic ignored "-Wpedantic"
#include <EGL/egl.h>

namespace castor
{
	namespace System
	{
		bool getScreenSize( uint32_t p_screen, castor::Size & p_size )
		{
			bool result = false;
			auto display = eglgetDisplay( EGLNativeDisplayType( p_screen ) );

			if ( !display )
			{
				Logger::logError( "Failed to open display." );
			}
			else
			{
				auto surface = eglgetCurrentSurface( EGL_READ );

				if ( !surface )
				{
					Logger::logError( "Failed to open display's surface." );
				}
				else
				{
					int width;
					int height;
					eglQuerySurface( display, surface, EGL_WIDTH, &width );
					eglQuerySurface( display, surface, EGL_HEIGHT, &height );
					p_size.set( width, height );
					result = true;
				}
			}

			return result;
		}

		String getLastErrorText()
		{
			String strReturn;
			int error = errno;
			char * szError = nullptr;

			if ( error != 0 && ( szError = strerror( error ) ) != nullptr )
			{
				strReturn = string::toString( error ) + cuT( " (" ) + string::stringCast< xchar >( szError ) + cuT( ")" );
				string::replace( strReturn, cuT( "\n" ), cuT( "" ) );
			}

			return strReturn;
		}
	}

	void getLocaltime( std::tm *& p_tm, time_t const * p_pTime )
	{
		p_tm = localtime( p_pTime );
	}
}

#endif
