#ifndef ___CU_PrecompiledHeader___
#define ___CU_PrecompiledHeader___

#	include <exception>
#	include <fstream>
#	include <iostream>
#	include <list>
#	include <map>
#	include <math.h>
#	include <set>
#	include <sstream>
#	include <stdarg.h>
#	include <string>
#	include <string.h>
#	include <time.h>
#	include <vector>

#	include "config.h"
#	include "Value.h"

using namespace Castor::Templates;

#	pragma message( "********************************************************************")
#	pragma message( "	CastorUtils")

#ifndef CHECK_MEMORYLEAKS
#	ifdef _DEBUG
#		define CHECK_MEMORYLEAKS 1
#		pragma message( "		Checking Memory leaks")
#		include "Memory.h"
#	else
#		define CHECK_MEMORYLEAKS 0
#	endif
#endif

#if CASTOR_USE_DOUBLE
#	pragma message( "		'real' is defined as double")
#else
#	pragma message( "		'real' is defined as float")
#endif

#if CASTOR_USE_MT_CONTAINERS
#	pragma message( "		using multithreaded containers instead of std")
#else
#	pragma message( "		using std containers instead of multithreaded")
#endif

#if CASTOR_USE_MULTITHREADING
#	pragma message( "		multi threaded dll")
#else
#	pragma message( "		single threaded dll")
#endif

#if CASTOR_USE_FILE_STREAM
#	pragma message( "		Using C++ stream files")
#else
#	pragma message( "		Using C file io")
#endif

#pragma message( "********************************************************************")

#endif