/*
See LICENSE file in root folder
*/
#ifndef ___CU_PoolModule_HPP___
#define ___CU_PoolModule_HPP___
#pragma once

#include "CastorUtils/CastorUtils.hpp"

namespace castor
{
	/**@name Memory pool */
	//@{
	/**
	\~english
	\brief		Supported MemoryData types.
	\~french
	\brief		Types de MemoryData supportés.
	*/
	enum class MemoryDataType
	{
		eFixed,
		eMarked,
		eFixedGrowing,
		eFixedGrowingMarked,
		CU_ScopedEnumBounds( eFixed )
	};
	/**
	\~english
	\brief		Pool errors enumeration.
	\~french
	\brief		Enumération des erreurs de pool.
	*/
	enum class PoolErrorType
	{
		eCommonOutOfMemory,
		eCommonPoolIsFull,
		eCommonMemoryLeaksDetected,
		eCommonNotFromRange,
		eMarkedLeakAddress,
		eMarkedDoubleDelete,
		eMarkedNotFromPool,
		eGrowingNotFromRanges,
		eSTLAllocatorUnique,
		CU_ScopedEnumBounds( eCommonOutOfMemory )
	};
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
	/**
	\~english
	\brief		Memory allocation policy. It can grow of a fixed objects count.
	\remarks	Allocates an additional byte, marks it, to be an overview of memory leaks.
				Holds the memory buffers, free chunks and currently allocated objects count.
	\~french
	\brief		Politique d'allocation de mémoire, pouvant grandir, d'un nombre fixe d'objets.
	\remarks	Alloue un octet de plus par objet, et marque cet octet, afin d'avoir un aperçu plus fin des fuites mémoires.
				Contient le tampon mémoire, les chunks libres, et le nombre d'objets actuellement alloués.
	*/
	template< typename Object >
	class FixedGrowingSizeMarkedMemoryData;
	/**
	\~english
	\brief		Memory allocation policy. It can grow of a fixed objects count.
	\remarks	Variable size, basic memory leaks check.
				Holds the memory buffers, free chunks and currently allocated objects count.
	\~french
	\brief		Politique d'allocation de mémoire, pouvant grandir, d'un nombre fixe d'objets.
	\remarks	Taille variable, check basique des fuites mémoire.
				Contient le tampon mémoire, les chunks libres, et le nombre d'objets actuellement alloués.
	*/
	template< typename Object, typename MemoryAllocator >
	class FixedGrowingSizeMemoryData;
	/**
	\~english
	\brief		Memory allocation policy.
	\remarks	Allocates an additional byte, marks it, to be an overview of memory leaks.
				Holds the memory buffers, free chunks and currently allocated objects count.
	\~french
	\brief		Politique d'allocation de mémoire.
	\remarks	Alloue un octet de plus par objet, et marque cet octet, afin d'avoir un aperçu plus fin des fuites mémoires.
				Contient le tampon mémoire, les chunks libres, et le nombre d'objets actuellement alloués.
	*/
	template< typename Object >
	class FixedSizeMarkedMemoryData;
	/**
	\~english
	\brief		Memory allocation policy.
	\remarks	Fixed size, basic memory leaks check.
				Holds the memory buffers, free chunks and currently allocated objects count.
	\~french
	\brief		Politique d'allocation de mémoire.
	\remarks	Taille fixe, check basique des fuites mémoire.
				Contient le tampon mémoire, les chunks libres, et le nombre d'objets actuellement alloués.
	*/
	template< typename Object, typename MemoryAllocator >
	class FixedSizeMemoryData;
	/**
	\~english
	\brief		A pool managed object.
	\~french
	\brief		Un objet géré par un pool.
	*/
	template< typename Object, MemoryDataType MemDataType >
	class PoolManagedObject;
	/**
	\~english
	\brief		The object pool typer.
	\~french
	\brief		Le typeur de pools d'objets.
	*/
	template< typename Object, MemoryDataType Type >
	struct MemoryDataTyper;
	/**
	\~english
	\brief		The aligned object pool typer.
	\~french
	\brief		Le typeur de pools d'objets alignés.
	*/
	template< typename Object, uint32_t Align, MemoryDataType Type >
	struct AlignedMemoryDataTyper;
	/**
	\~english
	\brief		The MemoryData namer.
	\~french
	\brief		Le nommeur de MemoryData.
	*/
	template< MemoryDataType Type >
	struct MemoryDataNamer;
	/**
	\~english
	\brief		Policy used to construct/destruct object if needed.
	\~french
	\brief		Poliituqe utilisée pour construire/détruire un objet si nécessaire.
	*/
	template< typename Object >
	struct NewDeletePolicy;
	/**
	\~english
	\brief		The objects pool.
	\remarks	Uses a policy to change behaviour easily.
	\param		Object		The pool objects type.
	\param		MemDataType	The allocation/deallocation policy type.
	\~french
	\brief		Le pool d'objets.
	\remarks	Utilisation d'une politique pour permettre de changer le comportement assez facilement.
	\param		Object		Le type des objets du pool.
	\param		MemDataType	Le type de la politique d'allocation/désallocation.
	*/
	template< typename Object, MemoryDataType MemDataType >
	class ObjectPool;
	/**
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		08/01/2016
	\~english
	\brief		The aligned objects pool.
	\remarks	Uses a policy to change behaviour easily.
	\param		Object		The pool objects type.
	\param		MemDataType	The allocation/deallocation policy type.
	\~french
	\brief		Le pool d'objets alignés.
	\remarks	Utilisation d'une politique pour permettre de changer le comportement assez facilement.
	\param		Object		Le type des objets du pool.
	\param		MemDataType	Le type de la politique d'allocation/désallocation.
	*/
	template< typename Object, MemoryDataType MemDataType, uint32_t Align >
	class AlignedObjectPool;
	/**
	\~english
	\brief		Error text and report function.
	\~french
	\brief		Texte et fonction de report d'erreur.
	*/
	template< PoolErrorType ErrorType >
	struct Error;
	/**
	\~english
	\brief		Unique pool instance.
	\param		Object		The pool objects type.
	\param		MemDataType	The allocation/deallocation policy type.
	\~french
	\brief		Instance unique de pool.
	\param		Object		Le type des objets du pool.
	\param		MemDataType	Le type de la politique d'allocation/désallocation.
	*/
	template< typename Object, MemoryDataType MemDataType >
	class UniqueObjectPool;
	/**
	\~english
	\brief		Unique pool instance.
	\param		Object		The pool objects type.
	\param		MemDataType	The allocation/deallocation policy type.
	\~french
	\brief		Instance unique de pool.
	\param		Object		Le type des objets du pool.
	\param		MemDataType	Le type de la politique d'allocation/désallocation.
	*/
	template< typename Object, MemoryDataType MemDataType, uint32_t Align >
	class UniqueAlignedObjectPool;
	/**
	\~english
	\brief		Pool exception.
	\~french
	\brief		Exception de pool.
	*/
	template< PoolErrorType ErrorType >
	class PoolMemoryException;

	template< typename Object, typename MemoryAllocator = NonAlignedMemoryAllocator >
	class FixedSizeMemoryData;
	template< typename Object, typename MemoryAllocator = NonAlignedMemoryAllocator >
	class FixedGrowingSizeMemoryData;
	template< typename Object >
	class FixedSizeMarkedMemoryData;
	template< typename Object >
	class FixedGrowingSizeMarkedMemoryData;
	//@}
}

#endif
