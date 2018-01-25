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
	*	Convertit un renderer::WrapMode en VkSamplerAddressMode.
	*\param[in] mode
	*	Le renderer::WrapMode.
	*\return
	*	Le VkSamplerAddressMode.
	*/
	VkSamplerAddressMode convert( renderer::WrapMode const & mode );
}
