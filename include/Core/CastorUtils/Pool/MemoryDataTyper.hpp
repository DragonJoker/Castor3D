/*
See LICENSE file in root folder
*/
#ifndef ___CU_MEMORY_DATA_TYPER_H___
#define ___CU_MEMORY_DATA_TYPER_H___

#include "CastorUtils/Align/AlignedMemoryAllocator.hpp"
#include "CastorUtils/Align/NonAlignedMemoryAllocator.hpp"
#include "CastorUtils/Pool/PoolException.hpp"

namespace castor
{
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
	template< typename Object, MemoryDataType MemDataType >
	class PoolManagedObject;
	/**
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		08/01/2016
	\~english
	\brief		The object pool typer.
	\~french
	\brief		Le typeur de pools d'objets.
	*/
	template< typename Object, MemoryDataType Type > struct MemoryDataTyper;

	//!\~english Specialisation for MemoryDataType::eFixed.	\~french Spécialisation pour MemoryDataType::eFixed.
	template< typename Object >
	struct MemoryDataTyper< Object, MemoryDataType::eFixed >
	{
		using Type = FixedSizeMemoryData< Object >;
	};

	//!\~english Specialisation for MemoryDataType::eMarked.	\~french Spécialisation pour MemoryDataType::eMarked.
	template< typename Object >
	struct MemoryDataTyper< Object, MemoryDataType::eMarked >
	{
		using Type = FixedSizeMarkedMemoryData< Object >;
	};

	//!\~english Specialisation for MemoryDataType::eFixedGrowing.	\~french Spécialisation pour MemoryDataType::eFixedGrowing.
	template< typename Object >
	struct MemoryDataTyper< Object, MemoryDataType::eFixedGrowing >
	{
		using Type = FixedGrowingSizeMemoryData< Object >;
	};

	//!\~english Specialisation for MemoryDataType::eFixedGrowingMarked.	\~french Spécialisation pour MemoryDataType::eFixedGrowingMarked.
	template< typename Object >
	struct MemoryDataTyper< Object, MemoryDataType::eFixedGrowingMarked >
	{
		using Type = FixedGrowingSizeMarkedMemoryData< Object >;
	};

	/**
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		08/01/2016
	\~english
	\brief		The aligned object pool typer.
	\~french
	\brief		Le typeur de pools d'objets alignés.
	*/
	template< typename Object, uint32_t Align, MemoryDataType Type > struct AlignedMemoryDataTyper;

	//!\~english Specialisation for MemoryDataType::eFixed.	\~french Spécialisation pour MemoryDataType::eFixed.
	template< typename Object, uint32_t Align >
	struct AlignedMemoryDataTyper< Object, Align, MemoryDataType::eFixed >
	{
		using Type = FixedSizeMemoryData< Object, AlignedMemoryAllocator< Align > >;
	};

	//!\~english Specialisation for MemoryDataType::eFixedGrowing.	\~french Spécialisation pour MemoryDataType::eFixedGrowing.
	template< typename Object, uint32_t Align >
	struct AlignedMemoryDataTyper< Object, Align, MemoryDataType::eFixedGrowing >
	{
		using Type = FixedGrowingSizeMemoryData< Object, AlignedMemoryAllocator< Align > >;
	};

	/**
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		08/01/2016
	\~english
	\brief		The MemoryData namer.
	\~french
	\brief		Le nommeur de MemoryData.
	*/
	template< MemoryDataType Type > struct MemoryDataNamer
	{
		CU_API static char const * const Name;
	};
}

#endif
