/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___VkRenderer_MemoryRequirements_HPP___
#define ___VkRenderer_MemoryRequirements_HPP___
#pragma once

#include <Miscellaneous/MemoryRequirements.hpp>

namespace vk_renderer
{
	/**
	*\brief
	*	Convertit un renderer::MemoryRequirements en VkMemoryRequirements.
	*\param[in] mode
	*	Le renderer::MemoryRequirements.
	*\return
	*	Le VkMemoryRequirements.
	*/
	VkMemoryRequirements convert( renderer::MemoryRequirements const & value );
	/**
	*\brief
	*	Convertit un VkMemoryRequirements en renderer::MemoryRequirements.
	*\param[in] mode
	*	Le VkMemoryRequirements.
	*\return
	*	Le renderer::MemoryRequirements.
	*/
	renderer::MemoryRequirements convert( VkMemoryRequirements const & value );
}

#endif
