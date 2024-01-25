#include "CastorUtils/Config/PlatformConfig.hpp"

#if defined( CU_PlatformApple )

#include "CastorUtils/Miscellaneous/Utils.hpp"
#include "CastorUtils/Graphics/Size.hpp"

extern void getDisplaySize( uint32_t * w, uint32_t * h );

namespace castor
{
	namespace system
	{
		bool getScreenSize( uint32_t p_screen, Size & p_size )
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
				result = string::toString( error ) + cuT( " (" ) + makeString( szError ) + cuT( ")" );
				string::replace( result, cuT( "\n" ), cuT( "" ) );
			}

			return result;
		}

		String getOSName()
		{
			Array< char, 128u > name;
			FILE * fp = popen( "sw_vers -productName", "r" );
			auto nameRead = fread( name.data(), 1, name.size() - 1, fp );

			if ( !nameRead || nameRead >= name.size() )
			{
				CU_Failure( "Couldn't retrieve OS name" );
			}

			pclose( fp );
			Array< char, 128u > version;
			fp = popen( "sw_vers -productVersion", "r" );
			auto versionRead = fread( version.data(), 1, version.size() - 1, fp );

			if ( !versionRead || versionRead >= version.size() )
			{
				CU_Failure( "Couldn't retrieve OS version" );
			}

			pclose( fp );
			std::stringstream stream;
			stream << MbStringView{ name.data(), nameRead } << " " << MbStringView{ version.data(), versionRead };
			auto result = stream.str();
			return makeString( string::replace( result, "\n", "" ) );
		}
	}

	void getLocaltime( std::tm * tm, time_t const * pTime )
	{
		*tm = *localtime( pTime );
	}
}

#endif

