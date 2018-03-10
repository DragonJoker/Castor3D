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
	*	Convertit un renderer::ColourComponentFlag en VkColorComponentFlags.
	*\param[in] flags
	*	Le renderer::ColourComponentFlag.
	*\return
	*	Le VkColorComponentFlags.
	*/
	VkColorComponentFlags convert( renderer::ColourComponentFlags const & flags );
}
