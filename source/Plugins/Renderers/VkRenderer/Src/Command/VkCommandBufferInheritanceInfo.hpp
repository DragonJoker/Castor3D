/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___VkRenderer_CommandBufferInheritanceInfo_HPP___
#define ___VkRenderer_CommandBufferInheritanceInfo_HPP___
#pragma once

#include <Command/CommandBufferInheritanceInfo.hpp>

namespace vk_renderer
{
	/**
	*\brief
	*	Convertit un renderer::CommandBufferInheritanceInfo en VkCommandBufferInheritanceInfo.
	*\param[in] mode
	*	Le renderer::CommandBufferInheritanceInfo.
	*\return
	*	Le VkCommandBufferInheritanceInfo.
	*/
	VkCommandBufferInheritanceInfo convert( renderer::CommandBufferInheritanceInfo const & value );
}

#endif
