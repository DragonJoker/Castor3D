#include "Config/PlatformConfig.hpp"

#if defined( CASTOR_PLATFORM_WINDOWS )

#include "Align/Aligned.hpp"

#include "Log/Logger.hpp"
#include "Exception/Assertion.hpp"

#	include <malloc.h>
#	define CU_ALIGNED_ALLOC( m, a, s )\
	m = _aligned_malloc( s, a )
#	define CU_ALIGNED_FREE( m )\
	_aligned_free( m )

namespace Castor
{
	void * AlignedAlloc( size_t p_alignment, size_t p_size )
	{
		void * l_mem = nullptr;
		CU_ALIGNED_ALLOC( l_mem, p_alignment, p_size );
		return l_mem;
	}

	void AlignedFree( void * p_memory )
	{
		CU_ALIGNED_FREE( p_memory );
	}
}

#endif
