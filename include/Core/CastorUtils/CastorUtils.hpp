/*
See LICENSE file in root folder
*/
#ifndef ___CU_CastorUtils_H___
#define ___CU_CastorUtils_H___

#include "CastorUtils/Config/ConfigModule.hpp"

#include <CastorUtils/Config/BeginExternHeaderGuard.hpp>
#include <array>
#include <cassert>
#include <chrono>
#include <cstdarg>
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
#include <CastorUtils/Config/EndExternHeaderGuard.hpp>

namespace castor
{
	struct StringHash
	{
		using is_transparent = void;

		std::size_t operator()( std::string_view v )const
		{
			return std::hash< std::string_view >{}( v );
		}

		std::size_t operator()( std::wstring_view v )const
		{
			return std::hash< std::wstring_view >{}( v );
		}
	};

	/**
	\see		QuaternionT
	\~english
	\brief		Structure used to construct objects but without initialisation.
	\~french
	\brief		Structure utilisée pour les constructeurs d'objets à ne pas initialiser.
	*/
	struct NoInit
	{
	};
	static NoInit constexpr noInit;

	using s8 = int8_t;
	using u8 = uint8_t;
	using byte = uint8_t;
	using s16 = int16_t;
	using u16 = uint16_t;
	using s32 = int32_t;
	using u32 = uint32_t;
	using s64 = int64_t;
	using u64 = uint64_t;
	using usize = size_t;
	using f32 = float;
	using f64 = double;

	CU_DeclareVector( byte, Byte );
	CU_DeclareVector( s32, Int32 );
	CU_DeclareVector( u32, UInt32 );
}

#include "CastorUtils/Miscellaneous/Debug.hpp"

#endif
