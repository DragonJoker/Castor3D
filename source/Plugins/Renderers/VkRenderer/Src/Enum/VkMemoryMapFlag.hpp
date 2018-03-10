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
	*	Convertit un renderer::MemoryMapFlags en VkMemoryMapFlags.
	*\param[in] format
	*	Le renderer::MemoryMapFlags.
	*\return
	*	Le VkMemoryMapFlags.
	*/
	VkMemoryMapFlags convert( renderer::MemoryMapFlags const & flags );
}
