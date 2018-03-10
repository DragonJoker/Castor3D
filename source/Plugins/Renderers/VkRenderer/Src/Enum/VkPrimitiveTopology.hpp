/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#pragma once

#include <RendererPrerequisites.hpp>

namespace vk_renderer
{
	/**
	*\brief
	*	Convertit un renderer::PrimitiveTopology en VkPrimitiveTopology.
	*\param[in] format
	*	Le renderer::PrimitiveTopology.
	*\return
	*	Le VkPrimitiveTopology.
	*/
	VkPrimitiveTopology convert( renderer::PrimitiveTopology const & topology );
}
