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
#ifndef ___C3D_Module_General___
#define ___C3D_Module_General___

#ifdef WIN32
// disable: "<type> needs to have dll-interface to be used by clients'
// Happens on STL member variables which are not public therefore is ok
#   pragma warning (disable : 4251)

// disable: "non dll-interface class used as base for dll-interface class"
// Happens when deriving from Singleton because bug in compiler ignores
// template export
#   pragma warning (disable : 4275)

#	ifdef CASTOR3D_EXPORTS
#		define CS3D_API __declspec(dllexport)
#	else
#		define CS3D_API __declspec(dllimport)
#	endif

#else
#	define CS3D_API
#endif

namespace Castor
{
	namespace Utils
	{
		class File;
	}

	typedef C3DVector( int) IntArray;
}

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
#include <CastorUtils/MinMax.h>
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
#include <CastorUtils/Vector3f.h>
#include <CastorUtils/VertexSpherical.h>

#if CHECK_MEMORYLEAKS
#	include <CastorUtils/Memory.h>
#endif

using namespace Castor;
using namespace Castor::Utils;

#include "Log.h"

#endif