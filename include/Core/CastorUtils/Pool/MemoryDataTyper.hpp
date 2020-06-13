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


	template< MemoryDataType Type >
	struct MemoryDataNamer
	{
		CU_API static char const * const Name;
	};
}

#endif
