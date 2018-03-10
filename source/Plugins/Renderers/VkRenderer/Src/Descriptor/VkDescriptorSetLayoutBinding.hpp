/*
This file belongs to RendererLib.
See LICENSE file in root folder
*/
#ifndef ___VkRenderer_DescriptorSetLayoutBinding_HPP___
#define ___VkRenderer_DescriptorSetLayoutBinding_HPP___
#pragma once

#include <Descriptor/DescriptorSetLayoutBinding.hpp>

namespace vk_renderer
{
	/**
	*\brief
	*	Convertit un renderer::DescriptorSetLayoutBinding en VkDescriptorSetLayoutBinding.
	*\param[in] flags
	*	Le renderer::DescriptorSetLayoutBinding.
	*\return
	*	Le VkDescriptorSetLayoutBinding.
	*/
	VkDescriptorSetLayoutBinding convert( renderer::DescriptorSetLayoutBinding const & binding );
}

#endif
