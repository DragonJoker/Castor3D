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
	*	Convertit un renderer::DescriptorType en VkDescriptorType.
	*\param[in] type
	*	Le renderer::DescriptorType.
	*\return
	*	Le VkDescriptorType.
	*/
	VkDescriptorType convert( renderer::DescriptorType const & type );
}
