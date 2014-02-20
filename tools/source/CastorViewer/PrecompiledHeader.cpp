#include "CastorViewer/PrecompiledHeader.hpp"

#if defined( VLD_AVAILABLE )
#	include <vld.h>
#elif defined( _WIN32 ) && !defined( NDEBUG )
#	define _CRTDBG_MAP_ALLOC
#	include <cstdlib>
#	include <crtdbg.h>
#endif
