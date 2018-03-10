/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___VkRenderer_ClearAttachment_HPP___
#define ___VkRenderer_ClearAttachment_HPP___
#pragma once

#include <RenderPass/ClearAttachment.hpp>

namespace vk_renderer
{
	/**
	*\brief
	*	Convertit un renderer::ClearAttachment en VkClearAttachment.
	*\param[in] mode
	*	Le renderer::ClearAttachment.
	*\return
	*	Le VkClearAttachment.
	*/
	VkClearAttachment convert( renderer::ClearAttachment const & value );
}

#endif
