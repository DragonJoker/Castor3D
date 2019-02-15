#include "CastorDvpTDPrerequisites.hpp"

#if defined( CU_PlatformWindows  ) && !defined( NDEBUG ) && !defined( VLD_AVAILABLE )
#	define _CRTDBG_MAP_ALLOC
#	include <cstdlib>
#	include <crtdbg.h>
#elif defined( CU_PlatformLinux )
#	include <gdk/gdkx.h>
#	include <gtk/gtk.h>
#endif

#include <Engine.hpp>

namespace castortd
{
}
