/*
This file belongs to RendererLib.
See LICENSE file in root folder
*/
#pragma once

#include <Image/ImageSubresourceRange.hpp>

namespace vk_renderer
{
	/**
	*\brief
	*	Convertit un renderer::ImageSubresourceRange en VkImageSubresourceRange.
	*\param[in] flags
	*	Le renderer::ImageSubresourceRange.
	*\return
	*	Le VkImageSubresourceRange.
	*/
	VkImageSubresourceRange convert( renderer::ImageSubresourceRange const & range );
	/**
	*\brief
	*	Convertit un VkImageSubresourceRange en renderer::ImageSubresourceRange.
	*\param[in] flags
	*	Le VkImageSubresourceRange.
	*\return
	*	Le renderer::ImageSubresourceRange.
	*/
	renderer::ImageSubresourceRange convert( VkImageSubresourceRange const & range );
}
