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
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___C3D_PRECOMPILED_HEADER_H___
#define ___C3D_PRECOMPILED_HEADER_H___

#ifndef CHECK_MEMORYLEAKS
#	ifdef _DEBUG
#		define CHECK_MEMORYLEAKS 1
#	else
#		define CHECK_MEMORYLEAKS 0
#	endif
#endif

#pragma message( "********************************************************************")
#pragma message( "	Castor3D")

#if CHECK_MEMORYLEAKS
#	pragma message( "		Checking Memory leaks")
#endif
#include <CastorUtils/Memory.h>

#pragma message( "********************************************************************")

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
#include <stack>

#ifndef WIN32
#	define sprintf_s snprintf
#	define Sleep usleep
#endif

#include <CastorUtils/Module_Utils.h>

#include <CastorUtils/Value.h>
//using namespace Castor::Templates;

#include <CastorUtils/CastorString.h>
#include <CastorUtils/SmartPtrCommon.h>
#include <CastorUtils/Angle.h>
#include <CastorUtils/AutoSingleton.h>
#include <CastorUtils/Buffer.h>
#include <CastorUtils/Colour.h>
#include <CastorUtils/Container.h>
#include <CastorUtils/DynamicLibrary.h>
#include <CastorUtils/File.h>
#include <CastorUtils/Image.h>
#include <CastorUtils/Line.h>
#include <CastorUtils/List.h>
#include <CastorUtils/NonCopyable.h>
#include <CastorUtils/Manager.h>
#include <CastorUtils/Map.h>
#include <CastorUtils/FastMath.h>
#include <CastorUtils/Multimap.h>
#include <CastorUtils/Mutex.h>
#include <CastorUtils/Plane.h>
#include <CastorUtils/Point.h>
#include <CastorUtils/PreciseTimer.h>
#include <CastorUtils/Quaternion.h>
#include <CastorUtils/Resource.h>
#include <CastorUtils/ResourceLoader.h>
#include <CastorUtils/ScopedPtr.h>
#include <CastorUtils/Set.h>
#include <CastorUtils/Thread.h>
#include <CastorUtils/Vector.h>
#include <CastorUtils/SphericalVertex.h>

using namespace Castor::Resource;
using namespace Castor::Utils;
using namespace Castor::Math;
using namespace Castor::Templates;

#include "animation/Module_Animation.h"
#include "camera/Module_Camera.h"
#include "geometry/Module_Geometry.h"
#include "importer/Module_Importer.h"
#include "light/Module_Light.h"
#include "main/Module_Main.h"
#include "material/Module_Material.h"
#include "overlay/Module_Overlay.h"
#include "render_engine/Module_RenderEngine.h"
#include "render_system/Module_Render.h"
#include "scene/Module_Scene.h"
#include "shader/Module_Shader.h"
/*
#include "scene/Scene.h"
#include "material/Material.h"
#include "animation/Animation.h"
*/

#endif

