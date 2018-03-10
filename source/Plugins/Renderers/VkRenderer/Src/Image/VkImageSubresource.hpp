/*
This file belongs to RendererLib.
See LICENSE file in root folder
*/
#pragma once

#include <Image/ImageSubresource.hpp>

namespace vk_renderer
{
	/**
	*\brief
	*	Convertit un renderer::ImageSubresource en VkImageSubresource.
	*\param[in] flags
	*	Le renderer::ImageSubresource.
	*\return
	*	Le VkImageSubresource.
	*/
	VkImageSubresource convert( renderer::ImageSubresource const & range );
	/**
	*\brief
	*	Convertit un VkImageSubresource en renderer::ImageSubresource.
	*\param[in] flags
	*	Le VkImageSubresource.
	*\return
	*	Le renderer::ImageSubresource.
	*/
	renderer::ImageSubresource convert( VkImageSubresource const & range );
}
