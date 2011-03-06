#ifndef ___CU_PrecompiledHeader___
#define ___CU_PrecompiledHeader___

#include <exception>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <math.h>
#include <set>
#include <sstream>
#include <stdarg.h>
#include <string>
#include <string.h>
#include <time.h>
#include <time.h>
#include <vector>

#include <sys/stat.h>
#include <boost/filesystem.hpp>
#include <sys/types.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/fttrigon.h>

#ifndef CHECK_MEMORYLEAKS
#	ifdef _DEBUG
#		define CHECK_MEMORYLEAKS 1
#	else
#		define CHECK_MEMORYLEAKS 0
#	endif
#	include "Memory.h"
#endif

#include "config.h"
#include "Value.h"

using namespace Castor::Templates;

#endif
