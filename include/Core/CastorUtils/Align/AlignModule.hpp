/*
See LICENSE file in root folder
*/
#ifndef ___CU_AlignModule_HPP___
#define ___CU_AlignModule_HPP___
#pragma once

#include "CastorUtils/CastorUtils.hpp"

#if defined( CU_CompilerMSVC )
#	define CU_Alignas( a ) alignas( a )
#else
#	define CU_Alignas( a )
#endif

namespace castor
{
	/**@name Align */
	//@{
	/**
	\~english
	\brief		Templated class that provide alignment support.
	\~french
	\brief		Classe template qui fournit le support de l'alignement.
	*/
	template< int A >
	class CU_Alignas( A ) Aligned;
	/**
	\~english
	\brief		Templated class that provide alignment support.
	\~french
	\brief		Classe template qui fournit le support de l'alignement.
	*/
	template< typename T >
	class CU_Alignas( alignof( T ) ) AlignedFrom;
	/**
	\~english
	\brief		Memory allocator.
	\remarks	Allocates aligned memory.
	\~french
	\brief		Allocateur de mémoire.
	\remarks	Alloue de la mémoire alignée.
	*/
	template< size_t Align >
	class AlignedMemoryAllocator;
	/**
	\~english
	\brief		Memory allocator.
	\remarks	Allocates non aligned memory.
	\~french
	\brief		Allocateur de mémoire.
	\remarks	Alloue de la mémoire non alignée.
	*/
	class NonAlignedMemoryAllocator;
}

#endif
