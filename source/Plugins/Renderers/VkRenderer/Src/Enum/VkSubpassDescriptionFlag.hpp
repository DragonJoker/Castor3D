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
	*	Convertit un renderer::SubpassDescriptionFlags en VkSubpassDescriptionFlags.
	*\param[in] flags
	*	Le renderer::SubpassDescriptionFlags.
	*\return
	*	Le VkSubpassDescriptionFlags.
	*/
	VkSubpassDescriptionFlags convert( renderer::SubpassDescriptionFlags const & flags );
	/**
	*\brief
	*	Convertit un VkSubpassDescriptionFlags en renderer::SubpassDescriptionFlags.
	*\param[in] flags
	*	Le VkSubpassDescriptionFlags.
	*\return
	*	Le renderer::SubpassDescriptionFlags.
	*/
	renderer::SubpassDescriptionFlags convertSubpassDescriptionFlags( VkSubpassDescriptionFlags const & flags );
}
