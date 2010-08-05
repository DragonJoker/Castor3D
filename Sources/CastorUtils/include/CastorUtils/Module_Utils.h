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
#ifndef ___CSU_Module_Utils___
#define ___CSU_Module_Utils___

#	include <vector>
#	include <list>
#	include <map>
#	include <math.h>
#	include <iostream>
#	include <stdarg.h>
#	include <time.h>

#	include "StringConverter.h"
#	include "CastorString.h"

#	ifndef _UNICODE
#		define C3D_UNICODE 0						//!< Tells Castor3D was compiled in MultiByte mode
#		define C3D_T( x) x							//!< The macro to use with constant strings, id est : C3D_( "a string")
#		define Cout( x) std::cout << x				//!< Use std::cout
#		define Cerr( x) std::cerr << x				//!< Use std::cerr
#		ifndef WIN32
#			define Sprintf snprintf					//!< Defines snprintf instead of sprintf_s on Unix Systems
#			define Strcpy strncpy
#	else
#			define Sprintf sprintf_s
#			define Strcpy strcpy_s
#		endif
#	else
#		define C3D_UNICODE 1						//!< Tells Castor3D was compiled in Unicode mode
#		define C3D_T( x) L##x						//!< The macro to use with constant strings, id est : : C3D_( "a string")
#		define Cout( x) std::wcout << x				//!< Use std::wcout
#		define Cerr( x) std::wcerr << x				//!< Use std::wcerr
#		ifndef WIN32									
#			define Sprintf swnprintf				//!< Defines swnprintf instead of sprintf_s on Unix Systems
#			define Strcpy wstrncpy
#		else
#			define Sprintf swprintf_s				//!< Defines swprintf_s instead of sprintf_s on Windows Systems
#			define Strcpy wcscpy_s
#		endif
#	endif
//! The empty string macro

#	ifndef WIN32
#		define Sleep usleep
#		define fopen_s( x, y, z) *x = fopen( y, z)
#		define localtime_s( x, y) *x = localtime( y)
#	endif

using namespace General;
using namespace General::Utils;

#	ifdef M_PI
#		undef M_PI
#	endif

//! PI value
const float M_PI			= 3.14159265358979323846f;
//! PI / 2 value
const float M_PI_DIV_2		= 1.5707963267948966192313216916398f;
//! PI * 2 value
const float M_PI_MULT_2		= 6.283185307179586476925286766559f;

#	define _times( X) for (unsigned int i = 0 ; i < X ; i ++)
#	define _forui( X, Y) for (unsigned int X = 0 ; X < Y ; X ++)

namespace General
{	namespace Math
{
	//! Representation of vertex and vector
	class Vector3f;
	//! Spherical vertex representation
	class VertexSpherical;
	//! The transformation matrix representation
	class RotationMatrix;
	//! The quaternion representation
	class Quaternion;
	//! The line equation representation
	class Line;
	//! The plane equation representation
	class Plane;
	//! Represents a geometry container
	class Container;
	//! Combo box aligned on axis representation
	class ComboBox;
	//! The sphere representation
	class Sphere;
	//The representation of a line (y = ax + b)
	class Line;

	//! Vector3f array
	typedef std::vector	<Vector3f>			Vector3fArray;
	//! Vector3f pointer array
	typedef std::vector	<Vector3f *>		Vector3fPtrArray;
	//! Vector3f pointer list
	typedef std::list	<Vector3f *>		Vector3fPtrList;
	//! VertexSpherical array
	typedef std::vector <VertexSpherical *>	VertexSphericalPtrArray;
}
}

#endif
