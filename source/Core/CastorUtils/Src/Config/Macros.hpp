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

//!\~english The macro to use with constant strings, id est : cuT( "a string")	\~french Macro à utiliser pour les chaînes constantes, id est : cuT( "une chaîne")
#define cuT( x ) x
typedef char xchar;
typedef wchar_t ychar;

//@}

#define CASTOR_COUCOU Castor::Logger::LogDebug( cuT( "Coucou % 4d : %s @ line %d" ), __COUNTER__, __FUNCTION__, __LINE__ );

#if defined( _MSC_VER )
#	include <tchar.h>
#	define cvsnprintf _vsntprintf_s
#	if _MSC_VER < 1900
#		define sscanf sscanf_s
#	endif
#elif defined( __clang__)
#	if !defined( _WIN32 )
#		define _FILE_OFFSET_BITS 64
#		define cvsnprintf( buf, sz, cnt, fmt, arg ) vsnprintf( buf, cnt, fmt, arg )
#	else
#		define _FILE_OFFSET_BITS 64
#		define cvsnprintf( buf, sz, cnt, fmt, arg ) vsnprintf( buf, cnt, fmt, arg )
#	endif
#elif defined( __GNUG__)
#	if !defined( _WIN32 )
#		define _FILE_OFFSET_BITS 64
#		define cvsnprintf( buf, sz, cnt, fmt, arg ) vsnprintf( buf, cnt, fmt, arg )
#	else
#		define _FILE_OFFSET_BITS 64
#		define cvsnprintf( buf, sz, cnt, fmt, arg ) vsnprintf( buf, cnt, fmt, arg )
#	endif
#elif defined( __BORLANDC__ )
#	if !defined( _WIN32 )
#		define _FILE_OFFSET_BITS 64
#		define cvsnprintf( buf, sz, cnt, fmt, arg ) vsnprintf( buf, cnt, fmt, arg )
#	else
#		define _FILE_OFFSET_BITS 64
#		define cvsnprintf( buf, sz, cnt, fmt, arg ) vsnprintf( buf, cnt, fmt, arg )
#	endif
#else
#	error "Yet unsupported compiler"
#endif

#define DECLARE_SMART_PTR( class_name )\
	using class_name##SPtr = std::shared_ptr< class_name >;\
	using class_name##WPtr = std::weak_ptr< class_name >;\
	using class_name##UPtr = std::unique_ptr< class_name >;\
	using class_name##RPtr = class_name *

#define DECLARE_MAP( key, value, name )\
	using name##Map = std::map< key, value >;\
	using name##MapIt = name##Map::iterator;\
	using name##MapRIt = name##Map::reverse_iterator;\
	using name##MapConstIt = name##Map::const_iterator;\
	using name##MapConstRIt = name##Map::const_reverse_iterator;\
	using name##MapValueType = name##Map::value_type

#define DECLARE_MULTIMAP( key, value, name )\
	using name##MMap = std::multimap< key, value >;\
	using name##MMapIt = name##MMap::iterator;\
	using name##MMapRIt = name##MMap::reverse_iterator;\
	using name##MMapConstIt = name##MMap::const_iterator;\
	using name##MMapConstRIt = name##MMap::const_reverse_iterator;\
	using name##MapValueType = name##MMap::value_type

#define DECLARE_SET( key, name )\
	using name##Set = std::set< key >;\
	using name##SetIt = name##Set::iterator;\
	using name##SetRIt = name##Set::reverse_iterator;\
	using name##SetConstIt = name##Set::const_iterator;\
	using name##SetConstRIt = name##Set::const_reverse_iterator

#define DECLARE_MULTISET( key, name )\
	using name##MSet = std::multiset< key >;\
	using name##MSetIt = name##MSet::iterator;\
	using name##MSetRIt = name##MSet::reverse_iterator;\
	using name##MSetConstIt = name##MSet::const_iterator;\
	using name##MSetConstRIt = name##MSet::const_reverse_iterator

#define DECLARE_VECTOR( key, name )\
	using name##Array = std::vector< key >;\
	using name##ArrayIt = name##Array::iterator;\
	using name##ArrayRIt = name##Array::reverse_iterator;\
	using name##ArrayConstIt = name##Array::const_iterator;\
	using name##ArrayConstRIt = name##Array::const_reverse_iterator

#define DECLARE_ARRAY( key, count, name )\
	using name##Array = std::array< key, count >;\
	using name##ArrayIt = name##Array::iterator;\
	using name##ArrayRIt = name##Array::reverse_iterator;\
	using name##ArrayConstIt = name##Array::const_iterator;\
	using name##ArrayConstRIt = name##Array::const_reverse_iterator

#define DECLARE_LIST( key, name )\
	using name##List = std::list< key >;\
	using name##ListIt = name##List::iterator;\
	using name##ListRIt = name##List::reverse_iterator;\
	using name##ListConstIt = name##List::const_iterator;\
	using name##ListConstRIt = name##List::const_reverse_iterator

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
	using name##List = std::list< key >;\
	using typename name##List::iterator name##ListIt;\
	using typename name##List::reverse_iterator name##ListRIt;\
	using typename name##List::const_iterator name##ListConstIt;\
	using typename name##List::const_reverse_iterator name##ListConstRIt

#define DECLARE_COLLECTION( elem, key, name )\
	using name##Collection = Castor::Collection< elem, key >;\
	using name##CollectionIt = name##Collection::TObjPtrMapIt;\
	using name##CollectionConstIt = name##Collection::TObjPtrMapConstIt

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

#define CASTOR_ENUM_BOUNDS( EnumName, EnumMin )\
	EnumName##_COUNT,\
	EnumName##_MIN = EnumMin,\
	EnumName##_MAX = EnumName##_COUNT - 1

#define CASTOR_ENUM_CLASS_BOUNDS( EnumMin )\
	Count,\
	Min = EnumMin,\
	Max = Count - 1

#endif
