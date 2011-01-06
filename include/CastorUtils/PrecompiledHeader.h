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

#pragma message( "********************************************************************")
#pragma message( "	CastorUtils")

#ifndef CHECK_MEMORYLEAKS
#	ifdef _DEBUG
#		define CHECK_MEMORYLEAKS 1
#		pragma message( "		Checking Memory leaks")
#	else
#		define CHECK_MEMORYLEAKS 0
#	endif
#	include "Memory.h"
#endif

#ifndef _UNICODE
#	pragma message( "		Multibyte strings")
#else
#	pragma message( "		Unicode strings")
#endif

#include "config.h"
#include "Value.h"

using namespace Castor::Templates;

#if CASTOR_USE_DOUBLE
#	pragma message( "		Type 'real' is defined as double")
#else
#	pragma message( "		Type 'real' is defined as float")
#endif

#if CASTOR_USE_MT_CONTAINERS
#	pragma message( "		Using multithreaded containers instead of std")
#else
#	pragma message( "		Using std containers instead of multithreaded")
#endif

#if CASTOR_USE_MULTITHREADING
#	pragma message( "		Multi threaded dll")
#else
#	pragma message( "		Single threaded dll")
#endif

#pragma message( "********************************************************************")

#endif