/*
See LICENSE file in root folder
*/
#ifndef ___CU_PoolModule_HPP___
#define ___CU_PoolModule_HPP___
#pragma once

#include "CastorUtils/Align/AlignModule.hpp"
#include "CastorUtils/CastorUtils.hpp"

namespace castor
{
	/**@name Memory pool */
	//@{
	/**
	\~english
	\brief		Buddy allocator traits.
	\~french
	\brief		Traits d'un buddy allocator.
	*/
	struct BuddyAllocatorTraits;
	/**
	\~english
	\brief		Buddy allocator implementation.
	\~french
	\brief		Implémentation d'un buddy allocator.
	*/
	template< typename Traits >
	class BuddyAllocatorT;
	//@}
}

#endif
