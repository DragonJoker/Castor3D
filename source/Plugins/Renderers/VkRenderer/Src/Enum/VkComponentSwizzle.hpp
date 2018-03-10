/*
This file belongs to RendererLib.
See LICENSE file in root folder
*/
#pragma once

#include <RendererPrerequisites.hpp>

namespace vk_renderer
{
	/**
	*\brief
	*	Convertit un renderer::ComponentSwizzle en VkComponentSwizzle.
	*\param[in] value
	*	Le renderer::ComponentSwizzle.
	*\return
	*	Le VkComponentSwizzle.
	*/
	VkComponentSwizzle convert( renderer::ComponentSwizzle const & value );
}
