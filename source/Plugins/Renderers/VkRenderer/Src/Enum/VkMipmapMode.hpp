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
	*	Convertit un renderer::MipmapMode en VkSamplerMipmapMode.
	*\param[in] mode
	*	Le renderer::MipmapMode.
	*\return
	*	Le VkSamplerMipmapMode.
	*/
	VkSamplerMipmapMode convert( renderer::MipmapMode const & mode );
}
