#include "Config/PlatformConfig.hpp"

#if defined( CASTOR_PLATFORM_ANDROID )

#include "Miscellaneous/Utils.hpp"
#include "Graphics/Size.hpp"

#pragma GCC diagnostic ignored "-Wpedantic"
#include <EGL/egl.h>

namespace Castor
{
	namespace System
	{
		bool GetScreenSize( uint32_t p_screen, Castor::Size & p_size )
		{
			bool l_return = false;
			auto l_display = eglGetDisplay( EGLNativeDisplayType( p_screen ) );

			if ( !l_display )
			{
				Logger::LogError( "Failed to open display." );
			}
			else
			{
				auto l_surface = eglGetCurrentSurface( EGL_READ );

				if ( !l_surface )
				{
					Logger::LogError( "Failed to open display's surface." );
				}
				else
				{
					int l_width;
					int l_height;
					eglQuerySurface( l_display, l_surface, EGL_WIDTH, &l_width );
					eglQuerySurface( l_display, l_surface, EGL_HEIGHT, &l_height );
					p_size.set( l_width, l_height );
					l_return = true;
				}
			}

			return l_return;
		}

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
