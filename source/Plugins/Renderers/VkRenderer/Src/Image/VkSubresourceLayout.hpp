/*
This file belongs to RendererLib.
See LICENSE file in root folder
*/
#pragma once

#include <Image/SubresourceLayout.hpp>

namespace vk_renderer
{
	/**
	*\brief
	*	Convertit un renderer::SubresourceLayout en VkImageSubresourceRange.
	*\param[in] flags
	*	Le renderer::SubresourceLayout.
	*\return
	*	Le VkSubresourceLayout.
	*/
	VkSubresourceLayout convert( renderer::SubresourceLayout const & range );
	/**
	*\brief
	*	Convertit un VkSubresourceLayout en renderer::SubresourceLayout.
	*\param[in] flags
	*	Le VkSubresourceLayout.
	*\return
	*	Le renderer::SubresourceLayout.
	*/
	renderer::SubresourceLayout convert( VkSubresourceLayout const & range );
}
