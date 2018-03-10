/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___VkRenderer_ClearRect_HPP___
#define ___VkRenderer_ClearRect_HPP___
#pragma once

#include <RenderPass/ClearRect.hpp>

namespace vk_renderer
{
	/**
	*\brief
	*	Convertit un renderer::ClearRect en VkClearRect.
	*\param[in] mode
	*	Le renderer::ClearRect.
	*\return
	*	Le VkClearRect.
	*/
	VkClearRect convert( renderer::ClearRect const & value );
}

#endif
