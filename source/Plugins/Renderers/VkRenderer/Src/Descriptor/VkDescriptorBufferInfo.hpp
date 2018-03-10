/*
This file belongs to RendererLib.
See LICENSE file in root folder
*/
#pragma once

#include <Descriptor/DescriptorBufferInfo.hpp>

namespace vk_renderer
{
	/**
	*\brief
	*	Convertit un renderer::DescriptorBufferInfo en VkDescriptorBufferInfo.
	*\param[in] mode
	*	Le renderer::DescriptorBufferInfo.
	*\return
	*	Le VkDescriptorBufferInfo.
	*/
	VkDescriptorBufferInfo convert( renderer::DescriptorBufferInfo const & value );
}
