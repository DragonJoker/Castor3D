#include "CastorUtils/Config/PlatformConfig.hpp"

#if defined( CU_PlatformApple )

#include "CastorUtils/Miscellaneous/Utils.hpp"
#include "CastorUtils/Graphics/Size.hpp"

extern void getDisplaySize( uint32_t * w, uint32_t * h );

namespace castor
{
	namespace system
	{
		bool getScreenSize( uint32_t p_screen, castor::Size & p_size )
		{
			uint32_t w, h;
			getDisplaySize( &w, &h );
			return true;
		}

		String getLastErrorText()
		{
			String result;
			int error = errno;
			char * szError = nullptr;

			if ( error != 0 && ( szError = strerror( error ) ) != nullptr )
			{
				result = string::toString( error ) + cuT( " (" ) + string::stringCast< xchar >( szError ) + cuT( ")" );
				string::replace( result, cuT( "\n" ), cuT( "" ) );
			}

			return result;
		}

		String getOSName()
		{
			char name[128];
			FILE * fp = popen( "sw_vers -productName", "r" );
			auto read = fread( name, 1, sizeof( name ) - 1, fp );

			if ( !read || read >= sizeof( name ) )
			{
				CU_Failure( "Couldn't retrieve OS name" );
			}

			pclose( fp );
			char version[128];
			fp = popen( "sw_vers -productVersion", "r" );
			read = fread( version, 1, sizeof( version ) - 1, fp );

			if ( !read || read >= sizeof( version ) )
			{
				CU_Failure( "Couldn't retrieve OS version" );
			}

			pclose( fp );
			std::stringstream stream;
			stream << name << " " << version;
			auto result = stream.str();
			return string::replace( result, "\n", "" );
		}
	}

	void getLocaltime( std::tm * tm, time_t const * pTime )
	{
		*tm = *localtime( pTime );
	}
}

#endif

