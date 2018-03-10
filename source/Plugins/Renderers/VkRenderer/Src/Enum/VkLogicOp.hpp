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
	*	Convertit un renderer::LogicOp en VkLogicOp.
	*\param[in] value
	*	Le renderer::LogicOp.
	*\return
	*	Le VkLogicOp.
	*/
	VkLogicOp convert( renderer::LogicOp const & value );
}
