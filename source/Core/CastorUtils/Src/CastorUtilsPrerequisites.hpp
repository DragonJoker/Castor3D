/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

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
#ifndef ___CASTOR_UTILS_PREREQUISITES_H___
#define ___CASTOR_UTILS_PREREQUISITES_H___

#include "Macros.hpp"

#include <array>
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

#include "SmartPtr.hpp"
#include "ELogType.hpp"

#if defined( min )
#	undef min
#endif
#if defined( max )
#	undef max
#endif
#if defined( abs )
#	undef abs
#endif

#if defined( _MSC_VER )
#	if ( _MSC_VER < 1900 )
#		define TPL_PIXEL_FORMAT	uint32_t
#	else
#		define TPL_PIXEL_FORMAT	Castor::ePIXEL_FORMAT
#	endif
#else
#	define TPL_PIXEL_FORMAT	Castor::ePIXEL_FORMAT
#endif

namespace Castor
{
	/*!
	\author Sylvain DOREMUS
	\version 0.6.1.0
	\date 19/10/2011
	\~english
	\brief Castor::Loader supported file types enumeration
	\~french
	\brief Enumération des types de fichier supportés par Castor::Loader
	*/
	typedef enum eFILE_TYPE
		: uint8_t
	{
		//! Binary file
		eFILE_TYPE_BINARY,
		//! Text file
		eFILE_TYPE_TEXT,
		CASTOR_ENUM_BOUNDS( eFILE_TYPE, eFILE_TYPE_BINARY )
	}	eFILE_TYPE;
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		24/05/2016
	\~english
	\brief		Helper structure to find File type (BinaryFile or TextFile) from eFILE_TYPE.
	\~french
	\brief		Structure d'aide pour trouver le type de File (BinaryFile ou TextFile) à partir d'un eFILE_TYPE.
	*/
	template< eFILE_TYPE FT >
	struct FileTyper;
	/*!
	\~english
	\brief Pixel format enumeration
	\~french
	\brief Enumération des formats de Pixel
	*/
	typedef enum ePIXEL_FORMAT
		: uint8_t
	{
		//!\~english 8 bits luminosity	\~french Luminosité 8 bits
		ePIXEL_FORMAT_L8,
		//!\~english Half floats luminosity on VRAM, floats luminosity on RAM	\~french Luminosité en half float en VRAM, et en float en RAM
		ePIXEL_FORMAT_L16F32F,
		//!\~english Floats luminosity on CPU	\~french Luminosité en float
		ePIXEL_FORMAT_L32F,
		//!\~english 16 bits alpha and luminosity	\~french Alpha et luminosité 16 bits
		ePIXEL_FORMAT_A8L8,
		//!\~english Half floats alpha and luminosity on VRAM, floats alpha and luminosity on RAM	\~french Luminosité et alpha en half float en VRAM, et en float en RAM
		ePIXEL_FORMAT_AL16F32F,
		//!\~english Floats alpha and luminosity on CPU	\~french Luminosité et alpha en float
		ePIXEL_FORMAT_AL32F,
		//!\~english 16 bits 5551 ARGB	\~french 16 bits 5551 ARGB
		ePIXEL_FORMAT_A1R5G5B5,
		//!\~english 16 bits 4444 ARGB	\~french 16 bits 4444 ARGB
		ePIXEL_FORMAT_A4R4G4B4,
		//!\~english 16 bits 565 RGB	\~french 16 bits 565 RGB
		ePIXEL_FORMAT_R5G6B5,
		//!\~english 24 bits 888 RGB	\~french 24 bits 888 RGB
		ePIXEL_FORMAT_R8G8B8,
		//!\~english 24 bits 888 BGR	\~french 24 bits 888 BGR
		ePIXEL_FORMAT_B8G8R8,
		//!\~english 32 bits 8888 ARGB	\~french 32 bits 8888 ARGB
		ePIXEL_FORMAT_A8R8G8B8,
		//!\~english 32 bits 8888 ABGR	\~french 32 bits 8888 ABGR
		ePIXEL_FORMAT_A8B8G8R8,
		//!\~english Half float RGB	\~french Half float RGB
		ePIXEL_FORMAT_RGB16F,
		//!\~english Half float ARGB	\~french Half float ARGB
		ePIXEL_FORMAT_ARGB16F,
		//!\~english Half float RGB on VRAM, Float RGB on RAM	\~french RGB en half float en VRAM, et en float en RAM
		ePIXEL_FORMAT_RGB16F32F,
		//!\~english Half float ARGB on VRAM, Float ARGB on RAM	\~french ARGB en half float en VRAM, et en float en RAM
		ePIXEL_FORMAT_ARGB16F32F,
		//!\~english Float RGB	\~french RGB en flottants 32 bits
		ePIXEL_FORMAT_RGB32F,
		//!\~english Float ARGB	\~french ARGB en flottants 32 bits
		ePIXEL_FORMAT_ARGB32F,
		//!\~english DXT1 8 bits compressed format	\~french Format compressé DXT1 8 bits
		ePIXEL_FORMAT_DXTC1,
		//!\~english DXT3 16 bits compressed format	\~french Format compressé DXT3 16 bits
		ePIXEL_FORMAT_DXTC3,
		//!\~english DXT5 16 bits compressed format	\~french Format compressé DXT5 16 bits
		ePIXEL_FORMAT_DXTC5,
		//!\~english YUY2 16 bits compressed format	\~french Format compressé YUY2 16 bits
		ePIXEL_FORMAT_YUY2,
		//!\~english Depth 16 bits	\~french Profondeur 16 bits
		ePIXEL_FORMAT_DEPTH16,
		//!\~english Depth 24 bits	\~french Profondeur 24 bits
		ePIXEL_FORMAT_DEPTH24,
		//!\~english Depth 24 bits, Stencil 8 bits	\~french Profondeur 24 bits, Stencil 8 bits
		ePIXEL_FORMAT_DEPTH24S8,
		//!\~english Depth 32 bits	\~french Profondeur 32 bits
		ePIXEL_FORMAT_DEPTH32,
		//!\~english Depth 32 bits floating point	\~french Profondeur en float
		ePIXEL_FORMAT_DEPTH32F,
		//!\~english Stencil 1 bit	\~french Stencil 1 bit
		ePIXEL_FORMAT_STENCIL1,
		//!\~english Stencil 8 bits	\~french Stencil 8 bits
		ePIXEL_FORMAT_STENCIL8,
		CASTOR_ENUM_BOUNDS( ePIXEL_FORMAT, ePIXEL_FORMAT_L8 )
	}	ePIXEL_FORMAT;

#if CASTOR_USE_DOUBLE
	typedef double real;
#else
	typedef float real;
#endif

	typedef std::basic_string< xchar > String;
	typedef std::basic_regex< xchar > Regex;
	typedef std::regex_iterator< String::const_iterator > SRegexIterator;
	typedef std::match_results< String::const_iterator > MatchResults;
	typedef std::basic_stringstream< xchar > StringStream;
	typedef std::basic_ostream< xchar > OutputStream;

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

	class Angle;
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
	class Font;
	class FontManager;
	class Glyph;
	class Image;
	class ImageManager;
	class Colour;
	template< typename T >
	class Line2D;
	template< typename T >
	class Line3D;
	template< class T, eFILE_TYPE FT >
	class Loader;
	class ILoggerImpl;
	class Logger;
	template< typename T, uint32_t Rows, uint32_t Columns >
	class Matrix;
	template< class Owmer >
	class OwnedBy;
	class ParserParameterBase;
	template< TPL_PIXEL_FORMAT FT >
	class Pixel;
	template< typename T >
	class PlaneEquation;
	template< typename T, uint32_t Count >
	class Point;
	class Position;
	class PxBufferBase;
	template< TPL_PIXEL_FORMAT FT >
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
	template< class Obj,
			  class Key,
			  class PtrType = std::shared_ptr< Obj >,
			  typename PFNCreate = std::function< std::shared_ptr< Obj >() >,
			  class Predicate = std::less< Key > >
	class Factory;
	class Path;
	class DynamicLibrary;
	struct MessageBase;
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
	typedef PxBuffer< ePIXEL_FORMAT_A8R8G8B8 > PixelBuffer;

	template< typename T > using Point2 = Point< T, 2 >;
	template< typename T > using Point3 = Point< T, 3 >;
	template< typename T > using Point4 = Point< T, 4 >;

	template< typename T > using Coords2 = Coords< T, 2 >;
	template< typename T > using Coords3 = Coords< T, 3 >;
	template< typename T > using Coords4 = Coords< T, 4 >;

	using Quaternion = QuaternionT< double >;

	DECLARE_POINT( bool,		4, b	);
	DECLARE_POINT( bool,		3, b	);
	DECLARE_POINT( bool,		2, b	);
	DECLARE_POINT( int8_t,		4, c	);
	DECLARE_POINT( int8_t,		3, c	);
	DECLARE_POINT( int8_t,		2, c	);
	DECLARE_POINT( uint8_t,		4, ub	);
	DECLARE_POINT( uint8_t,		3, ub	);
	DECLARE_POINT( uint8_t,		2, ub	);
	DECLARE_POINT( int16_t,		4, s	);
	DECLARE_POINT( int16_t,		3, s	);
	DECLARE_POINT( int16_t,		2, s	);
	DECLARE_POINT( uint16_t,	4, us	);
	DECLARE_POINT( uint16_t,	3, us	);
	DECLARE_POINT( uint16_t,	2, us	);
	DECLARE_POINT( int32_t,		4, i	);
	DECLARE_POINT( int32_t,		3, i	);
	DECLARE_POINT( int32_t,		2, i	);
	DECLARE_POINT( uint32_t,	4, ui	);
	DECLARE_POINT( uint32_t,	3, ui	);
	DECLARE_POINT( uint32_t,	2, ui	);
	DECLARE_POINT( real,		4, r	);
	DECLARE_POINT( real,		3, r	);
	DECLARE_POINT( real,		2, r	);
	DECLARE_POINT( float,		4, f	);
	DECLARE_POINT( float,		3, f	);
	DECLARE_POINT( float,		2, f	);
	DECLARE_POINT( double,		4, d	);
	DECLARE_POINT( double,		3, d	);
	DECLARE_POINT( double,		2, d	);

	DECLARE_COORD( bool,		4, b	);
	DECLARE_COORD( bool,		3, b	);
	DECLARE_COORD( bool,		2, b	);
	DECLARE_COORD( int8_t,		4, c	);
	DECLARE_COORD( int8_t,		3, c	);
	DECLARE_COORD( int8_t,		2, c	);
	DECLARE_COORD( uint8_t,		4, ub	);
	DECLARE_COORD( uint8_t,		3, ub	);
	DECLARE_COORD( uint8_t,		2, ub	);
	DECLARE_COORD( int16_t,		4, s	);
	DECLARE_COORD( int16_t,		3, s	);
	DECLARE_COORD( int16_t,		2, s	);
	DECLARE_COORD( uint16_t,	4, us	);
	DECLARE_COORD( uint16_t,	3, us	);
	DECLARE_COORD( uint16_t,	2, us	);
	DECLARE_COORD( int32_t,		4, i	);
	DECLARE_COORD( int32_t,		3, i	);
	DECLARE_COORD( int32_t,		2, i	);
	DECLARE_COORD( uint32_t,	4, ui	);
	DECLARE_COORD( uint32_t,	3, ui	);
	DECLARE_COORD( uint32_t,	2, ui	);
	DECLARE_COORD( int,			4, i	);
	DECLARE_COORD( int,			3, i	);
	DECLARE_COORD( int,			2, i	);
	DECLARE_COORD( real,		4, r	);
	DECLARE_COORD( real,		3, r	);
	DECLARE_COORD( real,		2, r	);
	DECLARE_COORD( float,		4, f	);
	DECLARE_COORD( float,		3, f	);
	DECLARE_COORD( float,		2, f	);
	DECLARE_COORD( double,		4, d	);
	DECLARE_COORD( double,		3, d	);
	DECLARE_COORD( double,		2, d	);
	DECLARE_COORD( uint8_t,		4, ub	);
	DECLARE_COORD( uint8_t,		3, ub	);
	DECLARE_COORD( uint8_t,		2, ub	);

	DECLARE_SQMTX( real,	4, r	);
	DECLARE_SQMTX( real,	3, r	);
	DECLARE_SQMTX( real,	2, r	);
	DECLARE_SQMTX( float,	4, f	);
	DECLARE_SQMTX( float,	3, f	);
	DECLARE_SQMTX( float,	2, f	);
	DECLARE_SQMTX( double,	4, d	);
	DECLARE_SQMTX( double,	3, d	);
	DECLARE_SQMTX( double,	2, d	);

	DECLARE_MTX( real,		2, 3, r	);
	DECLARE_MTX( real,		2, 4, r	);
	DECLARE_MTX( real,		3, 2, r	);
	DECLARE_MTX( real,		3, 4, r	);
	DECLARE_MTX( real,		4, 2, r	);
	DECLARE_MTX( real,		4, 3, r	);
	DECLARE_MTX( float,		2, 3, f	);
	DECLARE_MTX( float,		2, 4, f	);
	DECLARE_MTX( float,		3, 2, f	);
	DECLARE_MTX( float,		3, 4, f	);
	DECLARE_MTX( float,		4, 2, f	);
	DECLARE_MTX( float,		4, 3, f	);
	DECLARE_MTX( double,	2, 3, d	);
	DECLARE_MTX( double,	2, 4, d	);
	DECLARE_MTX( double,	3, 2, d	);
	DECLARE_MTX( double,	3, 4, d	);
	DECLARE_MTX( double,	4, 2, d	);
	DECLARE_MTX( double,	4, 3, d	);

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

	typedef std::deque< std::unique_ptr< MessageBase > > MessageQueue;
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
	typedef std::function< void ( String const & p_strLog, ELogType p_eLogType ) > LogCallback;

	/*!
	\~english
	\brief		Supported MemoryData types.
	\~french
	\brief		Types de MemoryData supportés.
	*/
	typedef enum eMEMDATA_TYPE
	{
		eMEMDATA_TYPE_FIXED,
		eMEMDATA_TYPE_FIXED_MARKED,
		eMEMDATA_TYPE_FIXED_GROWING,
		eMEMDATA_TYPE_FIXED_GROWING_MARKED,
		CASTOR_ENUM_BOUNDS( eMEMDATA_TYPE, eMEMDATA_TYPE_FIXED )
	}	eMEMDATA_TYPE;

	template< typename Object, eMEMDATA_TYPE MemDataType > class PoolManagedObject;

	class NonAlignedMemoryAllocator;
	template< size_t Align > class AlignedMemoryAllocator;

	template< typename Object, typename MemoryAllocator = NonAlignedMemoryAllocator > class FixedSizeMemoryData;
	template< typename Object, typename MemoryAllocator = NonAlignedMemoryAllocator > class FixedGrowingSizeMemoryData;
	template< typename Object > class FixedSizeMarkedMemoryData;
	template< typename Object > class FixedGrowingSizeMarkedMemoryData;
}

constexpr Castor::real operator "" _r( long double p_value )
{
	return Castor::real( p_value );
}

#include "Debug.hpp"

#endif
