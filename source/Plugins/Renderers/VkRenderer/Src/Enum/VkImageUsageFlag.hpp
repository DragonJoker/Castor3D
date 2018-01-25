/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#pragma once

#include <RendererPrerequisites.hpp>

namespace vk_renderer
{
	/**
	*\brief
	*	Convertit un renderer::ImageUsageFlags en VkImageUsageFlags.
	*\param[in] flags
	*	Le renderer::ImageUsageFlags.
	*\return
	*	Le VkImageUsageFlags.
	*/
	VkImageUsageFlags convert( renderer::ImageUsageFlags const & flags );
}
