#include "CastorUtils/Config/PlatformConfig.hpp"

#if defined( CU_PlatformAndroid )

namespace c3d::debug
{
	namespace backtrace
	{
		void showBacktrace( OutputStream & stream, int toCapture, int toSkip )
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
}

#endif
