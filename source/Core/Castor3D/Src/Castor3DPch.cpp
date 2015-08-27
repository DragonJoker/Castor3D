#include "Castor3DPch.hpp"

#pragma warning( disable:4311 )
#pragma warning( disable:4312 )

#include <Point.hpp>
#include <SquareMatrix.hpp>

#if defined( VLD_AVAILABLE )
#	include <vld.h>
#elif defined( _MSC_VER )
#	define _CRTDBG_MAP_ALLOC
#	include <cstdlib>
#	include <crtdbg.h>
#endif
