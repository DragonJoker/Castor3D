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
			bool result = false;
			auto display = eglGetDisplay( EGLNativeDisplayType( p_screen ) );

			if ( !display )
			{
				Logger::LogError( "Failed to open display." );
			}
			else
			{
				auto surface = eglGetCurrentSurface( EGL_READ );

				if ( !surface )
				{
					Logger::LogError( "Failed to open display's surface." );
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

		String GetLastErrorText()
		{
			String strReturn;
			int error = errno;
			char * szError = nullptr;

			if ( error != 0 && ( szError = strerror( error ) ) != nullptr )
			{
				strReturn = string::to_string( error ) + cuT( " (" ) + string::string_cast< xchar >( szError ) + cuT( ")" );
				string::replace( strReturn, cuT( "\n" ), cuT( "" ) );
			}

			return strReturn;
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

			char res[128];
			{
				ProcessFile file{ popen( "/bin/cat /proc/cpuinfo | grep -c '^processor'", "r" ) };
				ENSURE( fread( res, 1, sizeof( res ) - 1, file ) < sizeof( res ) );
			}

			return res[0];
		}
	}

	void Localtime( std::tm * p_tm, time_t const * p_pTime )
	{
		p_tm = localtime( p_pTime );
	}
}

#endif
