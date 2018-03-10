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
	*	Convertit un renderer::MultisampleStateFlags en VkPipelineMultisampleStateCreateFlags.
	*\param[in] flags
	*	Le renderer::MultisampleStateFlags.
	*\return
	*	Le VkPipelineMultisampleStateCreateFlags.
	*/
	VkPipelineMultisampleStateCreateFlags convert( renderer::MultisampleStateFlags const & flags );
}
