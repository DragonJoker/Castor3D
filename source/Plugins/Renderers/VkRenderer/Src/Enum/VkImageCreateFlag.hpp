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
	*	Convertit un renderer::ImageCreateFlags en VkImageCreateFlags.
	*\param[in] flags
	*	Le renderer::ImageCreateFlags.
	*\return
	*	Le VkImageCreateFlags.
	*/
	VkImageCreateFlags convert( renderer::ImageCreateFlags const & flags );
	/**
	*\brief
	*	Convertit un VkAccessFlags en renderer::AccessFlags.
	*\param[in] flags
	*	Le VkAccessFlags.
	*\return
	*	Le renderer::AccessFlags.
	*/
	renderer::ImageCreateFlags convertImageCreateFlags( VkImageCreateFlags const & flags );
}
