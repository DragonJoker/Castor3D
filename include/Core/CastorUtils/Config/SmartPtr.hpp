/*
See LICENSE file in root folder
*/
#ifndef ___CU_SmartPtr_HPP___
#define ___CU_SmartPtr_HPP___

#include "CastorUtils/Config/BeginExternHeaderGuard.hpp"
#include "CastorUtils/Config/PlatformConfig.hpp"

#include <memory>
#include "CastorUtils/Config/EndExternHeaderGuard.hpp"

namespace castor
{
	template< typename TypeT >
	struct Deleter
	{
		void operator()( TypeT * pointer )noexcept;
	};

	template< typename TypeT >
	using UniquePtr = std::unique_ptr< TypeT, Deleter< TypeT > >;

	template< typename TypeT, typename TypeU, typename ... ParamsT >
	UniquePtr< TypeT > makeUniqueDerived( ParamsT && ... params )
	{
		return UniquePtr< TypeT >( new TypeU( std::forward< ParamsT >( params )... ) );
	}

	template< typename TypeT, typename ... ParamsT >
	UniquePtr< TypeT > makeUnique( ParamsT && ... params )
	{
		return UniquePtr< TypeT >( new TypeT( std::forward< ParamsT >( params )... ) );
	}

	template< typename TypeU, typename TypeT >
	UniquePtr< TypeU > ptrCast( UniquePtr< TypeT > ptr )
	{
		return UniquePtr< TypeU >( &static_cast< TypeU & >( *ptr.release() ) );
	}

	template< typename TypeU, typename TypeT >
	UniquePtr< TypeU > ptrRefCast( UniquePtr< TypeT > & ptr )
	{
		return UniquePtr< TypeU >( &static_cast< TypeU & >( *ptr.release() ) );
	}
}

#define CU_DeclareSmartPtr( nmspc, class_name, expdecl )\
}\
namespace castor\
{\
	template<>\
	struct Deleter< nmspc::class_name >\
	{\
		expdecl void operator()( nmspc::class_name * pointer )noexcept;\
	};\
}\
namespace nmspc\
{\
	using class_name##UPtr = castor::UniquePtr< class_name >;\
	using class_name##RPtr = class_name *

#define CU_ImplementSmartPtr( nmspc, class_name )\
namespace castor\
{\
	void Deleter< nmspc::class_name >::operator()( nmspc::class_name * pointer )noexcept\
	{\
		delete pointer;\
	}\
}

#define CU_DeclareTemplateSmartPtr( nmspc, class_name )\
}\
namespace castor\
{\
	template< typename InstT >\
	struct Deleter< nmspc::class_name##T< InstT > >\
	{\
		void operator()( nmspc::class_name##T< InstT > * pointer )noexcept\
		{\
			delete pointer;\
		}\
	};\
}\
namespace nmspc\
{\
	template< typename InstT >\
	using class_name##UPtrT = castor::UniquePtr< class_name##T< InstT > >;\
	template< typename InstT >\
	using class_name##RPtrT = class_name##T< InstT > *

#define CU_DeclareEnumTemplateSmartPtr( nmspc, class_name, enum_name )\
}\
namespace castor\
{\
	template< enum_name EnumT >\
	struct Deleter< nmspc::class_name##T< EnumT > >\
	{\
		void operator()( nmspc::class_name##T< EnumT > * pointer )noexcept\
		{\
			delete pointer;\
		}\
	};\
}\
namespace nmspc\
{\
	template< enum_name EnumT >\
	using class_name##UPtrT = castor::UniquePtr< class_name##T< EnumT > >;\
	template< enum_name EnumT >\
	using class_name##RPtrT = class_name##T< EnumT > *

#endif
