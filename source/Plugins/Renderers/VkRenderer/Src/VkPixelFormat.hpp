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
	*	Convertit un renderer::PixelFormat en VkFormat.
	*\param[in] format
	*	Le renderer::PixelFormat.
	*\return
	*	Le VkFormat.
	*/
	VkFormat convert( renderer::PixelFormat const & format )noexcept;
	/**
	*\brief
	*	Convertit un VkFormat en renderer::PixelFormat.
	*\param[in] format
	*	Le VkFormat.
	*\return
	*	Le renderer::PixelFormat.
	*/
	renderer::PixelFormat convert( VkFormat const & format )noexcept;
}
