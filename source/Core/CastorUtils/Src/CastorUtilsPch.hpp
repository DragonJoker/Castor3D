/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
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

#	if defined( CASTOR_PLATFORM_WINDOWS )
#		if defined( CASTOR_COMPILER_MSVC )
#			include <tchar.h>
#		endif
#		include <windows.h>
#		include <direct.h>
#	else
#		include <sys/time.h>
#		if defined( CASTOR_PLATFORM_ANDROID ) || defined( CASTOR_PLATFORM_LINUX )
#			include <dlfcn.h>
#		endif
#	endif

#	if defined( CASTOR_COMPILER_GNUC )
#		include <sys/time.h>
#		include <errno.h>
#		include <iostream>
#		include <unistd.h>
#		include <cerrno>
#		if defined( CASTOR_PLATFORM_LINUX )
#			include <X11/Xlib.h>
#		endif
#	endif

extern "C"
{
#	if !defined( CASTOR_PLATFORM_ANDROID )
#		include <FreeImage.h>
#	endif
}

#endif

#endif
