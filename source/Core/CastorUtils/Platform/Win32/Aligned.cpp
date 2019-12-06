#include "CastorUtils/Align/Aligned.hpp"

#if defined( CU_PlatformWindows )

#include <malloc.h>

namespace castor
{
	void * alignedAlloc( size_t alignment, size_t size )
	{
		return _aligned_malloc( alignment, size );
	}

	void alignedFree( void * memory )
	{
		_aligned_free( memory );
	}
}

#endif
