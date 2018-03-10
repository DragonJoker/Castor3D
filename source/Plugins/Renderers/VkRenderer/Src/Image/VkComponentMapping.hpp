/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___VkRenderer_ComponentMapping_HPP___
#define ___VkRenderer_ComponentMapping_HPP___
#pragma once

#include <Image/ComponentMapping.hpp>

namespace vk_renderer
{
	/**
	*\brief
	*	Convertit un renderer::ComponentMapping en VkComponentMapping.
	*\param[in] value
	*	Le renderer::ComponentMapping.
	*\return
	*	Le VkImageSubresourceLayers.
	*/
	VkComponentMapping convert( renderer::ComponentMapping const & value );
}

#endif
