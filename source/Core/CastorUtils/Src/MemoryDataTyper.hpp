/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___CU_MEMORY_DATA_TYPER_H___
#define ___CU_MEMORY_DATA_TYPER_H___

#include "PoolException.hpp"
#include "AlignedMemoryAllocator.hpp"
#include "NonAlignedMemoryAllocator.hpp"

namespace Castor
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
	template< typename Object, eMEMDATA_TYPE Type > struct MemoryDataTyper;

	//!\~english Specialisation for eMEMDATA_TYPE_FIXED.	\~french Spécialisation pour eMEMDATA_TYPE_FIXED.
	template< typename Object >
	struct MemoryDataTyper< Object, eMEMDATA_TYPE_FIXED >
	{
		using Type = FixedSizeMemoryData< Object >;
	};

	//!\~english Specialisation for eMEMDATA_TYPE_FIXED_MARKED.	\~french Spécialisation pour eMEMDATA_TYPE_FIXED_MARKED.
	template< typename Object >
	struct MemoryDataTyper< Object, eMEMDATA_TYPE_FIXED_MARKED >
	{
		using Type = FixedSizeMarkedMemoryData< Object >;
	};

	//!\~english Specialisation for eMEMDATA_TYPE_FIXED_GROWING.	\~french Spécialisation pour eMEMDATA_TYPE_FIXED_GROWING.
	template< typename Object >
	struct MemoryDataTyper< Object, eMEMDATA_TYPE_FIXED_GROWING >
	{
		using Type = FixedGrowingSizeMemoryData< Object >;
	};

	//!\~english Specialisation for eMEMDATA_TYPE_FIXED_GROWING_MARKED.	\~french Spécialisation pour eMEMDATA_TYPE_FIXED_GROWING_MARKED.
	template< typename Object >
	struct MemoryDataTyper< Object, eMEMDATA_TYPE_FIXED_GROWING_MARKED >
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
	template< typename Object, uint32_t Align, eMEMDATA_TYPE Type > struct AlignedMemoryDataTyper;

	//!\~english Specialisation for eMEMDATA_TYPE_FIXED.	\~french Spécialisation pour eMEMDATA_TYPE_FIXED.
	template< typename Object, uint32_t Align >
	struct AlignedMemoryDataTyper< Object, Align, eMEMDATA_TYPE_FIXED >
	{
		using Type = FixedSizeMemoryData< Object, AlignedMemoryAllocator< Align > >;
	};

	//!\~english Specialisation for eMEMDATA_TYPE_FIXED_GROWING.	\~french Spécialisation pour eMEMDATA_TYPE_FIXED_GROWING.
	template< typename Object, uint32_t Align >
	struct AlignedMemoryDataTyper< Object, Align, eMEMDATA_TYPE_FIXED_GROWING >
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
	template< eMEMDATA_TYPE Type > struct MemoryDataNamer
	{
		CU_API static char const * const Name;
	};
}

#endif
