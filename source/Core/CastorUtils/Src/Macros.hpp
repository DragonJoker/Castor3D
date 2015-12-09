﻿/*
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
#ifndef ___Castor_Macros___
#define ___Castor_Macros___

#include "config.hpp"
#include "PlatformConfig.hpp"
#include "CompilerConfig.hpp"

#if !defined( __FUNCTION__ )
//! Defines this macro if not in debug
#	define __FUNCTION__ ""
#endif

/**@name Unicode support */
//@{

#if CASTOR_USE_UNICODE
//!\~english The macro to use with constant strings, id est : cuT( "a string")\~french Macro à utiliser pour les chaînes constantes, id est : cuT( "une chaîne")
#	define cuT( x ) L##x
typedef wchar_t xchar;
typedef char ychar;
#	define xout wcout
#	define xerr wcerr
#	define xlog wclog
#else
//!\~english The macro to use with constant strings, id est : cuT( "a string")\~french Macro à utiliser pour les chaînes constantes, id est : cuT( "une chaîne")
#	define cuT( x ) x
typedef char xchar;
typedef wchar_t ychar;
#	define xout cout
#	define xerr cerr
#	define xlog clog
#endif

//@}

#define CASTOR_COUCOU Castor::Logger::LogDebug( cuT( "Coucou % 4d : %s @ line %d" ), __COUNTER__, __FUNCTION__, __LINE__ );

#if defined( _MSC_VER )
#	include <tchar.h>
#	define cvsnprintf _vsntprintf_s
#	define ccsncpy( x, y, z ) _tcsncpy_s( x, z, y, z )
#	if _MSC_VER < 1900
#		define sscanf sscanf_s
#	endif
#elif defined( __clang__)
#	if !defined( _WIN32 )
#		define _FILE_OFFSET_BITS 64
#		if !CASTOR_USE_UNICODE
#			define cvsnprintf( buf, sz, cnt, fmt, arg ) vsnprintf( buf, cnt, fmt, arg )
#			define ccsncpy( x, y, z ) strncpy_s( x, y, z )
#		else
#			define cvsnprintf( buf, sz, cnt, fmt, arg ) vswprintf( buf, cnt, fmt, arg )
#			define ccsncpy( x, y, z ) wcsncpy_s( x, y, z )
#		endif
#	else
#		define _FILE_OFFSET_BITS 64
#		if !CASTOR_USE_UNICODE
#			define cvsnprintf( buf, sz, cnt, fmt, arg ) vsnprintf( buf, cnt, fmt, arg )
#			define ccsncpy( x, y, z ) strncpy_s( x, y, z )
#		else
#			define cvsnprintf( buf, sz, cnt, fmt, arg ) vsnwprintf( buf, cnt, fmt, arg )
#			define ccsncpy( x, y, z ) wcsncpy_s( x, y, z )
#		endif
#	endif
#elif defined( __GNUG__)
#	if !defined( _WIN32 )
#		define _FILE_OFFSET_BITS 64
#		if !CASTOR_USE_UNICODE
#			define cvsnprintf( buf, sz, cnt, fmt, arg ) vsnprintf( buf, cnt, fmt, arg )
#			define ccsncpy( x, y, z ) strncpy_s( x, y, z )
#		else
#			define cvsnprintf( buf, sz, cnt, fmt, arg ) vswprintf( buf, cnt, fmt, arg )
#			define ccsncpy( x, y, z ) wcsncpy_s( x, y, z )
#		endif
#	else
#		define _FILE_OFFSET_BITS 64
#		if !CASTOR_USE_UNICODE
#			define cvsnprintf( buf, sz, cnt, fmt, arg ) vsnprintf( buf, cnt, fmt, arg )
#			define ccsncpy( x, y, z ) strncpy_s( x, y, z )
#		else
#			define cvsnprintf( buf, sz, cnt, fmt, arg ) vsnwprintf( buf, cnt, fmt, arg )
#			define ccsncpy( x, y, z ) wcsncpy_s( x, y, z )
#		endif
#	endif
#elif defined( __BORLANDC__ )
#	if !defined( _WIN32 )
#		define _FILE_OFFSET_BITS 64
#		if !CASTOR_USE_UNICODE
#			define cvsnprintf( buf, sz, cnt, fmt, arg ) vsnprintf( buf, cnt, fmt, arg )
#			define ccsncpy( x, y, z ) strncpy_s( x, y, z )
#		else
#			define cvsnprintf( buf, sz, cnt, fmt, arg ) vswprintf( buf, cnt, fmt, arg )
#			define ccsncpy( x, y, z ) wcsncpy_s( x, y, z )
#		endif
#	else
#		define _FILE_OFFSET_BITS 64
#		if !CASTOR_USE_UNICODE
#			define cvsnprintf( buf, sz, cnt, fmt, arg ) vsnprintf( buf, cnt, fmt, arg )
#			define ccsncpy( x, y, z ) strncpy_s( x, y, z )
#		else
#			define cvsnprintf( buf, sz, cnt, fmt, arg ) vsnwprintf( buf, cnt, fmt, arg )
#			define ccsncpy( x, y, z ) wcsncpy_s( x, y, z )
#		endif
#	endif
#else
#	error "Yet unsupported compiler"
#endif

#if CASTOR_HAS_DEFAULTED_FUNC
#	define CASTOR_DEFAULT = default
#else
#	define CASTOR_DEFAULT
#endif

#if CASTOR_HAS_DELETED_FUNC
#	define CASTOR_DELETED = delete
#else
#	define CASTOR_DELETED
#endif

#define DECLARE_SMART_PTR( class_name )\
	typedef std::shared_ptr< class_name > class_name##SPtr;\
	typedef std::weak_ptr< class_name > class_name##WPtr;\
	typedef std::unique_ptr< class_name > class_name##UPtr;\
	typedef class_name * class_name##RPtr

#define DECLARE_MAP( key, value, name )\
	typedef std::map< key, value > name##Map;\
	typedef name##Map::iterator name##MapIt;\
	typedef name##Map::reverse_iterator name##MapRIt;\
	typedef name##Map::const_iterator name##MapConstIt;\
	typedef name##Map::const_reverse_iterator name##MapConstRIt;\
	typedef name##Map::value_type name##Pair

#define DECLARE_MULTIMAP( key, value, name )\
	typedef std::multimap< key, value > name##MMap;\
	typedef name##MMap::iterator name##MMapIt;\
	typedef name##MMap::reverse_iterator name##MMapRIt;\
	typedef name##MMap::const_iterator name##MMapConstIt;\
	typedef name##MMap::const_reverse_iterator name##MMapConstRIt;\
	typedef name##MMap::value_type name##Pair

#define DECLARE_SET( key, name )\
	typedef std::set< key > name##Set;\
	typedef name##Set::iterator name##SetIt;\
	typedef name##Set::reverse_iterator name##SetRIt;\
	typedef name##Set::const_iterator name##SetConstIt;\
	typedef name##Set::const_reverse_iterator name##SetConstRIt

#define DECLARE_VECTOR( key, name )\
	typedef std::vector< key > name##Array;\
	typedef name##Array::iterator name##ArrayIt;\
	typedef name##Array::reverse_iterator name##ArrayRIt;\
	typedef name##Array::const_iterator name##ArrayConstIt;\
	typedef name##Array::const_reverse_iterator name##ArrayConstRIt

#define DECLARE_ARRAY( key, count, name )\
	typedef std::array< key, count > name##Array;\
	typedef name##Array::iterator name##ArrayIt;\
	typedef name##Array::reverse_iterator name##ArrayRIt;\
	typedef name##Array::const_iterator name##ArrayConstIt;\
	typedef name##Array::const_reverse_iterator name##ArrayConstRIt

#define DECLARE_LIST( key, name )\
	typedef std::list< key > name##List;\
	typedef name##List::iterator name##ListIt;\
	typedef name##List::reverse_iterator name##ListRIt;\
	typedef name##List::const_iterator name##ListConstIt;\
	typedef name##List::const_reverse_iterator name##ListConstRIt

#define DECLARE_TPL_MAP( key, value, name )\
	typedef std::map< key, value > name##Map;\
	typedef typename name##Map::iterator name##MapIt;\
	typedef typename name##Map::reverse_iterator name##MapRIt;\
	typedef typename name##Map::const_iterator name##MapConstIt;\
	typedef typename name##Map::const_reverse_iterator name##MapConstRIt;\
	typedef typename name##Map::value_type name##Pair

#define DECLARE_TPL_SET( key, name )\
	typedef std::set< key > name##Set;\
	typedef typename name##Set::iterator name##SetIt;\
	typedef typename name##Set::reverse_iterator name##SetRIt;\
	typedef typename name##Set::const_iterator name##SetConstIt;\
	typedef typename name##Set::const_reverse_iterator name##SetConstRIt

#define DECLARE_TPL_VECTOR( key, name )\
	typedef std::vector< key > name##Array;\
	typedef typename name##Array::iterator name##ArrayIt;\
	typedef typename name##Array::reverse_iterator name##ArrayRIt;\
	typedef typename name##Array::const_iterator name##ArrayConstIt;\
	typedef typename name##Array::const_reverse_iterator name##ArrayConstRIt

#define DECLARE_TPL_ARRAY( key, count, name )\
	typedef std::array< key, count > name##Array;\
	typedef typename name##Array::iterator name##ArrayIt;\
	typedef typename name##Array::reverse_iterator name##ArrayRIt;\
	typedef typename name##Array::const_iterator name##ArrayConstIt;\
	typedef typename name##Array::const_reverse_iterator name##ArrayConstRIt

#define DECLARE_TPL_LIST( key, name )\
	typedef std::list< key > name##List;\
	typedef typename name##List::iterator name##ListIt;\
	typedef typename name##List::reverse_iterator name##ListRIt;\
	typedef typename name##List::const_iterator name##ListConstIt;\
	typedef typename name##List::const_reverse_iterator name##ListConstRIt

#define DECLARE_COLLECTION( elem, key, name )\
	typedef Castor::Collection< elem, key > name##Collection;\
	typedef name##Collection::TObjPtrMapIt name##CollectionIt;\
	typedef name##Collection::TObjPtrMapConstIt name##CollectionConstIt;

#define DECLARE_POINT( type, count, name )\
	typedef Point< type, count > Point##count##name;\
	DECLARE_SMART_PTR( Point##count##name );\
	DECLARE_VECTOR( Point##count##name, Point##count##name );\
	DECLARE_LIST( Point##count##name, Point##count##name );\
	DECLARE_VECTOR( Point##count##name##SPtr, Point##count##name##Ptr );\
	DECLARE_LIST( Point##count##name##SPtr, Point##count##name##Ptr )

#define DECLARE_COORD( type, count, name )\
	typedef Coords< type, count > Coords##count##name;\
	DECLARE_SMART_PTR( Coords##count##name );\
	DECLARE_VECTOR( Coords##count##name, Coords##count##name );\
	DECLARE_LIST( Coords##count##name, Coords##count##name );\
	DECLARE_VECTOR( Coords##count##name##SPtr, Coords##count##name##Ptr );\
	DECLARE_LIST( Coords##count##name##SPtr, Coords##count##name##Ptr )

#define DECLARE_MTX( type, rows, cols, name )\
	typedef Matrix< type, rows, cols > Matrix##rows##x##cols##name;\
	DECLARE_SMART_PTR( Matrix##rows##x##cols##name );\
	DECLARE_VECTOR( Matrix##rows##x##cols##name, Matrix##rows##x##cols##name );\
	DECLARE_LIST( Matrix##rows##x##cols##name, Matrix##rows##x##cols##name );\
	DECLARE_VECTOR( Matrix##rows##x##cols##name##SPtr, Matrix##rows##x##cols##name##Ptr );\
	DECLARE_LIST( Matrix##rows##x##cols##name##SPtr, Matrix##rows##x##cols##name##Ptr )

#define DECLARE_SQMTX( type, count, name )\
	typedef SquareMatrix< type, count > Matrix##count##x##count##name;\
	DECLARE_SMART_PTR( Matrix##count##x##count##name );\
	DECLARE_VECTOR( Matrix##count##x##count##name, Matrix##count##x##count##name );\
	DECLARE_LIST( Matrix##count##x##count##name, Matrix##count##x##count##name );\
	DECLARE_VECTOR( Matrix##count##x##count##name##SPtr, Matrix##count##x##count##name##Ptr );\
	DECLARE_LIST( Matrix##count##x##count##name##SPtr, Matrix##count##x##count##name##Ptr )

#if !defined CU_PARAM_UNUSED
#	define CU_PARAM_UNUSED( x )
#endif

#endif
