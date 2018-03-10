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
	*	Convertit un renderer::CommandBufferUsageFlag en VkCommandBufferUsageFlags.
	*\param[in] flags
	*	Le renderer::CommandBufferUsageFlag.
	*\return
	*	Le VkCommandBufferUsageFlags.
	*/
	VkPipelineStageFlags convert( renderer::PipelineStageFlags const & flags );
	/**
	*\brief
	*	Convertit un renderer::PipelineStageFlag en VkPipelineStageFlagBits.
	*\param[in] flags
	*	Le renderer::PipelineStageFlag.
	*\return
	*	Le VkPipelineStageFlagBits.
	*/
	VkPipelineStageFlagBits convert( renderer::PipelineStageFlag const & flags );
}
