/**
*\file
*	VertexBuffer.h
*\author
*	Sylvain Doremus
*/
#ifndef ___VkRenderer_VertexInputState_HPP___
#define ___VkRenderer_VertexInputState_HPP___
#pragma once

#include <Pipeline/VertexInputState.hpp>

namespace vk_renderer
{
	/**
	*\brief
	*	Convertit un renderer::VertexInputState en VkPipelineVertexInputStateCreateInfo.
	*\param[in] state
	*	Le renderer::VertexInputState.
	*\return
	*	Le VkPipelineVertexInputStateCreateInfo.
	*/
	VkPipelineVertexInputStateCreateInfo convert( renderer::VertexInputState const & state
		, std::vector< VkVertexInputAttributeDescription > const & attributes
		, std::vector< VkVertexInputBindingDescription > const & bindings );
}

#endif
