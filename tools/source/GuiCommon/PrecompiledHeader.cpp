#include "GuiCommon/PrecompiledHeader.hpp"

#if defined( _WIN32 ) && !defined( NDEBUG ) && !defined( VLD_AVAILABLE )
#	define _CRTDBG_MAP_ALLOC
#	include <cstdlib>
#	include <crtdbg.h>
#endif