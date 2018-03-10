/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_MemoryHeapFlag_HPP___
#define ___Renderer_MemoryHeapFlag_HPP___
#pragma once

namespace renderer
{
	/**
	*\brief
	*	Enumération des types de pile mémoire.
	*/
	enum class MemoryHeapFlag
		: uint32_t
	{
		eDeviceLocal = 0x00000001,
	};
	Utils_ImplementFlag( MemoryHeapFlag )
}

#endif
