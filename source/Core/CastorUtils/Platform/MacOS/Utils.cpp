#include "CastorUtils/Config/PlatformConfig.hpp"

#if defined( CU_PlatformApple )

#include "CastorUtils/Miscellaneous/Utils.hpp"
#include "CastorUtils/Graphics/Size.hpp"

namespace castor
{
	namespace System
	{
		bool getScreenSize( uint32_t p_screen, castor::Size & p_size )
		{
			// auto id = CGMainDisplayID();
			// auto w = CGDisplayPixelsWide( id );
			// auto h = CGDisplayPixelsHigh( id );
			// p_size = Size{ uint32_t( w), uint32_t( h ) };
			return false;
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

	void getLocaltime( std::tm * p_tm, time_t const * p_pTime )
	{
		*p_tm = *localtime( p_pTime );
	}
}

#endif

