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

#undef CASTOR_UNICODE

#ifndef _WIN32
#	define _FILE_OFFSET_BITS 64
#	define FSeek fseeko64
#	define FTell ftello64
#	define Localtime( x, y) x = localtime( y)
#	define Sleep usleep
#else
#	ifdef __GNUG__
#		define _FILE_OFFSET_BITS 64
#		define FSeek fseeko64
#		define FTell ftello64
#		define Localtime( x, y) x = localtime( y)
#	else
#		define FSeek _fseeki64
#		define FTell _ftelli64
#		define Localtime( x, y) x = localtime( y)
#	endif
#endif

#ifndef _UNICODE
#	define CASTOR_UNICODE 0						//!< Tells Castor3D was compiled in MultiByte mode
#	define CU_T( x) x							//!< The macro to use with constant strings, id est : C3D_( "a string")
#	define Cout( x) std::cout << x				//!< Use std::cout
#	define Cerr( x) std::cerr << x				//!< Use std::cerr
#	ifndef _WIN32
#		define Sprintf snprintf					//!< Defines snprintf instead of sprintf_s on Unix Systems
#		define Strcpy( x, y, z) strncpy(x, z, y)
#		define Sscanf sscanf
#		define Vsnprintf( x, y, z, w, t) vsnprintf( x, z, w, t)
#		define FOpen( file, path, flags) file = fopen( path, flags)
#       define FOpen64( file, path, flags) file = fopen64( path, flags)
#   else
#       ifndef __GNUG__
#			define Sprintf sprintf_s
#		    define Strcpy strcpy_s
#		    define Sscanf sscanf_s
#			define Vsnprintf( x, y, z, w, t) vsnprintf_s( x, y, z, w, t)
#			define FOpen( file, path, flags) fopen_s( & file, path, flags)
#           define FOpen64( file, path, flags) fopen_s( & file, path, flags)
#       else
#		    define Sprintf snprintf					//!< Defines snprintf instead of sprintf_s on Unix Systems
#		    define Strcpy( x, y, z) strncpy(x, z, y)
#		    define Sscanf sscanf
#			define Vsnprintf( x, y, z, w, t) vsnprintf( x, z, w, t)
#			define FOpen( file, path, flags) file = fopen( path, flags)
#           define FOpen64( file, path, flags) file = fopen64( path, flags)
#       endif
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
#		define Strcpy( x, y, z) wstrncpy( x, z, y)
#		define Sscanf swscanf
#		define Vsnprintf( x, y, z, w, t) vswprintf( x, z, w, t)
#		define FOpen( file, path, flags) file = wfopen( path, flags)
#		define FOpen64( file, path, flags) file = wfopen64( path, flags)
#	else
#		ifndef __GNUG__
#			define Sprintf swprintf_s				//!< Defines swprintf_s instead of sprintf_s on Windows Systems
#			define Strcpy wcscpy_s
#			define Sscanf swscanf_s
#			define Vsnprintf( x, y, z, w, t) _vsnwprintf_s( x, y, z, w, t)
#			define FOpen( file, path, flags) _wfopen_s( & file, path, flags)
#			define FOpen64( file, path, flags) _wfopen_s( & file, path, flags)
#		else
#			define Sprintf swnprintf				//!< Defines swnprintf instead of sprintf_s on Unix Systems
#			define Strcpy( x, y, z) wstrncpy( x, z, y)
#			define Sscanf swscanf
#			define Vsnprintf( x, y, z, w, t) vswprintf( x, z, w, t)
#			define FOpen( file, path, flags) file = wfopen( path, flags)
#			define FOpen64( file, path, flags) file = wfopen64( path, flags)
#		endif
#	endif
#endif

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

#include <vector>
#include <list>
#include <map>
#include <set>

namespace Castor
{
	namespace MultiThreading
	{
		class RecursiveMutex;
		class Mutex;
		class Thread;
	}
#if CASTOR_USE_MT_CONTAINERS
	namespace MultiThreading
	{
		template <typename ObjType> class Vector;
		template <typename ObjType> class List;
		template <typename ObjType> class Set;
		template <typename KeyType, typename ObjType> class Map;
		template <typename KeyType, typename ObjType> class Multimap;
	}

	//! struct holding vector, list and set
	template <typename T>
	struct Container
	{
		typedef Castor::MultiThreading::Vector<	T>	Vector;
		typedef Castor::MultiThreading::List<	T>	List;
		typedef Castor::MultiThreading::Set<	T>	Set;
	};
	//! struct holding map and multimap
	template <typename T, typename U>
	struct KeyedContainer
	{
		typedef Castor::MultiThreading::Map<		T, U>	Map;
		typedef Castor::MultiThreading::Multimap<	T, U>	Multimap;
	};
#else
	//! struct holding vector, list and set
	template <typename T>
	struct Container
	{
		typedef std::vector<	T>	Vector;
		typedef std::list<		T>	List;
		typedef std::set<		T>	Set;
	};
	//! struct holding map and multimap
	template <typename T, typename U>
	struct KeyedContainer
	{
		typedef std::map<		T, U>	Map;
		typedef std::multimap<	T, U>	Multimap;
	};
#endif
}

#include "StringConverter.h"
#include "CastorString.h"
#include "Value.h"
#include "Vector.h"
#include "List.h"
#include "Map.h"
#include "Multimap.h"
#include "Set.h"
#include <cstdarg>

#if ! _HAS_TR1 && ! _HAS_0X
#	include <boost/shared_ptr.hpp>
#	include <boost/weak_ptr.hpp>
#	include <boost/array.hpp>
#else
#	include <array>
#endif

#include "SmartPtr.h"

namespace Castor
{
	namespace Templates
	{
		template <typename T> class ScopedPtr;
		class SharedCount;

#if _HAS_TR1
		//! static size array
		template <typename T, size_t Count>
		struct Array
		{
			typedef std::tr1::array<T, Count> Value;
		};
#elif _HAS_0X
		//! static size array
		template <typename T, size_t Count>
		struct Array
		{
			typedef std::array<T, Count> Value;
		};
#else
		//! static size array
		template <typename T, size_t Count>
		struct Array
		{
			typedef boost::array<T, Count> Value;
		};
#endif
	}

	using namespace Templates;

	namespace Math
	{
		class SphericalVertex;					//!< Spherical vertex representation
		class Quaternion;						//!< The quaternion representation
		template <typename T> class Line;		//!< The representation of a line (y = ax + b)
		template <typename T> class PlaneEq;	//!< The plane equation representation
		class ContainerBox;						//!< Represents a geometry container
		class CubeBox;							//!< Combo box aligned on axis representation
		class SphereBox;						//!< The sphere representation
		class Angle;							//!< Angle representation independant from degree or radian
		class Rectangle;

		template <typename T, size_t Count> class Point;
		template <typename T, size_t Rows, size_t Columns> class Matrix;
		template <typename T, size_t Rows> class SquareMatrix;

		typedef Point<	int, 	4>		Point4i;
		typedef Point<	int, 	3>		Point3i;
		typedef Point<	int, 	2>		Point2i;
		typedef Point<	real, 	4>		Point4r;
		typedef Point<	real, 	3>		Point3r;
		typedef Point<	real, 	2>		Point2r;
		typedef Point<	float, 	4>		Point4f;
		typedef Point<	float, 	3>		Point3f;
		typedef Point<	float, 	2>		Point2f;
		typedef Point<	double,	4>		Point4d;
		typedef Point<	double,	3>		Point3d;
		typedef Point<	double,	2>		Point2d;

		typedef SquareMatrix<	real, 	4>	Matrix4x4r;
		typedef SquareMatrix<	real, 	3>	Matrix3x3r;
		typedef SquareMatrix<	real, 	2>	Matrix2x2r;
		typedef SquareMatrix<	float, 	4>	Matrix4x4f;
		typedef SquareMatrix<	float, 	3>	Matrix3x3f;
		typedef SquareMatrix<	float, 	2>	Matrix2x2f;
		typedef SquareMatrix<	double,	4>	Matrix4x4d;
		typedef SquareMatrix<	double,	3>	Matrix3x3d;
		typedef SquareMatrix<	double,	2>	Matrix2x2d;

		typedef Templates::shared_ptr<	Quaternion		>	QuaternionPtr;
		typedef Templates::shared_ptr<	SphericalVertex	>	SphericalVertexPtr;
		typedef Templates::shared_ptr<	ContainerBox	>	ContainerBoxPtr;
		typedef Templates::shared_ptr<	CubeBox			>	CubeBoxPtr;
		typedef Templates::shared_ptr<	SphereBox		>	SphereBoxPtr;
		typedef Templates::shared_ptr<	Point4i			>	Point4iPtr;
		typedef Templates::shared_ptr<	Point3i			>	Point3iPtr;
		typedef Templates::shared_ptr<	Point2i			>	Point2iPtr;
		typedef Templates::shared_ptr<	Point4r			>	Point4rPtr;
		typedef Templates::shared_ptr<	Point3r			>	Point3rPtr;
		typedef Templates::shared_ptr<	Point2r			>	Point2rPtr;
		typedef Templates::shared_ptr<	Point4f			>	Point4fPtr;
		typedef Templates::shared_ptr<	Point3f			>	Point3fPtr;
		typedef Templates::shared_ptr<	Point2f			>	Point2fPtr;
		typedef Templates::shared_ptr<	Point4d			>	Point4dPtr;
		typedef Templates::shared_ptr<	Point3d			>	Point3dPtr;
		typedef Templates::shared_ptr<	Point2d			>	Point2dPtr;
		typedef Templates::shared_ptr<	Matrix4x4r		>	Matrix4x4rPtr;
		typedef Templates::shared_ptr<	Matrix3x3r		>	Matrix3x3rPtr;
		typedef Templates::shared_ptr<	Matrix2x2r		>	Matrix2x2rPtr;
		typedef Templates::shared_ptr<	Matrix4x4f		>	Matrix4x4fPtr;
		typedef Templates::shared_ptr<	Matrix3x3f		>	Matrix3x3fPtr;
		typedef Templates::shared_ptr<	Matrix2x2f		>	Matrix2x2fPtr;
		typedef Templates::shared_ptr<	Matrix4x4d		>	Matrix4x4dPtr;
		typedef Templates::shared_ptr<	Matrix3x3d		>	Matrix3x3dPtr;
		typedef Templates::shared_ptr<	Matrix2x2d		>	Matrix2x2dPtr;

		typedef Container<Point4r				>::Vector	Point4rArray;				//!< Point3r array
		typedef Container<Point3r				>::Vector	Point3rArray;				//!< Point3r array
		typedef Container<Point2r				>::Vector	Point2rArray;				//!< Point3r array
		typedef Container<Point4rPtr			>::Vector	Point4rPtrArray;			//!< Point3r pointer array
		typedef Container<Point3rPtr			>::Vector	Point3rPtrArray;			//!< Point3r pointer array
		typedef Container<Point2rPtr			>::Vector	Point2rPtrArray;			//!< Point3r pointer array
		typedef Container<Point4rPtr			>::List		Point4rPtrList;				//!< Point3r pointer list
		typedef Container<Point3rPtr			>::List		Point3rPtrList;				//!< Point3r pointer list
		typedef Container<Point2rPtr			>::List		Point2rPtrList;				//!< Point3r pointer list
		typedef Container<SphericalVertexPtr	>::Vector	SphericalVertexPtrArray;	//!< SphericalVertex array
	}

	typedef KeyedContainer<	String,	bool	>::Map		BoolStrMap;
	typedef Container<		int				>::Vector	IntArray;
}

#if CHECK_MEMORYLEAKS
#	include "Memory.h"
#endif

#endif
