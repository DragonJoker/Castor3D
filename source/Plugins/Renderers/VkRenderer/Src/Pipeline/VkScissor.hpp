/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#pragma once

#include <Pipeline/Scissor.hpp>

namespace vk_renderer
{
	/**
	*\brief
	*	Convertit un renderer::Scissor en VkRect2D
	*\param[in] flags
	*	Le renderer::Scissor.
	*\return
	*	Le VkRect2D.
	*/
	VkRect2D convert( renderer::Scissor const & scissor );
}
