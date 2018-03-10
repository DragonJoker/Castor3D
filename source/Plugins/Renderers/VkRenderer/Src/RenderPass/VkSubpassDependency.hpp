/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___VkRenderer_SubpassDependency_HPP___
#define ___VkRenderer_SubpassDependency_HPP___
#pragma once

#include <RenderPass/SubpassDependency.hpp>

namespace vk_renderer
{
	/**
	*\brief
	*	Convertit un renderer::SubpassDependency en VkSubpassDependency.
	*\param[in] mode
	*	Le renderer::SubpassDependency.
	*\return
	*	Le VkSubpassDependency.
	*/
	VkSubpassDependency convert( renderer::SubpassDependency const & value );
}

#endif
