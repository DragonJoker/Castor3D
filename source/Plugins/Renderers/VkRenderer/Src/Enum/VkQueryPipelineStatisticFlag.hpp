/*
This file belongs to Renderer.
See LICENSE file in root folder
*/
#pragma once

#include <RendererPrerequisites.hpp>

namespace vk_renderer
{
	/**
	*\brief
	*	Convertit un renderer::QueryPipelineStatisticFlags en VkQueryPipelineStatisticFlags.
	*\param[in] flags
	*	Le renderer::QueryPipelineStatisticFlags.
	*\return
	*	Le VkQueryPipelineStatisticFlags.
	*/
	VkQueryPipelineStatisticFlags convert( renderer::QueryPipelineStatisticFlags const & flags );
}
