#include "Config/PlatformConfig.hpp"

#if defined( CASTOR_PLATFORM_LINUX )

#include "Aligned.hpp"

#include "Log/Logger.hpp"
#include "Exception/Assertion.hpp"

#	if GCC_VERSION >= 50000
#		ifndef _ISOC11_SOURCE
#			define _ISOC11_SOURCE
#		endif
#		include <cstdlib>
#		define CU_ALIGNED_ALLOC( m, a, s )\
	REQUIRE( ( s % a ) == 0 && cuT( "size is not an integral multiple of alignment" ) );\
	m = aligned_alloc( a, s )
#	else
#		include <cstdlib>
#		define CU_ALIGNED_ALLOC( m, a, s )\
	int l_error = posix_memalign( &m, a, s );\
	if ( l_error )\
	{\
		if ( l_error == EINVAL )\
		{\
			Logger::LogError( StringStream() << cuT( "Aligned allocation failed, alignment of " ) << a << cuT( " is not a power of two times sizeof( void * )" ) );\
		}\
		else if ( l_error == ENOMEM )\
		{\
			Logger::LogError( StringStream() << cuT( "Aligned allocation failed, no memory available" ) );\
		}\
		m = nullptr;\
	}
#	endif
#	define CU_ALIGNED_FREE( m )\
	free( m )

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
