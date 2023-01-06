#include "CastorUtils/Config/PlatformConfig.hpp"

#if defined( CU_PlatformAndroid )

#include "CastorUtils/Graphics/Size.hpp"
#include "CastorUtils/Miscellaneous/Utils.hpp"

#pragma GCC diagnostic ignored "-Wpedantic"
#include <EGL/egl.h>

namespace castor
{
	namespace System
	{
		bool getScreenSize( uint32_t p_screen, castor::Size & p_size )
		{
			bool result = false;
			auto display = eglGetDisplay( EGLNativeDisplayType( intptr_t( p_screen ) ) );

			if ( !display )
			{
				Logger::logError( "Failed to open display." );
			}
			else
			{
				auto surface = eglGetCurrentSurface( EGL_READ );

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

		String getOSName()
		{
			return "Android";
		}
	}

	void getLocaltime( std::tm * p_tm, time_t const * p_pTime )
	{
		*p_tm = *localtime( p_pTime );
	}
}

#endif
