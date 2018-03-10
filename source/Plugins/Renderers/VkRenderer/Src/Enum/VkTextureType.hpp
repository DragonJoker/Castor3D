/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___VkRenderer_TextureType_HPP___
#define ___VkRenderer_TextureType_HPP___
#pragma once

#include <RendererPrerequisites.hpp>

namespace vk_renderer
{
	/**
	*\brief
	*	Convertit un renderer::TextureType en VkImageViewType.
	*\param[in] value
	*	Le renderer::TextureType.
	*\return
	*	Le VkImageViewType.
	*/
	VkImageType convert( renderer::TextureType const & value )noexcept;
}

#endif
