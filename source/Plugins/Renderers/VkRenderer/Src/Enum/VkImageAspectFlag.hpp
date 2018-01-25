/*
This file belongs to Renderer.
See LICENSE file in root folder
*/
#pragma once

#include <RendererPrerequisites.hpp>

namespace vk_renderer
{
	/**
	*\brief
	*	Convertit un renderer::ImageAspectFlags en VkImageAspectFlags.
	*\param[in] flags
	*	Le renderer::ImageAspectFlags.
	*\return
	*	Le VkImageAspectFlags.
	*/
	VkImageAspectFlags convert( renderer::ImageAspectFlags const & flags );
	/**
	*\brief
	*	Convertit un VkImageAspectFlags en renderer::ImageAspectFlags.
	*\param[in] flags
	*	Le VkImageAspectFlags.
	*\return
	*	Le renderer::ImageAspectFlags.
	*/
	renderer::ImageAspectFlags convertAspectMask( VkImageAspectFlags const & flags );
}
