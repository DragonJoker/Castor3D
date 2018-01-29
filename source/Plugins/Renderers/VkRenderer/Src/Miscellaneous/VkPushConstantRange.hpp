/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___VkRenderer_PushConstantRange_HPP___
#define ___VkRenderer_PushConstantRange_HPP___
#pragma once

#include <Miscellaneous/PushConstantRange.hpp>

namespace vk_renderer
{
	/**
	*\brief
	*	Convertit un renderer::PushConstantRange en VkPushConstantRange.
	*\param[in] mode
	*	Le renderer::PushConstantRange.
	*\return
	*	Le VkPushConstantRange.
	*/
	VkPushConstantRange convert( renderer::PushConstantRange const & value );
}

#endif
