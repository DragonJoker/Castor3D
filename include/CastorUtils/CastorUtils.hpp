/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

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

#include "Macros.hpp"

#if CASTOR_HAS_STDSMARTPTR
#	include <memory.h>
#elif CASTOR_HAS_TR1SMARTPTR
#	include <memory.h>
#else
#	include <boost/shared_ptr.hpp>
#	include <boost/weak_ptr.hpp>
#endif

#if CASTOR_HAS_STDARRAY
#	include <array>
#elif CASTOR_HAS_TR1ARRAY
#	include <array>
#else
#	include <boost/array.hpp>
#endif

#if CASTOR_USE_DOUBLE
	typedef double real;
#else
	typedef float real;
#endif

#include <vector>
#include <list>
#include <map>
#include <set>

namespace Castor
{
	typedef enum
	{
		eMonthUndef,
		eMonthJan,
		eMonthFeb,
		eMonthMar,
		eMonthApr,
		eMonthMay,
		eMonthJun,
		eMonthJul,
		eMonthAug,
		eMonthSep,
		eMonthOct,
		eMonthNov,
		eMonthDec
	}
	eMONTH;

	typedef enum
	{
		eDayUndef,
		eDaySun,
		eDayMon,
		eDayTue,
		eDayWed,
		eDayThu,
		eDayFri,
		eDaySat,
	}
	eWEEK_DAY;

	class String;

	namespace Utils
	{
		class Date;
		class DateTime;
		class Time;
	}
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

#include "StringConverter.hpp"
#include "String.hpp"
#include "Value.hpp"
#include "Vector.hpp"
#include "List.hpp"
#include "Map.hpp"
#include "Multimap.hpp"
#include "Set.hpp"
#include <cstdarg>

#include "SmartPtr.hpp"

namespace Castor
{
	namespace Templates
	{
		//! static size array
#if CASTOR_HAS_STDARRAY
		using std::array;
#elif CASTOR_HAS_TR1ARRAY
		using std::tr1::array;
#else
		using boost::array;
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

		typedef Point<	int, 			4>		Point4i;
		typedef Point<	int, 			3>		Point3i;
		typedef Point<	int, 			2>		Point2i;
		typedef Point<	real, 			4>		Point4r;
		typedef Point<	real, 			3>		Point3r;
		typedef Point<	real, 			2>		Point2r;
		typedef Point<	float, 			4>		Point4f;
		typedef Point<	float, 			3>		Point3f;
		typedef Point<	float, 			2>		Point2f;
		typedef Point<	double,			4>		Point4d;
		typedef Point<	double,			3>		Point3d;
		typedef Point<	double,			2>		Point2d;
		typedef Point<	unsigned char,	4>		Point4ub;
		typedef Point<	unsigned char,	3>		Point3ub;
		typedef Point<	unsigned char,	2>		Point2ub;
		typedef Point<	unsigned int,	2>		Size;
		typedef Point<	unsigned int,	2>		Position;

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
		typedef Templates::shared_ptr<	Point4ub		>	Point4ubPtr;
		typedef Templates::shared_ptr<	Point3ub		>	Point3ubPtr;
		typedef Templates::shared_ptr<	Point2ub		>	Point2ubPtr;
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

	namespace Resources
	{
		//! Pixel format enumerator
		/*!
		Enumerates the supported pixel formats
		*/
		typedef enum
		{	ePIXEL_FORMAT_L4		//!< 4 bits luminosity
		,	ePIXEL_FORMAT_A4L4		//!< 8 bits alpha and luminosity
		,	ePIXEL_FORMAT_L8		//!< 8 bits luminosity
		,	ePIXEL_FORMAT_A8L8		//!< 16 bits alpha and luminosity
		,	ePIXEL_FORMAT_A1R5G5B5	//!< 16 bits 5551 ARGB
		,	ePIXEL_FORMAT_A4R4G4B4	//!< 16 bits 4444 ARGB
		,	ePIXEL_FORMAT_R8G8B8	//!< 24 bits 888 RGB
		,	ePIXEL_FORMAT_A8R8G8B8	//!< 32 bits 8888 ARGBA
		,	ePIXEL_FORMAT_DXTC1		//!< DXT1 8 bits compressed format
		,	ePIXEL_FORMAT_DXTC3		//!< DXT3 16 bits compressed format
		,	ePIXEL_FORMAT_DXTC5		//!< DXT5 16 bits compressed format
		,	ePIXEL_FORMAT_YUY2		//!< YUY2 16 bits compressed format
		,	ePIXEL_FORMAT_COUNT
		}	ePIXEL_FORMAT;

		class Image;								//!< The image representation
		class ImageLoader;							//!< Image loader
		class ImageManager;							//!< The images manager
		class Font;
		class FontLoader;
		class FontManager;
		template <typename T> class Buffer;			//!< The buffer representation
		template <class T> class ResourceLoader;	//!< Resource loader
		template <typename ResType> class Resource;	//!< Resource representation

		typedef Templates::shared_ptr<	Image	>	ImagePtr;
		typedef Templates::shared_ptr<	Font	>	FontPtr;
	}

	typedef KeyedContainer<	String,	bool	>::Map		BoolStrMap;
	typedef Container<		int				>::Vector	IntArray;
}

#if CHECK_MEMORYLEAKS
#	include "Memory.hpp"
#endif

#endif
