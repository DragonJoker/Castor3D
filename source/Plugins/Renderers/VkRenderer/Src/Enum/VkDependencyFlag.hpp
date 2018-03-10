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
	*	Convertit un renderer::DependencyFlags en VkDependencyFlags.
	*\param[in] flags
	*	Le renderer::DependencyFlags.
	*\return
	*	Le VkDependencyFlags.
	*/
	VkDependencyFlags convert( renderer::DependencyFlags const & flags );
	/**
	*\brief
	*	Convertit un VkAccessFlags en renderer::DependencyFlags.
	*\param[in] flags
	*	Le VkDependencyFlags.
	*\return
	*	Le renderer::DependencyFlags.
	*/
	renderer::DependencyFlags convertDependencyFlags( VkDependencyFlags const & flags );
}
