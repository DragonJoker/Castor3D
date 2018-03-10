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
	*	Convertit un renderer::DynamicState en VkDynamicState.
	*\param[in] value
	*	Le renderer::DynamicState.
	*\return
	*	Le VkDynamicState.
	*/
	VkDynamicState convert( renderer::DynamicState const & value );
}
