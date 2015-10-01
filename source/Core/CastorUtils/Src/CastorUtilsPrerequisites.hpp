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
#ifndef ___CASTOR_UTILS_PREREQUISITES_H___
#define ___CASTOR_UTILS_PREREQUISITES_H___

#include "Macros.hpp"

#if CASTOR_HAS_STDARRAY || CASTOR_HAS_TR1ARRAY
#	include <array>
#else
#	include <boost/array.hpp>
#endif

#include <vector>
#include <list>
#include <map>
#include <set>
#include <string>
#include <cstdint>
#include <deque>

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
#	define TPL_PIXEL_FORMAT	uint32_t
#else
#	define TPL_PIXEL_FORMAT	ePIXEL_FORMAT
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
	CASTOR_TYPE( uint8_t )
	{
		eFILE_TYPE_BINARY,
		eFILE_TYPE_TEXT,
	}	eFILE_TYPE;
	/*!
	\~english
	\brief Pixel format enumeration
	\~french
	\brief Enumération des formats de Pixel
	*/
	typedef enum ePIXEL_FORMAT
	CASTOR_TYPE( uint8_t )
	{
		ePIXEL_FORMAT_L8,			//!< 8 bits luminosity
		ePIXEL_FORMAT_L16F32F,		//!< Half floats luminosity on GPU, floats luminosity on CPU
		ePIXEL_FORMAT_L32F,			//!< Floats luminosity on CPU
		ePIXEL_FORMAT_A8L8,			//!< 16 bits alpha and luminosity
		ePIXEL_FORMAT_AL16F32F,		//!< Half floats alpha and luminosity on GPU, floats alpha and luminosity on CPU
		ePIXEL_FORMAT_AL32F,		//!< Floats alpha and luminosity on CPU
		ePIXEL_FORMAT_A1R5G5B5,		//!< 16 bits 5551 ARGB
		ePIXEL_FORMAT_A4R4G4B4,		//!< 16 bits 4444 ARGB
		ePIXEL_FORMAT_R5G6B5,		//!< 16 bits 565 RGB
		ePIXEL_FORMAT_R8G8B8,		//!< 24 bits 888 RGB
		ePIXEL_FORMAT_B8G8R8,		//!< 24 bits 888 BGR
		ePIXEL_FORMAT_A8R8G8B8,		//!< 32 bits 8888 ARGB
		ePIXEL_FORMAT_A8B8G8R8,		//!< 32 bits 8888 ABGR
		ePIXEL_FORMAT_RGB16F32F,	//!< Half float RGB on GPU, Float RGB on CPU
		ePIXEL_FORMAT_ARGB16F32F,	//!< Half float ARGB on GPU, Float ARGB on CPU
		ePIXEL_FORMAT_RGB32F,		//!< Float RGB
		ePIXEL_FORMAT_ARGB32F,		//!< Float ARGB
		ePIXEL_FORMAT_DXTC1,		//!< DXT1 8 bits compressed format
		ePIXEL_FORMAT_DXTC3,		//!< DXT3 16 bits compressed format
		ePIXEL_FORMAT_DXTC5,		//!< DXT5 16 bits compressed format
		ePIXEL_FORMAT_YUY2,			//!< YUY2 16 bits compressed format
		ePIXEL_FORMAT_DEPTH16,		//!< Depth 16 bits
		ePIXEL_FORMAT_DEPTH24,		//!< Depth 24 bits
		ePIXEL_FORMAT_DEPTH24S8,	//!< Depth 24 bits, Stencil 8 bits
		ePIXEL_FORMAT_DEPTH32,		//!< Depth 32 bits
		ePIXEL_FORMAT_STENCIL1,		//!< Stencil 1 bit
		ePIXEL_FORMAT_STENCIL8,		//!< Stencil 8 bits
		ePIXEL_FORMAT_COUNT,
	}	ePIXEL_FORMAT;

#if CASTOR_USE_DOUBLE
	typedef double real;
#else
	typedef float real;
#endif

	// static size array
#if CASTOR_HAS_STDARRAY
	using std::array;
#elif CASTOR_HAS_TR1ARRAY
	using std::tr1::array;
#else
	using boost::array;
#endif
	typedef std::basic_string< xchar > String;
	typedef std::basic_stringstream< xchar > StringStream;
	typedef std::basic_ostream< xchar > OutputStream;

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
	class Colour;
	template< typename T >
	class Line2D;
	template< typename T >
	class Line3D;
	template< class T, eFILE_TYPE FT, class TFile >
	class Loader;
	class ILoggerImpl;
	class Logger;
	template< typename T, uint32_t Rows, uint32_t Columns >
	class Matrix;
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
	class Quaternion;
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
			  typename PFNCreate = std::function< std::shared_ptr< Obj >() >,
			  class Predicate = std::less< Key > >
	class Factory;
	class Path;
	class DynamicLibrary;
	template< int A >
	struct Aligned;
	struct MessageBase;
	class Logger;
	class LoggerImpl;
	class ProgramConsole;

	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Typedef over a buffer of pixels in format A8R8G8B8
	\~french
	\brief		Typedef sur un buffer de pixels au format A8R8G8B8
	*/
	typedef PxBuffer< ePIXEL_FORMAT_A8R8G8B8 > PixelBuffer;

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
	DECLARE_MAP( String, bool, BoolStr );
	DECLARE_MAP( String, String, StrStr );
	DECLARE_SET( String, Str );

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
}

#endif
