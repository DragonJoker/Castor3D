/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_MemoryHeap_HPP___
#define ___Renderer_MemoryHeap_HPP___
#pragma once

#include "RendererPrerequisites.hpp"

namespace renderer
{
	/**
	*\~english
	*\brief
	*	Specifies a memory heap.
	*\~french
	*\brief
	*	Spécifie un tas mémoire.
	*/
	struct MemoryHeap
	{
		uint64_t size;
		MemoryHeapFlags flags;
	};
}

#endif
