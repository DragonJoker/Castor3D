/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___CASTOR_UTILS_PCH_H___
#define ___CASTOR_UTILS_PCH_H___

#include "config.hpp"

#if CASTOR_USE_PCH
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

#	if defined _WIN32
#		if defined( _MSC_VER )
#			include <tchar.h>
#		endif
#		include <windows.h>
#		include <direct.h>
#	else
#		include <sys/time.h>
#		if defined( __linux__ )
#			include <dlfcn.h>
#		endif
#	endif

#	if defined( __GNUG__ )
#		include <sys/time.h>
#		include <errno.h>
#		include <iostream>
#		include <unistd.h>
#		include <cerrno>
#		if !defined( _WIN32 )
#			include <X11/Xlib.h>
#		endif
#	endif

extern "C"
{
#	include <FreeImage.h>
}

#endif

#endif
