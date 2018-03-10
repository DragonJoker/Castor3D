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
	*	Convertit un renderer::QueueFlags en VkQueueFlags.
	*\param[in] flags
	*	Le renderer::QueueFlags.
	*\return
	*	Le VkQueueFlags.
	*/
	VkQueueFlags convert( renderer::QueueFlags const & flags );
	/**
	*\brief
	*	Convertit un VkQueueFlags en renderer::QueueFlags.
	*\param[in] flags
	*	Le VkQueueFlags.
	*\return
	*	Le renderer::QueueFlags.
	*/
	renderer::QueueFlags convertQueueFlags( VkQueueFlags const & flags );
}
