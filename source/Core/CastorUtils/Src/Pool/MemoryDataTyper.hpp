/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___CU_MEMORY_DATA_TYPER_H___
#define ___CU_MEMORY_DATA_TYPER_H___

#include "PoolException.hpp"
#include "Align/AlignedMemoryAllocator.hpp"
#include "Align/NonAlignedMemoryAllocator.hpp"

namespace castor
{
	/*!
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

	/*!
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

	/*!
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
