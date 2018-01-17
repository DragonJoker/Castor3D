/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_MemoryMapFlag_HPP___
#define ___Renderer_MemoryMapFlag_HPP___
#pragma once

#include <Design/FlagCombination.hpp>

namespace renderer
{
	/**
	*\~french
	*\brief
	*	Masques de bits décrivant les propriétés de mapping pour les tampons.
	*\~english
	*\brief
	*	Bitmask specifying memory mapping properties.
	*/
	enum class MemoryMapFlag
		: uint32_t
	{
		eRead = 0x00000001,
		eWrite = 0x00000002,
		ePersistent = 0x00000004,
		eCoherent = 0x00000008,
		eInvalidateRange = 0x00000010,
		eInvalidateBuffer = 0x00000020,
		eFlushExplicit = 0x00000040,
		eUnsynchronised = 0x00000080,
	};
	IMPLEMENT_FLAGS( MemoryMapFlag )
}

#endif
