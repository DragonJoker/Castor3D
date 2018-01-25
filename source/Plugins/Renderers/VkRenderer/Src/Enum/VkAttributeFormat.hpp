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
	*	Convertit un renderer::AttributeFormat en VkFormat.
	*\param[in] flags
	*	Le renderer::AttributeFormat.
	*\return
	*	Le VkFormat.
	*/
	VkFormat convert( renderer::AttributeFormat format );
}
