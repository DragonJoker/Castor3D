/*
This file belongs to RendererLib.
See LICENSE file in root folder
*/
#pragma once

#include <RendererPrerequisites.hpp>

namespace vk_renderer
{
	/**
	*\brief
	*	Convertit un renderer::MemoryHeapFlags en VkMemoryHeapFlags.
	*\param[in] flags
	*	Le renderer::MemoryHeapFlags.
	*\return
	*	Le VkMemoryHeapFlags.
	*/
	VkMemoryHeapFlags convert( renderer::MemoryHeapFlags const & flags );
	/**
	*\brief
	*	Convertit un VkMemoryHeapFlags en renderer::MemoryHeapFlags.
	*\param[in] flags
	*	Le VkMemoryHeapFlags.
	*\return
	*	Le renderer::MemoryHeapFlags.
	*/
	renderer::MemoryHeapFlags convertMemoryHeapFlags( VkMemoryHeapFlags const & flags );
}
