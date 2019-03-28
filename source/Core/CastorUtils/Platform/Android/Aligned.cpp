#include "CastorUtils/Config/PlatformConfig.hpp"

#if defined( CU_PlatformAndroid )

#	include "CastorUtils/Align/Aligned.hpp"
#	include "CastorUtils/Exception/Assertion.hpp"

#	include <cstdlib>

namespace castor
{
	void * alignedAlloc( size_t alignment, size_t size )
	{
		CU_Require( ( size % alignment ) == 0 && cuT( "size is not an integral multiple of alignment" ) );
		return malloc( size );
	}

	void alignedFree( void * memory )
	{
		free( memory );
	}
}

#endif
