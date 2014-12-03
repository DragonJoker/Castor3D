#include "PrecompiledHeader.hpp"

#if defined( VLD_AVAILABLE )
#	include <vld.h>
#elif defined( _MSC_VER ) && !defined( NDEBUG )
#	define _CRTDBG_MAP_ALLOC
#	include <cstdlib>
#	include <crtdbg.h>
#endif
