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
	*	Convertit un renderer::PipelineBindPoint en VkPipelineBindPoint.
	*\param[in] point
	*	Le renderer::PipelineBindPoint.
	*\return
	*	Le VkPipelineBindPoint.
	*/
	VkPipelineBindPoint convert( renderer::PipelineBindPoint const & point );
}
