#include "CastorUtils/Config/PlatformConfig.hpp"

#if defined( CU_PlatformAndroid )

#	include "CastorUtils/Config/MultiThreadConfig.hpp"
#	include "CastorUtils/Miscellaneous/MiscellaneousModule.hpp"
#	include "CastorUtils/Miscellaneous/StringUtils.hpp"

#	include <cstdlib>

namespace castor::debug
{
	namespace backtrace
	{
		template< typename CharT >
		void showBacktrace( std::basic_ostream< CharT > & stream, int toCapture, int toSkip )
		{
		}
	}

	void initialise()
	{
	}

	void cleanup()
	{
	}

	void loadModule( DynamicLibrary const & library )
	{
	}

	void unloadModule( DynamicLibrary const & library )
	{
	}

	OutputStream & operator<<( OutputStream & p_stream, Backtrace const & p_backtrace )
	{
		backtrace::showBacktrace( p_stream, p_backtrace.m_toCapture, p_backtrace.m_toSkip );
		return p_stream;
	}

}

#endif
