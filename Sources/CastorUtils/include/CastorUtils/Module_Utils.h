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
#ifndef ___Castor_Module_Utils___
#define ___Castor_Module_Utils___

#include "config.h"
#include "StringConverter.h"
#include "CastorString.h"
#include "Value.h"
#include "Vector.h"
#include "List.h"
#include "Map.h"
#include "Multimap.h"
#include "Set.h"
#include <cstdarg>
#include <array>

#undef CASTOR_UNICODE

#ifndef _UNICODE
#	define CASTOR_UNICODE 0						//!< Tells Castor3D was compiled in MultiByte mode
#	define CU_T( x) x							//!< The macro to use with constant strings, id est : C3D_( "a string")
#	define Cout( x) std::cout << x				//!< Use std::cout
#	define Cerr( x) std::cerr << x				//!< Use std::cerr
#	ifndef WIN32
#		define Sprintf snprintf					//!< Defines snprintf instead of sprintf_s on Unix Systems
#		define Strcpy strncpy
#else
#		define Sprintf sprintf_s
#		define Strcpy strcpy_s
#	endif
#else
#	define CASTOR_UNICODE 1						//!< Tells Castor3D was compiled in Unicode mode
#	define CU_T( x) L##x						//!< The macro to use with constant strings, id est : : C3D_( "a string")
#	define Cout( x) std::wcout << x				//!< Use std::wcout
#	define Cerr( x) std::wcerr << x				//!< Use std::wcerr
#	define atoi _wtoi
#	define atof _wtof
#	define atol _wtol
#	ifndef WIN32									
#		define Sprintf swnprintf				//!< Defines swnprintf instead of sprintf_s on Unix Systems
#		define Strcpy wstrncpy
#	else
#		define Sprintf swprintf_s				//!< Defines swprintf_s instead of sprintf_s on Windows Systems
#		define Strcpy wcscpy_s
#	endif
#endif

#ifndef WIN32
#	define Sleep usleep
#	define fopen_s( x, y, z) *x = fopen( y, z)
#	define localtime_s( x, y) *x = localtime( y)
#endif
/*
using namespace Castor;
using namespace Castor::Utils;
*/
#if CASTOR_USE_DOUBLE
	typedef double real;
#	define square_root sqrt
#	define ator atof
#else
	typedef float real;
#	define square_root sqrtf
#	define ator (real)atof
#endif

#define _times( X) for (unsigned int i = 0 ; i < X ; i ++)
#define _forui( X, Y) for (unsigned int X = 0 ; X < Y ; X ++)

#if CASTOR_USE_MT_CONTAINERS
#	define C3DVector( x)		Castor::MultiThreading::Vector< x >
#	define C3DMap(x, y)			Castor::MultiThreading::Map< x, y >
#	define C3DMultimap(x, y)	Castor::MultiThreading::Multimap< x, y >
#	define C3DList( x)			Castor::MultiThreading::List< x >
#	define C3DSet( x)			Castor::MultiThreading::Set< x >
#else
#	define C3DVector( x)		std::vector< x >
#	define C3DMap(x, y)			std::map< x, y >
#	define C3DMultimap(x, y)	std::multimap< x, y >
#	define C3DList( x)			std::list< x >
#	define C3DSet( x)			std::set< x >
#endif

namespace Castor
{
	namespace Templates
	{
//		template <typename T> class SharedPtr;
		template <typename T> class ScopedPtr;
//		template <typename T> class WeakPtr;
		class SharedCount;
	}

	namespace Math
	{
		//! Spherical vertex representation
		class SphericalVertex;
		//! The quaternion representation
		class Quaternion;
		//! The representation of a line (y = ax + b)
		template <typename T> class Line;
		//! The plane equation representation
		template <typename T> class Plane;
		//! Represents a geometry container
		class Container;
		//! Combo box aligned on axis representation
		class ComboBox;
		//! The sphere representation
		class Sphere;
		//! Angle representation independant from degree or radian
		class Angle;

		template <typename T, size_t Count> class Point;
		template <typename T, size_t Rows, size_t Columns> class Matrix;
		template <typename T, size_t Rows> class SquareMatrix;

		typedef Point<real, 4>			Point4r;
		typedef Point<real, 3>			Point3r;
		typedef Point<real, 2>			Point2r;

		typedef Point<float, 4>			Point4f;
		typedef Point<float, 3>			Point3f;
		typedef Point<float, 2>			Point2f;

		typedef Point<double, 4>		Point4d;
		typedef Point<double, 3>		Point3d;
		typedef Point<double, 2>		Point2d;

		typedef Point<int, 4>			Point4i;
		typedef Point<int, 3>			Point3i;
		typedef Point<int, 2>			Point2i;

		typedef SquareMatrix<real, 4>	Matrix4x4r;
		typedef SquareMatrix<real, 3>	Matrix3x3r;
		typedef SquareMatrix<real, 2>	Matrix2x2r;

		typedef SquareMatrix<float, 4>	Matrix4x4f;
		typedef SquareMatrix<float, 3>	Matrix3x3f;
		typedef SquareMatrix<float, 2>	Matrix2x2f;

		typedef SquareMatrix<double, 4>	Matrix4x4d;
		typedef SquareMatrix<double, 3>	Matrix3x3d;
		typedef SquareMatrix<double, 2>	Matrix2x2d;

		typedef Templates::SharedPtr<Quaternion>			QuaternionPtr;
		typedef Templates::SharedPtr<SphericalVertex>		SphericalVertexPtr;
		typedef Templates::SharedPtr<Container>				ContainerPtr;
		typedef Templates::SharedPtr<ComboBox>				ComboBoxPtr;
		typedef Templates::SharedPtr<Sphere>				SpherePtr;

		typedef Templates::SharedPtr<Point4r>				Point4rPtr;
		typedef Templates::SharedPtr<Point3r>				Point3rPtr;
		typedef Templates::SharedPtr<Point2r>				Point2rPtr;

		typedef Templates::SharedPtr<Point4i>				Point4iPtr;
		typedef Templates::SharedPtr<Point3i>				Point3iPtr;
		typedef Templates::SharedPtr<Point2i>				Point2iPtr;

		typedef Templates::SharedPtr<Matrix4x4r>			Matrix4x4rPtr;
		typedef Templates::SharedPtr<Matrix3x3r>			Matrix3x3rPtr;
		typedef Templates::SharedPtr<Matrix2x2r>			Matrix2x2rPtr;

		//! Point3r array
		typedef C3DVector(	Point4r)				Point4rArray;
		//! Point3r pointer array
		typedef C3DVector(	Point4rPtr)				Point4rPtrArray;
		//! Point3r pointer list
		typedef C3DList(	Point4rPtr)				Point4rPtrList;
		
		//! Point3r array
		typedef C3DVector(	Point3r)				Point3rArray;
		//! Point3r pointer array
		typedef C3DVector(	Point3rPtr)				Point3rPtrArray;
		//! Point3r pointer list
		typedef C3DList(	Point3rPtr)				Point3rPtrList;
		
		//! Point3r array
		typedef C3DVector(	Point2r)				Point2rArray;
		//! Point3r pointer array
		typedef C3DVector(	Point2rPtr)				Point2rPtrArray;
		//! Point3r pointer list
		typedef C3DList(	Point2rPtr)				Point2rPtrList;

		//! SphericalVertex array
		typedef C3DVector(	SphericalVertexPtr)		SphericalVertexPtrArray;
	}

	typedef C3DMap(		String,		bool)	BoolStrMap;
}

#if CHECK_MEMORYLEAKS
#	include "Memory.h"
#endif

#endif