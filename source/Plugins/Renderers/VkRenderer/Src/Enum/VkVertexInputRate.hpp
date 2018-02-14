/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#pragma once

#include <RendererPrerequisites.hpp>

namespace vk_renderer
{
	/**
	*\brief
	*	Convertit un renderer::VertexInputRate en VkVertexInputRate.
	*\param[in] value
	*	Le renderer::VertexInputRate.
	*\return
	*	Le VkVertexInputRate.
	*/
	VkVertexInputRate convert( renderer::VertexInputRate const & value );
}
