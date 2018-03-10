/*
This file belongs to RendererLib.
See LICENSE file in root folder
*/
#pragma once

#include <Descriptor/WriteDescriptorSet.hpp>

namespace vk_renderer
{
	class DescriptorSet;
	/**
	*\brief
	*	Convertit un renderer::WriteDescriptorSet en VkWriteDescriptorSet.
	*\param[in] mode
	*	Le renderer::WriteDescriptorSet.
	*\return
	*	Le VkWriteDescriptorSet.
	*/
	VkWriteDescriptorSet convert( renderer::WriteDescriptorSet const & value
		, DescriptorSet const & descriptorSet
		, std::list< VkDescriptorImageInfo > & imageInfos
		, std::list< VkDescriptorBufferInfo > & bufferInfos
		, std::list< VkBufferView > & texelBufferViews );
}
