#include "Config/PlatformConfig.hpp"

#if defined( CU_PlatformWindows )

#include "Align/Aligned.hpp"

#include "Log/Logger.hpp"
#include "Exception/Assertion.hpp"

#	include <malloc.h>
#	define CU_AlignedAlloc( m, a, s )\
	m = _aligned_malloc( s, a )
#	define CU_AlignedFree( m )\
	_aligned_free( m )

namespace castor
{
	void * alignedAlloc( size_t alignment, size_t size )
	{
		void * mem;
		CU_AlignedAlloc( mem, alignment, size );
		return mem;
	}

	void alignedFree( void * memory )
	{
		CU_AlignedFree( memory );
	}
}

#endif
