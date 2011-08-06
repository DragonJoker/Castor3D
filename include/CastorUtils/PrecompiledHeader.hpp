#ifndef ___CU_PrecompiledHeader___
#define ___CU_PrecompiledHeader___

#include <exception>
#include <fstream>
#include <iostream>
#include <list>
#include <locale>
#include <map>
#include <cmath>
#include <set>
#include <sstream>
#include <cstdarg>
#include <string>
#include <cstring>
#include <ctime>
#include <vector>

#include <sys/stat.h>
#include <boost/filesystem.hpp>
#include <sys/types.h>
#include <cstdio>
#include <cstdlib>

#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/fttrigon.h>

#ifndef CHECK_MEMORYLEAKS
#	ifndef NDEBUG
#		define CHECK_MEMORYLEAKS 1
#	else
#		define CHECK_MEMORYLEAKS 0
#	endif
#	include "Memory.hpp"
#endif

#include "config.hpp"
#include "Value.hpp"

using namespace Castor::Templates;
#endif
