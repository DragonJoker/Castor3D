/*
This source file is part of Castor3D (http://dragonjoker.co.cc

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___C3D_PRECOMPILED_HEADER_H___
#define ___C3D_PRECOMPILED_HEADER_H___

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <algorithm>
#include <math.h>
#include <list>
#include <stdarg.h>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <assert.h>

#ifndef WIN32
#	define sprintf_s snprintf
#	define Sleep usleep
#endif

#include <CastorUtils/STLMacros.h>
#include <CastorUtils/STLMapMacros.h>
#include <CastorUtils/STLSetMacros.h>
#include <CastorUtils/MinMax.h>
#include <CastorUtils/Resource.h>
#include <CastorUtils/ResourceLoader.h>
#include <CastorUtils/Image.h>
#include <CastorUtils/File.h>
#include <CastorUtils/Manager.h>
#include <CastorUtils/AutoSingleton.h>
#include <CastorUtils/PreciseTimer.h>
#include <CastorUtils/Mutex.h>
#include <CastorUtils/Thread.h>
#include <CastorUtils/NonCopyable.h>
#include <CastorUtils/Buffer.h>
#include <CastorUtils/Module_Image.h>
#include <CastorUtils/DynamicLibrary.h>
#include <CastorUtils/Vector3f.h>
#include <CastorUtils/Quaternion.h>
#include <CastorUtils/Container.h>
#include <CastorUtils/Line.h>
#include <CastorUtils/Plane.h>
#include <CastorUtils/Colour.h>
#include <CastorUtils/AutoSingleton.h>


#include <CastorUtils/Module_Utils.h>

#ifndef CHECK_MEMORYLEAKS
#	ifdef _DEBUG
#		define CHECK_MEMORYLEAKS 1
#	else
#		define CHECK_MEMORYLEAKS 0
#	endif
#endif

#if CHECK_MEMORYLEAKS
//#	warning "Castor3D Checking Memory leaks"
#	include <CastorUtils/Memory.h>
#endif

using namespace General::Resource;
using namespace General::Utils;
using namespace General::Math;

#endif

