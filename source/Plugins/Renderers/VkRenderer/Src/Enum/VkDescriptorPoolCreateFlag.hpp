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
	*	Convertit un renderer::DescriptorPoolCreateFlags en VkDescriptorPoolCreateFlags.
	*\param[in] flags
	*	Le renderer::DescriptorPoolCreateFlags.
	*\return
	*	Le VkDescriptorPoolCreateFlags.
	*/
	VkDescriptorPoolCreateFlags convert( renderer::DescriptorPoolCreateFlags const & flags );
}
