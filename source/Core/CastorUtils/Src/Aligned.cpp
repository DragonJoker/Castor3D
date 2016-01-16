#include "Aligned.hpp"

#if defined( _MSC_VER )
#	include <malloc.h>
#	define CU_ALIGNED_FREE( m )\
	_aligned_free( m )
#	define CU_ALIGNED_ALLOC( m, a, s )\
	m = _aligned_malloc( s, a )
#else
#	include <stdlib.h>
#	define CU_ALIGNED_FREE( m )\
	free( m )
#	define CU_ALIGNED_ALLOC( m, a, s )\
	m = aligned_alloc( a, s )
#	if defined( __GNUG__ )
#		include <sys/time.h>
#		include <errno.h>
#		include <iostream>
#		include <unistd.h>
#		include <cerrno>
#		if !defined( _WIN32 )
#			include <X11/Xlib.h>
#		endif
#	endif
#endif

namespace Castor
{
	void * AlignedAlloc( size_t p_alignment, size_t p_size )
	{
		void * l_mem = NULL;
		CU_ALIGNED_ALLOC( l_mem, p_alignment, p_size );
		return l_mem;
	}

	void AlignedFree( void * p_memory )
	{
		CU_ALIGNED_FREE( p_memory );
	}
}
