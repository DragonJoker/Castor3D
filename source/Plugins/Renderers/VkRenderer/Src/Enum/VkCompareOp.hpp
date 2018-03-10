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
	*	Convertit un renderer::CompareOp en VkCompareOp.
	*\param[in] value
	*	Le renderer::CompareOp.
	*\return
	*	Le VkCompareOp.
	*/
	VkCompareOp convert( renderer::CompareOp const & value );
}
