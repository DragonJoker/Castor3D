/*
See LICENSE file in root folder
*/
#ifndef ___CastorUtils_Prerequisites_HPP___
#define ___CastorUtils_Prerequisites_HPP___
#pragma once

#ifndef NOMINMAX
#	define NOMINMAX
#endif

#include <array>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <functional>
#include <iostream>
#include <list>
#include <locale>
#include <map>
#include <map>
#include <memory>
#include <regex>
#include <set>
#include <sstream>
#include <string>
#include <deque>
#include <sstream>
#include <vector>

#include "CastorUtils/Config/SmartPtr.hpp"
#include "CastorUtils/Log/ELogType.hpp"

namespace castor
{
	/**@name File handling */
	//@{
	/**
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
		CU_ScopedEnumBounds( eBinary )
	};
	/**
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
	//@}
	/**@name Graphics */
	//@{
	/**
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
		eR8_UNORM,
		//!\~english	Half floats luminosity on VRAM, floats luminosity on RAM.
		//!\~french		Luminosité en half float en VRAM, et en float en RAM.
		eR16_SFLOAT,
		//!\~english	32 bits loats luminosity.
		//!\~french		Luminosité en float 32 bits.
		eR32_SFLOAT,
		//!\~english	16 bits alpha and luminosity.
		//!\~french		Alpha et luminosité 16 bits.
		eR8A8_UNORM,
		//!\~english	Half floats alpha and luminosity on VRAM, floats alpha and luminosity on RAM.
		//!\~french		Luminosité et alpha en half float en VRAM, et en float en RAM.
		eR16A16_SFLOAT,
		//!\~english	32 bits floats alpha and luminosity.
		//!\~french		Luminosité et alpha en float 32 bits.
		eR32A32_SFLOAT,
		//!\~english	16 bits 5551 ARGB.
		//!\~french		16 bits 5551 ARGB.
		eR5G5B5A1_UNORM,
		//!\~english	16 bits 565 RGB.
		//!\~french		16 bits 565 RGB.
		eR5G6B5_UNORM,
		//!\~english	24 bits 888 RGB.
		//!\~french		24 bits 888 RGB.
		eR8G8B8_UNORM,
		//!\~english	24 bits 888 BGR.
		//!\~french		24 bits 888 BGR.
		eB8G8R8_UNORM,
		//!\~english	24 bits 888 RGB in sRGB colour space.
		//!\~french		24 bits 888 RGB dans l'espace de couleurs sRGB.
		eR8G8B8_SRGB,
		//!\~english	24 bits 888 BGR in sRGB colour space.
		//!\~french		24 bits 888 BGR dans l'espace de couleurs sRGB.
		eB8G8R8_SRGB,
		//!\~english	32 bits 8888 RGBA.
		//!\~french		32 bits 8888 RGBA.
		eR8G8B8A8_UNORM,
		//!\~english	32 bits 8888 ABGR.
		//!\~french		32 bits 8888 ABGR.
		eA8B8G8R8_UNORM,
		//!\~english	32 bits 8888 RGBA in sRGB colour space.
		//!\~french		32 bits 8888 RGBA dans l'espace de couleurs sRGB.
		eR8G8B8A8_SRGB,
		//!\~english	32 bits 8888 ABGR in sRGB colour space.
		//!\~french		32 bits 8888 ABGR dans l'espace de couleurs sRGB.
		eA8B8G8R8_SRGB,
		//!\~english	Half float RGB.
		//!\~french		Half float RGB.
		eR16G16B16_SFLOAT,
		//!\~english	Half float ARGB.
		//!\~french		Half float ARGB.
		eR16G16B16A16_SFLOAT,
		//!\~english	32 bits float RGB.
		//!\~french		RGB en flottants 32 bits.
		eR32G32B32_SFLOAT,
		//!\~english	32 bits float ARGB.
		//!\~french		ARGB en flottants 32 bits.
		eR32G32B32A32_SFLOAT,
		//!\~english	DXT1 8 bits compressed format.
		//!\~french		Format compressé DXT1 8 bits.
		eBC1_RGB_UNORM_BLOCK,
		//!\~english	DXT3 16 bits compressed format.
		//!\~french		Format compressé DXT3 16 bits.
		eBC3_UNORM_BLOCK,
		//!\~english	DXT5 16 bits compressed format.
		//!\~french		Format compressé DXT5 16 bits.
		eBC5_UNORM_BLOCK,
		//!\~english	Depth 16 bits.
		//!\~french		Profondeur 16 bits.
		eD16_UNORM,
		//!\~english	Depth 24 bits, Stencil 8 bits.
		//!\~french		Profondeur 24 bits, Stencil 8 bits.
		eD24_UNORM_S8_UINT,
		//!\~english	Depth 32 bits floating point.
		//!\~french		Profondeur en float 32 bits.
		eD32_UNORM,
		//!\~english	Depth 32 bits floating point.
		//!\~french		Profondeur en float 32 bits.
		eD32_SFLOAT,
		//!\~english	Depth 32 bits floating point, Stencil 8 bits.
		//!\~french		Profondeur en float 32 bits, Stencil 8 bits.
		eD32_SFLOAT_S8_UINT,
		//!\~english	Stencil 8 bits.
		//!\~french		Stencil 8 bits.
		eS8_UINT,
		CU_ScopedEnumBounds( eR8_UNORM )
	};
	/**
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
		CU_ScopedEnumBounds( eIn )
	};
	//@}
	/**@name STL typedefs */
	//@{
	using String = std::basic_string< xchar >;
	using Regex = std::basic_regex< xchar >;
	using RegexIterator = std::regex_iterator< String::const_iterator >;
	using MatchResults = std::match_results< String::const_iterator >;
	using StringStream = std::basic_stringstream< xchar >;
	using OutputStringStream = std::basic_ostringstream< xchar >;
	using InputStringStream = std::basic_istringstream< xchar >;
	using OutputStream = std::basic_ostream< xchar >;
	using InputStream = std::basic_istream< xchar >;
	using Seconds = std::chrono::seconds;
	using Milliseconds = std::chrono::milliseconds;
	using Microseconds = std::chrono::microseconds;
	using Nanoseconds = std::chrono::nanoseconds;
	//@}
	/**
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
	static NoInit constexpr noInit;

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
	class BoundingContainer;
	template< typename T, uint32_t Count >
	class Coords;
	class BoundingBox;
	template< typename BlockType >
	class DynamicBitsetT;
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
	class ImageLoader;
	class ImageWriter;
	class ColourComponent;
	class HdrColourComponent;
	template< typename ComponentType >
	class RgbColourT;
	template< typename ComponentType >
	class RgbaColourT;
	using RgbColour = RgbColourT< ColourComponent >;
	using RgbaColour = RgbaColourT< ColourComponent >;
	using HdrRgbColour = RgbColourT< HdrColourComponent >;
	using HdrRgbaColour = RgbaColourT< HdrColourComponent >;
	template< typename T >
	class Line2D;
	template< typename T >
	class Line3D;
	template< class T, FileType FT >
	class Loader;
	class LoggerImpl;
	class LoggerInstance;
	class Logger;
	template< typename CharT
		, template< typename CharT > typename StreambufT >
	struct LoggerStreamT;
	template< typename CharT, typename TraitsT >
	class LoggerStreambufT;
	template< typename T, uint32_t Rows, uint32_t Columns >
	class Matrix;
	template< class Owmer >
	class OwnedBy;
	class ParserParameterBase;
	template< PixelFormat FT >
	class Pixel;
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
	class BoundingSphere;
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

	using PixelBuffer = PxBuffer< PixelFormat::eR8G8B8A8_UNORM >;
	using DynamicBitset = DynamicBitsetT< uint32_t >;

	template< typename T > using Point2 = Point< T, 2 >;
	template< typename T > using Point3 = Point< T, 3 >;
	template< typename T > using Point4 = Point< T, 4 >;

	template< typename T > using Coords2 = Coords< T, 2 >;
	template< typename T > using Coords3 = Coords< T, 3 >;
	template< typename T > using Coords4 = Coords< T, 4 >;

	template< typename T > using Matrix2x2 = SquareMatrix< T, 2 >;
	template< typename T > using Matrix3x3 = SquareMatrix< T, 3 >;
	template< typename T > using Matrix4x4 = SquareMatrix< T, 4 >;

	using Angle = AngleT< float >;
	using Quaternion = QuaternionT< float >;

	CU_DeclarePoint( bool,		4, b );
	CU_DeclarePoint( bool,		3, b );
	CU_DeclarePoint( bool,		2, b );
	CU_DeclarePoint( int8_t,	4, c );
	CU_DeclarePoint( int8_t,	3, c );
	CU_DeclarePoint( int8_t,	2, c );
	CU_DeclarePoint( uint8_t,	4, ub );
	CU_DeclarePoint( uint8_t,	3, ub );
	CU_DeclarePoint( uint8_t,	2, ub );
	CU_DeclarePoint( int16_t,	4, s );
	CU_DeclarePoint( int16_t,	3, s );
	CU_DeclarePoint( int16_t,	2, s );
	CU_DeclarePoint( uint16_t,	4, us );
	CU_DeclarePoint( uint16_t,	3, us );
	CU_DeclarePoint( uint16_t,	2, us );
	CU_DeclarePoint( int32_t,	4, i );
	CU_DeclarePoint( int32_t,	3, i );
	CU_DeclarePoint( int32_t,	2, i );
	CU_DeclarePoint( uint32_t,	4, ui );
	CU_DeclarePoint( uint32_t,	3, ui );
	CU_DeclarePoint( uint32_t,	2, ui );
	CU_DeclarePoint( float,		4, f );
	CU_DeclarePoint( float,		3, f );
	CU_DeclarePoint( float,		2, f );
	CU_DeclarePoint( double,	4, d );
	CU_DeclarePoint( double,	3, d );
	CU_DeclarePoint( double,	2, d );

	CU_DeclareCoord( bool,		4, b );
	CU_DeclareCoord( bool,		3, b );
	CU_DeclareCoord( bool,		2, b );
	CU_DeclareCoord( int8_t,	4, c );
	CU_DeclareCoord( int8_t,	3, c );
	CU_DeclareCoord( int8_t,	2, c );
	CU_DeclareCoord( uint8_t,	4, ub );
	CU_DeclareCoord( uint8_t,	3, ub );
	CU_DeclareCoord( uint8_t,	2, ub );
	CU_DeclareCoord( int16_t,	4, s );
	CU_DeclareCoord( int16_t,	3, s );
	CU_DeclareCoord( int16_t,	2, s );
	CU_DeclareCoord( uint16_t,	4, us );
	CU_DeclareCoord( uint16_t,	3, us );
	CU_DeclareCoord( uint16_t,	2, us );
	CU_DeclareCoord( int32_t,	4, i );
	CU_DeclareCoord( int32_t,	3, i );
	CU_DeclareCoord( int32_t,	2, i );
	CU_DeclareCoord( uint32_t,	4, ui );
	CU_DeclareCoord( uint32_t,	3, ui );
	CU_DeclareCoord( uint32_t,	2, ui );
	CU_DeclareCoord( int32_t,	4, i );
	CU_DeclareCoord( int32_t,	3, i );
	CU_DeclareCoord( int32_t,	2, i );
	CU_DeclareCoord( float,		4, f );
	CU_DeclareCoord( float,		3, f );
	CU_DeclareCoord( float,		2, f );
	CU_DeclareCoord( double,	4, d );
	CU_DeclareCoord( double,	3, d );
	CU_DeclareCoord( double,	2, d );
	CU_DeclareCoord( uint8_t,	4, ub );
	CU_DeclareCoord( uint8_t,	3, ub );
	CU_DeclareCoord( uint8_t,	2, ub );

	CU_DeclareConstCoord( bool,		4, b );
	CU_DeclareConstCoord( bool,		3, b );
	CU_DeclareConstCoord( bool,		2, b );
	CU_DeclareConstCoord( int8_t,	4, c );
	CU_DeclareConstCoord( int8_t,	3, c );
	CU_DeclareConstCoord( int8_t,	2, c );
	CU_DeclareConstCoord( uint8_t,	4, ub );
	CU_DeclareConstCoord( uint8_t,	3, ub );
	CU_DeclareConstCoord( uint8_t,	2, ub );
	CU_DeclareConstCoord( int16_t,	4, s );
	CU_DeclareConstCoord( int16_t,	3, s );
	CU_DeclareConstCoord( int16_t,	2, s );
	CU_DeclareConstCoord( uint16_t,	4, us );
	CU_DeclareConstCoord( uint16_t,	3, us );
	CU_DeclareConstCoord( uint16_t,	2, us );
	CU_DeclareConstCoord( int32_t,	4, i );
	CU_DeclareConstCoord( int32_t,	3, i );
	CU_DeclareConstCoord( int32_t,	2, i );
	CU_DeclareConstCoord( uint32_t,	4, ui );
	CU_DeclareConstCoord( uint32_t,	3, ui );
	CU_DeclareConstCoord( uint32_t,	2, ui );
	CU_DeclareConstCoord( int32_t,	4, i );
	CU_DeclareConstCoord( int32_t,	3, i );
	CU_DeclareConstCoord( int32_t,	2, i );
	CU_DeclareConstCoord( float,	4, f );
	CU_DeclareConstCoord( float,	3, f );
	CU_DeclareConstCoord( float,	2, f );
	CU_DeclareConstCoord( double,	4, d );
	CU_DeclareConstCoord( double,	3, d );
	CU_DeclareConstCoord( double,	2, d );
	CU_DeclareConstCoord( uint8_t,	4, ub );
	CU_DeclareConstCoord( uint8_t,	3, ub );
	CU_DeclareConstCoord( uint8_t,	2, ub );
	
	CU_DeclareSqMtx( bool,		4, b );
	CU_DeclareSqMtx( bool,		3, b );
	CU_DeclareSqMtx( bool,		2, b );
	CU_DeclareSqMtx( int32_t,	4, i );
	CU_DeclareSqMtx( int32_t,	3, i );
	CU_DeclareSqMtx( int32_t,	2, i );
	CU_DeclareSqMtx( uint32_t,	4, ui );
	CU_DeclareSqMtx( uint32_t,	3, ui );
	CU_DeclareSqMtx( uint32_t,	2, ui );
	CU_DeclareSqMtx( float,		4, f );
	CU_DeclareSqMtx( float,		3, f );
	CU_DeclareSqMtx( float,		2, f );
	CU_DeclareSqMtx( double,	4, d );
	CU_DeclareSqMtx( double,	3, d );
	CU_DeclareSqMtx( double,	2, d );

	CU_DeclareMtx( bool,		2, 3, b );
	CU_DeclareMtx( bool,		2, 4, b );
	CU_DeclareMtx( bool,		3, 2, b );
	CU_DeclareMtx( bool,		3, 4, b );
	CU_DeclareMtx( bool,		4, 2, b );
	CU_DeclareMtx( bool,		4, 3, b );
	CU_DeclareMtx( int32_t,		2, 3, i );
	CU_DeclareMtx( int32_t,		2, 4, i );
	CU_DeclareMtx( int32_t,		3, 2, i );
	CU_DeclareMtx( int32_t,		3, 4, i );
	CU_DeclareMtx( int32_t,		4, 2, i );
	CU_DeclareMtx( int32_t,		4, 3, i );
	CU_DeclareMtx( uint32_t,	2, 3, ui );
	CU_DeclareMtx( uint32_t,	2, 4, ui );
	CU_DeclareMtx( uint32_t,	3, 2, ui );
	CU_DeclareMtx( uint32_t,	3, 4, ui );
	CU_DeclareMtx( uint32_t,	4, 2, ui );
	CU_DeclareMtx( uint32_t,	4, 3, ui );
	CU_DeclareMtx( float,		2, 3, f );
	CU_DeclareMtx( float,		2, 4, f );
	CU_DeclareMtx( float,		3, 2, f );
	CU_DeclareMtx( float,		3, 4, f );
	CU_DeclareMtx( float,		4, 2, f );
	CU_DeclareMtx( float,		4, 3, f );
	CU_DeclareMtx( double,		2, 3, d );
	CU_DeclareMtx( double,		2, 4, d );
	CU_DeclareMtx( double,		3, 2, d );
	CU_DeclareMtx( double,		3, 4, d );
	CU_DeclareMtx( double,		4, 2, d );
	CU_DeclareMtx( double,		4, 3, d );

	CU_DeclareSmartPtr( Quaternion );
	CU_DeclareSmartPtr( SphericalVertex );
	CU_DeclareSmartPtr( BoundingBox );
	CU_DeclareSmartPtr( BoundingSphere );
	CU_DeclareSmartPtr( Image );
	CU_DeclareSmartPtr( Font );
	CU_DeclareSmartPtr( PxBufferBase );
	CU_DeclareSmartPtr( FileParserContext );
	CU_DeclareSmartPtr( ParserParameterBase );
	CU_DeclareSmartPtr( DynamicLibrary );

	CU_DeclareVector( uint8_t, Byte );
	CU_DeclareVector( SphericalVertexSPtr, SphericalVertexPtr );
	CU_DeclareVector( int32_t, Int32 );
	CU_DeclareVector( uint32_t, UInt32 );
	CU_DeclareVector( String, String );
	CU_DeclareVector( Path, Path );
	CU_DeclareVector( ParserParameterBaseSPtr, ParserParameter );
	CU_DeclareVector( PxBufferBaseSPtr, PxBufferPtr );
	CU_DeclareMap( String, uint32_t, UInt32Str );
	CU_DeclareMap( String, uint64_t, UInt64Str );
	CU_DeclareMap( String, bool, BoolStr );
	CU_DeclareMap( String, String, StrStr );
	CU_DeclareSet( String, Str );
	CU_DeclareMap( uint32_t, String, StrUInt32 );
	/**
	 *\~english
	 *\brief		Logging callback function.
	 *\param[in]	text	The logged text.
	 *\param[in]	type	The log type.
	 *\param[in]	newLine	Tells if we add the end line character.
	 *\~french
	 *\brief		Fonction de callback de log.
	 *\param[in]	text	Le texte écrit.
	 *\param[in]	type	Le type de log.
	 *\param[in]	newLine	Dit si on ajoute le caractère de fin de ligne.
	 */
	using LogCallback = std::function< void ( String const & text, LogType type, bool newLine ) >;
	/**@name Memory pool */
	//@{
	class NonAlignedMemoryAllocator;
	template< size_t Align >
	class AlignedMemoryAllocator;

	template< typename Object, typename MemoryAllocator = NonAlignedMemoryAllocator >
	class FixedSizeMemoryData;
	template< typename Object, typename MemoryAllocator = NonAlignedMemoryAllocator >
	class FixedGrowingSizeMemoryData;
	template< typename Object >
	class FixedSizeMarkedMemoryData;
	template< typename Object >
	class FixedGrowingSizeMarkedMemoryData;
	//@}
	/**
	 *\~english
	 *\return		A std::stringstream using C locale.
	 *\~french
	 *\return		Un std::stringstream utilisant une locale C.
	 */
	template< typename CharT >
	inline std::basic_stringstream< CharT > makeStringStreamT()
	{
		static std::locale const loc{ "C" };
		std::basic_stringstream< CharT > result;
		result.imbue( loc );
		return result;
	}

	inline StringStream makeStringStream()
	{
		static std::locale const loc{ "C" };
		StringStream result;
		result.imbue( loc );
		return result;
	}
}

constexpr castor::Seconds operator "" _s( unsigned long long value )
{
	return castor::Seconds( int64_t( value ) );
}

constexpr castor::Milliseconds operator "" _ms( unsigned long long value )
{
	return castor::Milliseconds( int64_t( value ) );
}

constexpr castor::Microseconds operator "" _us( unsigned long long value )
{
	return castor::Microseconds( int64_t( value ) );
}

constexpr castor::Nanoseconds operator "" _ns( unsigned long long value )
{
	return castor::Nanoseconds( int64_t( value ) );
}

#include "Miscellaneous/Debug.hpp"

#endif
