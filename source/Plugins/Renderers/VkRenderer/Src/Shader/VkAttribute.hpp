/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#pragma once

#include "VkRendererPrerequisites.hpp"

#include <Shader/Attribute.hpp>

namespace vk_renderer
{
	/**
	*\brief
	*	Convertit un renderer::Attribute en VkVertexInputAttributeDescription.
	*\param[in] value
	*	Le renderer::Attribute.
	*\return
	*	Le VkVertexInputAttributeDescription.
	*/
	VkVertexInputAttributeDescription convert( renderer::Attribute const & value );
}
