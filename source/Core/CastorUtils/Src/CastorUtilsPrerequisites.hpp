/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___CASTOR_UTILS_PREREQUISITES_H___
#define ___CASTOR_UTILS_PREREQUISITES_H___

#include <array>
#include <chrono>
#include <cstdint>
#include <functional>
#include <iostream>
#include <list>
#include <map>
#include <map>
#include <memory>
#include <regex>
#include <set>
#include <string>
#include <deque>
#include <sstream>
#include <vector>

#include "Config/SmartPtr.hpp"
#include "Log/ELogType.hpp"

#if defined( min )
#	undef min
#endif
#if defined( max )
#	undef max
#endif
#if defined( abs )
#	undef abs
#endif

namespace castor
{
	/*!
	\author Sylvain DOREMUS
	\version 0.6.1.0
	\date 19/10/2011
	\~english
	\brief castor::Loader supported file types enumeration
	\~french
	\brief Enumération des types de fichier supportés par castor::Loader
	*/
	enum class FileType
		: uint8_t
	{
		//! Binary file
		eBinary,
		//! Text file
		eText,
		CASTOR_SCOPED_ENUM_BOUNDS( eBinary )
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		24/05/2016
	\~english
	\brief		Helper structure to find File type (BinaryFile or TextFile) from FileType.
	\~french
	\brief		Structure d'aide pour trouver le type de File (BinaryFile ou TextFile) à partir d'un FileType.
	*/
	template< FileType FT >
	struct FileTyper;
	/*!
	\~english
	\brief Pixel format enumeration
	\~french
	\brief Enumération des formats de Pixel
	*/
	enum class PixelFormat
		: uint8_t
	{
		//!\~english	8 bits luminosity.
		//!\~french		Luminosité 8 bits.
		eL8,
		//!\~english	Half floats luminosity on VRAM, floats luminosity on RAM.
		//!\~french		Luminosité en half float en VRAM, et en float en RAM.
		eL16F32F,
		//!\~english	32 bits loats luminosity.
		//!\~french		Luminosité en float 32 bits.
		eL32F,
		//!\~english	16 bits alpha and luminosity.
		//!\~french		Alpha et luminosité 16 bits.
		eA8L8,
		//!\~english	Half floats alpha and luminosity on VRAM, floats alpha and luminosity on RAM.
		//!\~french		Luminosité et alpha en half float en VRAM, et en float en RAM.
		eAL16F32F,
		//!\~english	32 bits floats alpha and luminosity.
		//!\~french		Luminosité et alpha en float 32 bits.
		eAL32F,
		//!\~english	16 bits 5551 ARGB.
		//!\~french		16 bits 5551 ARGB.
		eA1R5G5B5,
		//!\~english	16 bits 4444 ARGB.
		//!\~french		16 bits 4444 ARGB.
		eA4R4G4B4,
		//!\~english	16 bits 565 RGB.
		//!\~french		16 bits 565 RGB.
		eR5G6B5,
		//!\~english	24 bits 888 RGB.
		//!\~french		24 bits 888 RGB.
		eR8G8B8,
		//!\~english	24 bits 888 BGR.
		//!\~french		24 bits 888 BGR.
		eB8G8R8,
		//!\~english	32 bits 8888 ARGB.
		//!\~french		32 bits 8888 ARGB.
		eA8R8G8B8,
		//!\~english	32 bits 8888 ABGR.
		//!\~french		32 bits 8888 ABGR.
		eA8B8G8R8,
		//!\~english	Half float RGB.
		//!\~french		Half float RGB.
		eRGB16F,
		//!\~english	Half float ARGB.
		//!\~french		Half float ARGB.
		eRGBA16F,
		//!\~english	Half float RGB on VRAM, Float RGB on RAM.
		//!\~french		RGB en half float en VRAM, et en float en RAM.
		eRGB16F32F,
		//!\~english	Half float ARGB on VRAM, Float ARGB on RAM.
		//!\~french		ARGB en half float en VRAM, et en float en RAM.
		eRGBA16F32F,
		//!\~english	32 bits float RGB.
		//!\~french		RGB en flottants 32 bits.
		eRGB32F,
		//!\~english	32 bits float ARGB.
		//!\~french		ARGB en flottants 32 bits.
		eRGBA32F,
		//!\~english	DXT1 8 bits compressed format.
		//!\~french		Format compressé DXT1 8 bits.
		eDXTC1,
		//!\~english	DXT3 16 bits compressed format.
		//!\~french		Format compressé DXT3 16 bits.
		eDXTC3,
		//!\~english	DXT5 16 bits compressed format.
		//!\~french		Format compressé DXT5 16 bits.
		eDXTC5,
		//!\~english	YUY2 16 bits compressed format.
		//!\~french		Format compressé YUY2 16 bits.
		eYUY2,
		//!\~english	Depth 16 bits.
		//!\~french		Profondeur 16 bits.
		eD16,
		//!\~english	Depth 24 bits.
		//!\~french		Profondeur 24 bits.
		eD24,
		//!\~english	Depth 24 bits, Stencil 8 bits.
		//!\~french		Profondeur 24 bits, Stencil 8 bits.
		eD24S8,
		//!\~english	Depth 32 bits.
		//!\~french		Profondeur 32 bits.
		eD32,
		//!\~english	Depth 32 bits floating point.
		//!\~french		Profondeur en float 32 bits.
		eD32F,
		//!\~english	Depth 32 bits floating point, Stencil 8 bits.
		//!\~french		Profondeur en float 32 bits, Stencil 8 bits.
		eD32FS8,
		//!\~english	Stencil 1 bit.
		//!\~french		Stencil 1 bit.
		eS1,
		//!\~english	Stencil 8 bits.
		//!\~french		Stencil 8 bits.
		eS8,
		CASTOR_SCOPED_ENUM_BOUNDS( eL8 )
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		03/01/2011
	\~english
	\brief		Intersection types
	\remark		Enumerates the different intersection types.
	\~french
	\brief		Types d'intersection
	\remark		Enumère les différents types d'intersection.
	*/
	enum class Intersection
		: uint8_t
	{
		//!\~english	Completely inside.
		//!\~french		Complètement à l'intérieur.
		eIn,
		//!\~english	Completely outside.
		//!\~french		Complètement à l'extérieur.
		eOut,
		//!\~english	Intersection.
		//!\~french		Intersection
		eIntersect,
		CASTOR_SCOPED_ENUM_BOUNDS( eIn )
	};

#if CASTOR_USE_DOUBLE
	using real = double;
#else
	using real = float;
#endif

	using String = std::basic_string< xchar >;
	using Regex = std::basic_regex< xchar >;
	using SRegexIterator = std::regex_iterator< String::const_iterator >;
	using MatchResults = std::match_results< String::const_iterator >;
	using StringStream = std::basic_stringstream< xchar >;
	using OutputStream = std::basic_ostream< xchar >;
	using Milliseconds = std::chrono::milliseconds;
	using Nanoseconds = std::chrono::nanoseconds;

	/*!
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		09/11/2015
	\see		QuaternionT
	\~english
	\brief		Structure used to construct objects but without initialisation.
	\~french
	\brief		Structure utilisée pour les constructeurs d'objets à ne pas initialiser.
	*/
	struct NoInit {};

	template< typename Type >
	class AngleT;
	template< size_t Size >
	struct BaseTypeFromSize;
	class BinaryFile;
	template< class T >
	class BinaryLoader;
	template< typename T, typename Key >
	class Collection;
	template< uint8_t Dimension >
	class ContainerBox;
	template< typename T, uint32_t Count >
	class Coords;
	class CubeBox;
	class File;
	class FileParser;
	class FileParserContext;
	template< typename FlagType
		, typename BaseType = typename BaseTypeFromSize< sizeof( FlagType ) >::Type >
	class FlagCombination;
	class Font;
	class FontCache;
	class Glyph;
	class Image;
	class ImageCache;
	class ColourComponent;
	class HdrColourComponent;
	template< typename ComponentType >
	class ColourT;
	using Colour = ColourT< ColourComponent >;
	using HdrColour = ColourT< HdrColourComponent >;
	template< typename T >
	class Line2D;
	template< typename T >
	class Line3D;
	template< class T, FileType FT >
	class Loader;
	class ILoggerImpl;
	class Logger;
	template< typename T, uint32_t Rows, uint32_t Columns >
	class Matrix;
	template< class Owmer >
	class OwnedBy;
	class ParserParameterBase;
	template< PixelFormat FT >
	class Pixel;
	template< typename T >
	class PlaneEquation;
	template< typename T, uint32_t Count >
	class Point;
	class Position;
	class PxBufferBase;
	template< PixelFormat FT >
	class PxBuffer;
	template< typename T >
	class QuaternionT;
	class Rectangle;
	template< typename T >
	class Resource;
	class Size;
	class SphereBox;
	class SphericalVertex;
	template< typename T, uint32_t Rows >
	class SquareMatrix;
	class TextFile;
	template< class T >
	class TextLoader;
	template< class Obj
		, class Key
		, class PtrType = std::shared_ptr< Obj >
		, typename PFNCreate = std::function< PtrType() >
		, class Predicate = std::less< Key > >
	class Factory;
	class Path;
	class DynamicLibrary;
	struct Message;
	class Logger;
	class LoggerImpl;
	class ProgramConsole;

	/*!
	\author		Sylvain DOREMUS
	\~english
	\brief		Typedef over a buffer of pixels in format A8R8G8B8
	\~french
	\brief		Typedef sur un buffer de pixels au format A8R8G8B8
	*/
	using PixelBuffer = PxBuffer< PixelFormat::eA8R8G8B8 >;

	template< typename T > using Point2 = Point< T, 2 >;
	template< typename T > using Point3 = Point< T, 3 >;
	template< typename T > using Point4 = Point< T, 4 >;

	template< typename T > using Coords2 = Coords< T, 2 >;
	template< typename T > using Coords3 = Coords< T, 3 >;
	template< typename T > using Coords4 = Coords< T, 4 >;

	using Angle = AngleT< real >;
	using Quaternion = QuaternionT< real >;

	DECLARE_POINT( bool,		4, b );
	DECLARE_POINT( bool,		3, b );
	DECLARE_POINT( bool,		2, b );
	DECLARE_POINT( int8_t,		4, c );
	DECLARE_POINT( int8_t,		3, c );
	DECLARE_POINT( int8_t,		2, c );
	DECLARE_POINT( uint8_t,		4, ub );
	DECLARE_POINT( uint8_t,		3, ub );
	DECLARE_POINT( uint8_t,		2, ub );
	DECLARE_POINT( int16_t,		4, s );
	DECLARE_POINT( int16_t,		3, s );
	DECLARE_POINT( int16_t,		2, s );
	DECLARE_POINT( uint16_t,	4, us );
	DECLARE_POINT( uint16_t,	3, us );
	DECLARE_POINT( uint16_t,	2, us );
	DECLARE_POINT( int32_t,		4, i );
	DECLARE_POINT( int32_t,		3, i );
	DECLARE_POINT( int32_t,		2, i );
	DECLARE_POINT( uint32_t,	4, ui );
	DECLARE_POINT( uint32_t,	3, ui );
	DECLARE_POINT( uint32_t,	2, ui );
	DECLARE_POINT( real,		4, r );
	DECLARE_POINT( real,		3, r );
	DECLARE_POINT( real,		2, r );
	DECLARE_POINT( float,		4, f );
	DECLARE_POINT( float,		3, f );
	DECLARE_POINT( float,		2, f );
	DECLARE_POINT( double,		4, d );
	DECLARE_POINT( double,		3, d );
	DECLARE_POINT( double,		2, d );

	DECLARE_COORD( bool,		4, b );
	DECLARE_COORD( bool,		3, b );
	DECLARE_COORD( bool,		2, b );
	DECLARE_COORD( int8_t,		4, c );
	DECLARE_COORD( int8_t,		3, c );
	DECLARE_COORD( int8_t,		2, c );
	DECLARE_COORD( uint8_t,		4, ub );
	DECLARE_COORD( uint8_t,		3, ub );
	DECLARE_COORD( uint8_t,		2, ub );
	DECLARE_COORD( int16_t,		4, s );
	DECLARE_COORD( int16_t,		3, s );
	DECLARE_COORD( int16_t,		2, s );
	DECLARE_COORD( uint16_t,	4, us );
	DECLARE_COORD( uint16_t,	3, us );
	DECLARE_COORD( uint16_t,	2, us );
	DECLARE_COORD( int32_t,		4, i );
	DECLARE_COORD( int32_t,		3, i );
	DECLARE_COORD( int32_t,		2, i );
	DECLARE_COORD( uint32_t,	4, ui );
	DECLARE_COORD( uint32_t,	3, ui );
	DECLARE_COORD( uint32_t,	2, ui );
	DECLARE_COORD( int,			4, i );
	DECLARE_COORD( int,			3, i );
	DECLARE_COORD( int,			2, i );
	DECLARE_COORD( real,		4, r );
	DECLARE_COORD( real,		3, r );
	DECLARE_COORD( real,		2, r );
	DECLARE_COORD( float,		4, f );
	DECLARE_COORD( float,		3, f );
	DECLARE_COORD( float,		2, f );
	DECLARE_COORD( double,		4, d );
	DECLARE_COORD( double,		3, d );
	DECLARE_COORD( double,		2, d );
	DECLARE_COORD( uint8_t,		4, ub );
	DECLARE_COORD( uint8_t,		3, ub );
	DECLARE_COORD( uint8_t,		2, ub );

	DECLARE_CONST_COORD( bool,		4, b );
	DECLARE_CONST_COORD( bool,		3, b );
	DECLARE_CONST_COORD( bool,		2, b );
	DECLARE_CONST_COORD( int8_t,	4, c );
	DECLARE_CONST_COORD( int8_t,	3, c );
	DECLARE_CONST_COORD( int8_t,	2, c );
	DECLARE_CONST_COORD( uint8_t,	4, ub );
	DECLARE_CONST_COORD( uint8_t,	3, ub );
	DECLARE_CONST_COORD( uint8_t,	2, ub );
	DECLARE_CONST_COORD( int16_t,	4, s );
	DECLARE_CONST_COORD( int16_t,	3, s );
	DECLARE_CONST_COORD( int16_t,	2, s );
	DECLARE_CONST_COORD( uint16_t,	4, us );
	DECLARE_CONST_COORD( uint16_t,	3, us );
	DECLARE_CONST_COORD( uint16_t,	2, us );
	DECLARE_CONST_COORD( int32_t,	4, i );
	DECLARE_CONST_COORD( int32_t,	3, i );
	DECLARE_CONST_COORD( int32_t,	2, i );
	DECLARE_CONST_COORD( uint32_t,	4, ui );
	DECLARE_CONST_COORD( uint32_t,	3, ui );
	DECLARE_CONST_COORD( uint32_t,	2, ui );
	DECLARE_CONST_COORD( int,		4, i );
	DECLARE_CONST_COORD( int,		3, i );
	DECLARE_CONST_COORD( int,		2, i );
	DECLARE_CONST_COORD( real,		4, r );
	DECLARE_CONST_COORD( real,		3, r );
	DECLARE_CONST_COORD( real,		2, r );
	DECLARE_CONST_COORD( float,		4, f );
	DECLARE_CONST_COORD( float,		3, f );
	DECLARE_CONST_COORD( float,		2, f );
	DECLARE_CONST_COORD( double,	4, d );
	DECLARE_CONST_COORD( double,	3, d );
	DECLARE_CONST_COORD( double,	2, d );
	DECLARE_CONST_COORD( uint8_t,	4, ub );
	DECLARE_CONST_COORD( uint8_t,	3, ub );
	DECLARE_CONST_COORD( uint8_t,	2, ub );
	
	DECLARE_SQMTX( bool,	4, b );
	DECLARE_SQMTX( bool,	3, b );
	DECLARE_SQMTX( bool,	2, b );
	DECLARE_SQMTX( int,		4, i );
	DECLARE_SQMTX( int,		3, i );
	DECLARE_SQMTX( int,		2, i );
	DECLARE_SQMTX( uint32_t,4, ui );
	DECLARE_SQMTX( uint32_t,3, ui );
	DECLARE_SQMTX( uint32_t,2, ui );
	DECLARE_SQMTX( real,	4, r );
	DECLARE_SQMTX( real,	3, r );
	DECLARE_SQMTX( real,	2, r );
	DECLARE_SQMTX( float,	4, f );
	DECLARE_SQMTX( float,	3, f );
	DECLARE_SQMTX( float,	2, f );
	DECLARE_SQMTX( double,	4, d );
	DECLARE_SQMTX( double,	3, d );
	DECLARE_SQMTX( double,	2, d );

	DECLARE_MTX( bool,		2, 3, b );
	DECLARE_MTX( bool,		2, 4, b );
	DECLARE_MTX( bool,		3, 2, b );
	DECLARE_MTX( bool,		3, 4, b );
	DECLARE_MTX( bool,		4, 2, b );
	DECLARE_MTX( bool,		4, 3, b );
	DECLARE_MTX( int,		2, 3, i );
	DECLARE_MTX( int,		2, 4, i );
	DECLARE_MTX( int,		3, 2, i );
	DECLARE_MTX( int,		3, 4, i );
	DECLARE_MTX( int,		4, 2, i );
	DECLARE_MTX( int,		4, 3, i );
	DECLARE_MTX( uint32_t,	2, 3, ui );
	DECLARE_MTX( uint32_t,	2, 4, ui );
	DECLARE_MTX( uint32_t,	3, 2, ui );
	DECLARE_MTX( uint32_t,	3, 4, ui );
	DECLARE_MTX( uint32_t,	4, 2, ui );
	DECLARE_MTX( uint32_t,	4, 3, ui );
	DECLARE_MTX( real,		2, 3, r );
	DECLARE_MTX( real,		2, 4, r );
	DECLARE_MTX( real,		3, 2, r );
	DECLARE_MTX( real,		3, 4, r );
	DECLARE_MTX( real,		4, 2, r );
	DECLARE_MTX( real,		4, 3, r );
	DECLARE_MTX( float,		2, 3, f );
	DECLARE_MTX( float,		2, 4, f );
	DECLARE_MTX( float,		3, 2, f );
	DECLARE_MTX( float,		3, 4, f );
	DECLARE_MTX( float,		4, 2, f );
	DECLARE_MTX( float,		4, 3, f );
	DECLARE_MTX( double,	2, 3, d );
	DECLARE_MTX( double,	2, 4, d );
	DECLARE_MTX( double,	3, 2, d );
	DECLARE_MTX( double,	3, 4, d );
	DECLARE_MTX( double,	4, 2, d );
	DECLARE_MTX( double,	4, 3, d );

	DECLARE_SMART_PTR( Quaternion );
	DECLARE_SMART_PTR( SphericalVertex );
	DECLARE_SMART_PTR( CubeBox );
	DECLARE_SMART_PTR( SphereBox );
	DECLARE_SMART_PTR( Image );
	DECLARE_SMART_PTR( Font );
	DECLARE_SMART_PTR( PxBufferBase );
	DECLARE_SMART_PTR( FileParserContext );
	DECLARE_SMART_PTR( ParserParameterBase );
	DECLARE_SMART_PTR( DynamicLibrary );

	DECLARE_VECTOR( uint8_t, Byte );
	DECLARE_VECTOR( SphericalVertexSPtr, SphericalVertexPtr );
	DECLARE_VECTOR( int, Int );
	DECLARE_VECTOR( String, String );
	DECLARE_VECTOR( Path, Path );
	DECLARE_VECTOR( ParserParameterBaseSPtr, ParserParameter );
	DECLARE_MAP( String, uint32_t, UIntStr );
	DECLARE_MAP( String, uint64_t, UInt64Str );
	DECLARE_MAP( String, bool, BoolStr );
	DECLARE_MAP( String, String, StrStr );
	DECLARE_SET( String, Str );
	DECLARE_MAP( uint32_t, String, StrUInt );
	/*!
	\author 	Sylvain DOREMUS
	\date 		27/08/2012
	\version	0.7.0.0
	\~english
	\brief		Message representation
	\~french
	\brief		Représentation d'un message
	*/
	struct Message
	{
		//! The message type.
		LogType m_type;
		//! The message text.
		std::string m_message;
		//! Tells if the new line character is printed.
		bool m_newLine;
	};
	//! The message queue.
	typedef std::deque< Message > MessageQueue;
	/**
	 *\~english
	 *\brief		Logging callback function
	 *\param[in]	p_pCaller	Pointer to the caller
	 *\param[in]	p_strLog	The logged text
	 *\param[in]	p_eLogType	The log type
	 *\~french
	 *\brief		Fonction de callback de log
	 *\param[in]	p_pCaller	Pointeur sur l'appelant
	 *\param[in]	p_strLog	Le texte écrit
	 *\param[in]	p_eLogType	Le type de log
	 */
	using LogCallback = std::function< void ( String const & p_strLog, LogType p_eLogType, bool p_newLine ) >;

	/*!
	\~english
	\brief		Supported MemoryData types.
	\~french
	\brief		Types de MemoryData supportés.
	*/
	enum class MemoryDataType
	{
		eFixed,
		eMarked,
		eFixedGrowing,
		eFixedGrowingMarked,
		CASTOR_SCOPED_ENUM_BOUNDS( eFixed )
	};

	template< typename Object, MemoryDataType MemDataType > class PoolManagedObject;

	class NonAlignedMemoryAllocator;
	template< size_t Align > class AlignedMemoryAllocator;

	template< typename Object, typename MemoryAllocator = NonAlignedMemoryAllocator > class FixedSizeMemoryData;
	template< typename Object, typename MemoryAllocator = NonAlignedMemoryAllocator > class FixedGrowingSizeMemoryData;
	template< typename Object > class FixedSizeMarkedMemoryData;
	template< typename Object > class FixedGrowingSizeMarkedMemoryData;
}

constexpr castor::real operator "" _r( long double p_value )
{
	return castor::real( p_value );
}

constexpr std::chrono::seconds operator "" _s( unsigned long long p_value )
{
	return std::chrono::seconds( int64_t( p_value ) );
}

constexpr castor::Milliseconds operator "" _ms( unsigned long long p_value )
{
	return castor::Milliseconds( int64_t( p_value ) );
}

constexpr std::chrono::microseconds operator "" _us( unsigned long long p_value )
{
	return std::chrono::microseconds( int64_t( p_value ) );
}

constexpr castor::Nanoseconds operator "" _ns( unsigned long long p_value )
{
	return castor::Nanoseconds( int64_t( p_value ) );
}

#include "Miscellaneous/Debug.hpp"

#endif
