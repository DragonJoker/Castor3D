/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___VkRenderer_BorderColour_HPP___
#define ___VkRenderer_BorderColour_HPP___
#pragma once

#include <RendererPrerequisites.hpp>

namespace vk_renderer
{
	/**
	*\brief
	*	Convertit un renderer::BorderColour en VkBorderColor.
	*\param[in] value
	*	Le renderer::BorderColour.
	*\return
	*	Le VkBorderColor.
	*/
	VkBorderColor convert( renderer::BorderColour const & value )noexcept;
}

#endif
