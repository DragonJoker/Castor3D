#include "CastorDvpTDPrerequisites.hpp"

#if defined( CASTOR_PLATFORM_WINDOWS  ) && !defined( NDEBUG ) && !defined( VLD_AVAILABLE )
#	define _CRTDBG_MAP_ALLOC
#	include <cstdlib>
#	include <crtdbg.h>
#elif defined( CASTOR_PLATFORM_LINUX )
#	include <gdk/gdkx.h>
#	include <gtk/gtk.h>
#endif

#include <Miscellaneous/PlatformWindowHandle.hpp>
#include <Engine.hpp>

namespace castortd
{
}
