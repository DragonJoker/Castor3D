/*
This file belongs to RendererLib.
See LICENSE file in root folder
*/
#pragma once

#include <Descriptor/DescriptorImageInfo.hpp>

namespace vk_renderer
{
	/**
	*\brief
	*	Convertit un renderer::DescriptorImageInfo en VkDescriptorImageInfo.
	*\param[in] mode
	*	Le renderer::DescriptorImageInfo.
	*\return
	*	Le VkDescriptorImageInfo.
	*/
	VkDescriptorImageInfo convert( renderer::DescriptorImageInfo const & value );
}
