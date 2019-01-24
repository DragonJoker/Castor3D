/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_UTILS_PCH_H___
#define ___CASTOR_UTILS_PCH_H___

#include "config.hpp"

#if CU_UsePCH
#	include "CastorUtilsPrerequisites.hpp"

#	include <algorithm>
#	include <fstream>
#	include <functional>
#	include <iostream>
#	include <limits>
#	include <list>
#	include <map>
#	include <numeric>
#	include <set>
#	include <sstream>
#	include <stack>
#	include <streambuf>
#	include <string>
#	include <vector>

#	include <cassert>
#	include <cmath>
#	include <cstdint>
#	include <cstdlib>
#	include <cstdarg>
#	include <cstring>
#	include <ctime>
#	include <cstdio>

#	include <sys/types.h>
#	include <sys/stat.h>
#	include <wchar.h>

#	include <array>

#	include <condition_variable>
#	include <mutex>
#	include <thread>

#	include <ft2build.h>

FT_BEGIN_HEADER
#	include FT_FREETYPE_H
#	include FT_GLYPH_H
#	include FT_OUTLINE_H
#	include FT_TRIGONOMETRY_H
#	include FT_TYPES_H
FT_END_HEADER

#	if defined( CU_PlatformWindows )
#		if defined( CU_CompilerMSVC )
#			include <tchar.h>
#		endif
#		include <windows.h>
#		include <direct.h>
#	else
#		include <sys/time.h>
#		if defined( CU_PlatformAndroid ) || defined( CU_PlatformLinux )
#			include <dlfcn.h>
#		endif
#	endif

#	if defined( CU_CompilerGNUC )
#		include <sys/time.h>
#		include <errno.h>
#		include <iostream>
#		include <unistd.h>
#		include <cerrno>
#		if defined( CU_PlatformLinux )
#			include <X11/Xlib.h>
#		endif
#	endif

extern "C"
{
#	if !defined( CU_PlatformAndroid )
#		include <FreeImage.h>
#	endif
}

#endif

#endif
