#include "CastorDvpTDPrerequisites.hpp"

#if defined( CU_PlatformWindows  ) && !defined( NDEBUG ) && !defined( VLD_AVAILABLE )
#	define _CRTDBG_MAP_ALLOC
#	include <cstdlib>
#	include <crtdbg.h>
#elif defined( CU_PlatformLinux )
#	pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreserved-identifier"
#	pragma GCC diagnostic push
#	pragma GCC diagnostic ignored "-Wold-style-cast"
#	pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#	include <gdk/gdkx.h>
#	include <gtk/gtk.h>
#	pragma GCC diagnostic pop
#	pragma clang diagnostic pop
#endif

#include <Castor3D/Engine.hpp>
