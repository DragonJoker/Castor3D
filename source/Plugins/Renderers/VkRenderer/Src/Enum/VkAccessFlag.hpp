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
	*	Convertit un renderer::AccessFlags en VkAccessFlags.
	*\param[in] flags
	*	Le renderer::AccessFlags.
	*\return
	*	Le VkAccessFlags.
	*/
	VkAccessFlags convert( renderer::AccessFlags const & flags );
	/**
	*\brief
	*	Convertit un VkAccessFlags en renderer::AccessFlags.
	*\param[in] flags
	*	Le VkAccessFlags.
	*\return
	*	Le renderer::AccessFlags.
	*/
	renderer::AccessFlags convertAccessFlags( VkAccessFlags const & flags );
}
