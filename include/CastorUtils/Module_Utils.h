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

#if ! _HAS_TR1
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

		typedef Templates::SmartPtr<	Quaternion		>::Shared	QuaternionPtr;
		typedef Templates::SmartPtr<	SphericalVertex	>::Shared	SphericalVertexPtr;
		typedef Templates::SmartPtr<	ContainerBox	>::Shared	ContainerBoxPtr;
		typedef Templates::SmartPtr<	CubeBox			>::Shared	CubeBoxPtr;
		typedef Templates::SmartPtr<	SphereBox		>::Shared	SphereBoxPtr;
		typedef Templates::SmartPtr<	Point4i			>::Shared	Point4iPtr;
		typedef Templates::SmartPtr<	Point3i			>::Shared	Point3iPtr;
		typedef Templates::SmartPtr<	Point2i			>::Shared	Point2iPtr;
		typedef Templates::SmartPtr<	Point4r			>::Shared	Point4rPtr;
		typedef Templates::SmartPtr<	Point3r			>::Shared	Point3rPtr;
		typedef Templates::SmartPtr<	Point2r			>::Shared	Point2rPtr;
		typedef Templates::SmartPtr<	Point4f			>::Shared	Point4fPtr;
		typedef Templates::SmartPtr<	Point3f			>::Shared	Point3fPtr;
		typedef Templates::SmartPtr<	Point2f			>::Shared	Point2fPtr;
		typedef Templates::SmartPtr<	Point4d			>::Shared	Point4dPtr;
		typedef Templates::SmartPtr<	Point3d			>::Shared	Point3dPtr;
		typedef Templates::SmartPtr<	Point2d			>::Shared	Point2dPtr;
		typedef Templates::SmartPtr<	Matrix4x4r		>::Shared	Matrix4x4rPtr;
		typedef Templates::SmartPtr<	Matrix3x3r		>::Shared	Matrix3x3rPtr;
		typedef Templates::SmartPtr<	Matrix2x2r		>::Shared	Matrix2x2rPtr;
		typedef Templates::SmartPtr<	Matrix4x4f		>::Shared	Matrix4x4fPtr;
		typedef Templates::SmartPtr<	Matrix3x3f		>::Shared	Matrix3x3fPtr;
		typedef Templates::SmartPtr<	Matrix2x2f		>::Shared	Matrix2x2fPtr;
		typedef Templates::SmartPtr<	Matrix4x4d		>::Shared	Matrix4x4dPtr;
		typedef Templates::SmartPtr<	Matrix3x3d		>::Shared	Matrix3x3dPtr;
		typedef Templates::SmartPtr<	Matrix2x2d		>::Shared	Matrix2x2dPtr;

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