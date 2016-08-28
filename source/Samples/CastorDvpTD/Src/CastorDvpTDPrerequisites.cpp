#include "CastorDvpTDPrerequisites.hpp"

#if defined( _MSC_VER ) && !defined( NDEBUG ) && !defined( VLD_AVAILABLE )
#	define _CRTDBG_MAP_ALLOC
#	include <cstdlib>
#	include <crtdbg.h>
#elif defined( __linux__ )
#	include <gdk/gdkx.h>
#	include <gtk/gtk.h>
#endif

#include <Miscellaneous/PlatformWindowHandle.hpp>
#include <Engine.hpp>

namespace castortd
{
}
