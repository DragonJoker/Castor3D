#include "CastorUtils/Config/PlatformConfig.hpp"

#if defined( CU_PlatformApple )

#include "CastorUtils/Align/Aligned.hpp"
#include "CastorUtils/Exception/Assertion.hpp"
#include "CastorUtils/Log/Logger.hpp"

#ifndef _ISOC11_SOURCE
#	define _ISOC11_SOURCE
#endif
#include <cstdlib>
#define CU_AlignedAlloc( m, a, s )\
	CU_Require( ( s % a ) == 0 && cuT( "size is not an integral multiple of alignment" ) );\
	m = aligned_alloc( a, s )
#define CU_AlignedFree( m )\
	free( m )

namespace castor
{
	void * alignedAlloc( size_t alignment, size_t size )
	{
		void * mem = nullptr;
		CU_AlignedAlloc( mem, alignment, size );
		return mem;
	}

	void alignedFree( void * memory )
	{
		CU_AlignedFree( memory );
	}
}

#endif
