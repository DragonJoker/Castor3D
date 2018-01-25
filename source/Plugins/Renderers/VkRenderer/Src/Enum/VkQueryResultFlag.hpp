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
	*	Convertit un renderer::QueryResultFlags en VkQueryResultFlags.
	*\param[in] flags
	*	Le renderer::QueryResultFlags.
	*\return
	*	Le VkQueryResultFlags.
	*/
	VkQueryResultFlags convert( renderer::QueryResultFlags const & flags );
}
