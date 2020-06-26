/*
See LICENSE file in root folder
*/
#ifndef ___Castor_Macros___
#define ___Castor_Macros___

#include "CastorUtils/config.hpp"
#include "CastorUtils/Config/PlatformConfig.hpp"
#include "CastorUtils/Config/CompilerConfig.hpp"

#if !defined( __FUNCTION__ )
//! Defines this macro if not in debug
#	define __FUNCTION__ ""
#endif

#define CU_Coucou std::clog << "Coucou " << __COUNTER__ << " : " << __FUNCTION__ << " @ line " << __LINE__ << std::endl;

#if defined( CU_CompilerMSVC )
#	include <tchar.h>
#	define cvsnprintf _vsntprintf_s
#	if CU_CompilerVersion < 1900
#		define sscanf sscanf_s
#	endif
#elif defined( CU_CompilerCLANG )
#	if !defined( CU_PlatformWindows )
#		define _FILE_OFFSET_BITS 64
#		define cvsnprintf( buf, sz, cnt, fmt, arg ) vsnprintf( buf, cnt, fmt, arg )
#	else
#		define _FILE_OFFSET_BITS 64
#		define cvsnprintf( buf, sz, cnt, fmt, arg ) vsnprintf( buf, cnt, fmt, arg )
#	endif
#elif defined( CU_CompilerGNUC )
#	if !defined( CU_PlatformWindows )
#		define _FILE_OFFSET_BITS 64
#		define cvsnprintf( buf, sz, cnt, fmt, arg ) vsnprintf( buf, cnt, fmt, arg )
#	else
#		define _FILE_OFFSET_BITS 64
#		define cvsnprintf( buf, sz, cnt, fmt, arg ) vsnprintf( buf, cnt, fmt, arg )
#	endif
#elif defined( CU_CompilerBORLAND )
#	if !defined( CU_PlatformWindows )
#		define _FILE_OFFSET_BITS 64
#		define cvsnprintf( buf, sz, cnt, fmt, arg ) vsnprintf( buf, cnt, fmt, arg )
#	else
#		define _FILE_OFFSET_BITS 64
#		define cvsnprintf( buf, sz, cnt, fmt, arg ) vsnprintf( buf, cnt, fmt, arg )
#	endif
#endif

#include <memory>

#define CU_DeclareSmartPtr( class_name )\
	using class_name##SPtr = std::shared_ptr< class_name >;\
	using class_name##WPtr = std::weak_ptr< class_name >;\
	using class_name##UPtr = std::unique_ptr< class_name >;\
	using class_name##RPtr = class_name *

#define CU_DeclareTemplateSmartPtr( class_name )\
	template< typename T > using class_name##SPtr = std::shared_ptr< class_name< T > >;\
	template< typename T > using class_name##WPtr = std::weak_ptr< class_name< T > >;\
	template< typename T > using class_name##UPtr = std::unique_ptr< class_name< T > >;\
	template< typename T > using class_name##RPtr = class_name< T > *

#include <map>

#define CU_DeclareMap( key, value, name )\
	using name##Map = std::map< key, value >;\
	using name##MapIt = name##Map::iterator;\
	using name##MapRIt = name##Map::reverse_iterator;\
	using name##MapConstIt = name##Map::const_iterator;\
	using name##MapConstRIt = name##Map::const_reverse_iterator;\
	using name##MapValueType = name##Map::value_type

#define CU_DeclareMultimap( key, value, name )\
	using name##MMap = std::multimap< key, value >;\
	using name##MMapIt = name##MMap::iterator;\
	using name##MMapRIt = name##MMap::reverse_iterator;\
	using name##MMapConstIt = name##MMap::const_iterator;\
	using name##MMapConstRIt = name##MMap::const_reverse_iterator;\
	using name##MapValueType = name##MMap::value_type

#include <set>

#define CU_DeclareSet( key, name )\
	using name##Set = std::set< key >;\
	using name##SetIt = name##Set::iterator;\
	using name##SetRIt = name##Set::reverse_iterator;\
	using name##SetConstIt = name##Set::const_iterator;\
	using name##SetConstRIt = name##Set::const_reverse_iterator

#define CU_DeclareMultiset( key, name )\
	using name##Mset = std::multiset< key >;\
	using name##MSetIt = name##MSet::iterator;\
	using name##MSetRIt = name##MSet::reverse_iterator;\
	using name##MSetConstIt = name##MSet::const_iterator;\
	using name##MSetConstRIt = name##MSet::const_reverse_iterator

#include <vector>

#define CU_DeclareVector( key, name )\
	using name##Array = std::vector< key >;\
	using name##ArrayIt = name##Array::iterator;\
	using name##ArrayRIt = name##Array::reverse_iterator;\
	using name##ArrayConstIt = name##Array::const_iterator;\
	using name##ArrayConstRIt = name##Array::const_reverse_iterator

#include <array>

#define CU_DeclareArray( key, count, name )\
	using name##Array = std::array< key, size_t( count ) >;\
	using name##ArrayIt = name##Array::iterator;\
	using name##ArrayRIt = name##Array::reverse_iterator;\
	using name##ArrayConstIt = name##Array::const_iterator;\
	using name##ArrayConstRIt = name##Array::const_reverse_iterator

#include <list>

#define CU_DeclareList( key, name )\
	using name##List = std::list< key >;\
	using name##ListIt = name##List::iterator;\
	using name##ListRIt = name##List::reverse_iterator;\
	using name##ListConstIt = name##List::const_iterator;\
	using name##ListConstRIt = name##List::const_reverse_iterator

#define CU_DeclareTemplateMap( key, value, name )\
	using name##Map = std::map< key, value >;\
	using name##MapIt = typename name##Map::iterator;\
	using name##MapRIt = typename name##Map::reverse_iterator;\
	using name##MapConstIt = typename name##Map::const_iterator;\
	using name##MapConstRIt = typename name##Map::const_reverse_iterator;\
	using name##Pair = typename name##Map::value_type

#define CU_DeclareTemplateSet( key, name )\
	using name##Set = std::set< key >;\
	using name##SetIt = typename name##Set::iterator;\
	using name##SetRIt = typename name##Set::reverse_iterator;\
	using name##SetConstIt = typename name##Set::const_iterator;\
	using name##SetConstRIt = typename name##Set::const_reverse_iterator

#define CU_DeclareTemplateVector( key, name )\
	using name##Array = std::vector< key >;\
	using name##ArrayIt = typename name##Array::iterator;\
	using name##ArrayRIt = typename name##Array::reverse_iterator;\
	using name##ArrayConstIt = typename name##Array::const_iterator;\
	using name##ArrayConstRIt = typename name##Array::const_reverse_iterator

#define CU_DeclareTemplateArray( key, count, name )\
	using name##Array = std::array< key, count >;\
	using name##ArrayIt = typename name##Array::iterator;\
	using name##ArrayRIt = typename name##Array::reverse_iterator;\
	using name##ArrayConstIt = typename name##Array::const_iterator;\
	using name##ArrayConstRIt = typename name##Array::const_reverse_iterator

#define CU_DeclareTemplateList( key, name )\
	using name##List = std::list< key >;\
	using name##ListIt = typename name##List::iterator;\
	using name##ListRIt = typename name##List::reverse_iterator;\
	using name##ListConstIt = typename name##List::const_iterator;\
	using name##ListConstRIt = typename name##List::const_reverse_iterator

#define CU_DeclareCollection( elem, key, name )\
	using name##Collection = castor::Collection< elem, key >;\
	using name##CollectionIt = name##Collection::TObjPtrMapIt;\
	using name##CollectionConstIt = name##Collection::TObjPtrMapConstIt

#define CU_DeclarePoint( type, count, name )\
	using Point##count##name = Point< type, count >;\
	CU_DeclareSmartPtr( Point##count##name );\
	CU_DeclareVector( Point##count##name, Point##count##name );\
	CU_DeclareList( Point##count##name, Point##count##name );\
	CU_DeclareVector( Point##count##name##SPtr, Point##count##name##Ptr );\
	CU_DeclareList( Point##count##name##SPtr, Point##count##name##Ptr )

#define CU_DeclareCoord( type, count, name )\
	using Coords##count##name = Coords< type, count >;\
	CU_DeclareSmartPtr( Coords##count##name );\
	CU_DeclareVector( Coords##count##name, Coords##count##name );\
	CU_DeclareList( Coords##count##name, Coords##count##name );\
	CU_DeclareVector( Coords##count##name##SPtr, Coords##count##name##Ptr );\
	CU_DeclareList( Coords##count##name##SPtr, Coords##count##name##Ptr )

#define CU_DeclareConstCoord( type, count, name )\
	using ConstCoords##count##name = Coords< type const, count >;\
	CU_DeclareSmartPtr( ConstCoords##count##name );\
	CU_DeclareVector( ConstCoords##count##name, ConstCoords##count##name );\
	CU_DeclareList( ConstCoords##count##name, ConstCoords##count##name );\
	CU_DeclareVector( ConstCoords##count##name##SPtr, ConstCoords##count##name##Ptr );\
	CU_DeclareList( ConstCoords##count##name##SPtr, ConstCoords##count##name##Ptr )

#define CU_DeclareMtx( type, rows, cols, name )\
	using Matrix##rows##x##cols##name = Matrix< type, rows, cols >;\
	CU_DeclareSmartPtr( Matrix##rows##x##cols##name );\
	CU_DeclareVector( Matrix##rows##x##cols##name, Matrix##rows##x##cols##name );\
	CU_DeclareList( Matrix##rows##x##cols##name, Matrix##rows##x##cols##name );\
	CU_DeclareVector( Matrix##rows##x##cols##name##SPtr, Matrix##rows##x##cols##name##Ptr );\
	CU_DeclareList( Matrix##rows##x##cols##name##SPtr, Matrix##rows##x##cols##name##Ptr )

#define CU_DeclareSqMtx( type, count, name )\
	using Matrix##count##x##count##name = SquareMatrix< type, count >;\
	CU_DeclareSmartPtr( Matrix##count##x##count##name );\
	CU_DeclareVector( Matrix##count##x##count##name, Matrix##count##x##count##name );\
	CU_DeclareList( Matrix##count##x##count##name, Matrix##count##x##count##name );\
	CU_DeclareVector( Matrix##count##x##count##name##SPtr, Matrix##count##x##count##name##Ptr );\
	CU_DeclareList( Matrix##count##x##count##name##SPtr, Matrix##count##x##count##name##Ptr )

#if !defined CU_UnusedParam
#	define CU_UnusedParam( x )
#endif

#define CU_EnumBounds( EnumName, EnumMin )\
	EnumName##_COUNT,\
	EnumName##_MIN = EnumMin,\
	EnumName##_MAX = EnumName##_COUNT - 1

#define CU_ScopedEnumBounds( EnumMin )\
	eCount,\
	eMin = EnumMin,\
	eMax = eCount - 1

#define CU_ImplementFlags( FlagType )\
	using FlagType##s = castor::FlagCombination< FlagType >;\
	inline constexpr FlagType##s operator|( FlagType lhs, FlagType rhs )\
	{\
		return FlagType##s( lhs ) | rhs;\
	}\
	inline constexpr FlagType##s operator|( FlagType##s::BaseType lhs, FlagType rhs )\
	{\
		return FlagType##s( lhs ) | rhs;\
	}\
	inline constexpr FlagType##s operator|( FlagType lhs, FlagType##s::BaseType rhs )\
	{\
		return FlagType##s( lhs ) | rhs;\
	}

#define CU_ImplementClassFlags( Class, FlagType )\
	using Class##FlagType##s = castor::FlagCombination< Class::FlagType >;\
	inline constexpr Class##FlagType##s operator|( Class::FlagType lhs, Class::FlagType rhs )\
	{\
		return Class##FlagType##s( lhs ) | rhs;\
	}\
	inline constexpr Class##FlagType##s operator|( Class##FlagType##s::BaseType lhs, Class::FlagType rhs )\
	{\
		return Class##FlagType##s( lhs ) | rhs;\
	}\
	inline constexpr Class##FlagType##s operator|( Class::FlagType lhs, Class##FlagType##s::BaseType rhs )\
	{\
		return Class##FlagType##s( lhs ) | rhs;\
	}

#endif
