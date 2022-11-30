/*
See LICENSE file in root folder
*/
#ifndef ___CU_CastorUtils_H___
#define ___CU_CastorUtils_H___

#include "CastorUtils/Config/ConfigModule.hpp"

#pragma warning( push )
#pragma warning( disable:4365 )
#pragma warning( disable:5262 )
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
#pragma warning( pop )

namespace castor
{
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

	CU_DeclareVector( uint8_t, Byte );
	CU_DeclareVector( int32_t, Int32 );
	CU_DeclareVector( uint32_t, UInt32 );
}

#pragma warning( push )
#pragma warning( disable:5262 )
#include "CastorUtils/Miscellaneous/Debug.hpp"
#pragma warning( pop )

#endif
