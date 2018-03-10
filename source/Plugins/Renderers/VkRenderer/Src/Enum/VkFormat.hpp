/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#pragma once

#include <RendererPrerequisites.hpp>

namespace vk_renderer
{
	/**
	*\brief
	*	Convertit un renderer::Format en VkFormat.
	*\param[in] format
	*	Le renderer::Format.
	*\return
	*	Le VkFormat.
	*/
	VkFormat convert( renderer::Format const & format )noexcept;
	/**
	*\brief
	*	Convertit un VkFormat en renderer::Format.
	*\param[in] format
	*	Le VkFormat.
	*\return
	*	Le renderer::Format.
	*/
	renderer::Format convert( VkFormat const & format )noexcept;
}
