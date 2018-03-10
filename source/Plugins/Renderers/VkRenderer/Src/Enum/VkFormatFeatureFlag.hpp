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
	*	Convertit un renderer::FormatFeatureFlags en VkFormatFeatureFlags.
	*\param[in] flags
	*	Le renderer::FormatFeatureFlags.
	*\return
	*	Le VkFormatFeatureFlags.
	*/
	VkFormatFeatureFlags convert( renderer::FormatFeatureFlags const & flags );
	/**
	*\brief
	*	Convertit un VkFormatFeatureFlags en renderer::FormatFeatureFlags.
	*\param[in] flags
	*	Le VkFormatFeatureFlags.
	*\return
	*	Le renderer::FormatFeatureFlags.
	*/
	renderer::FormatFeatureFlags convertFormatFeatureFlags( VkFormatFeatureFlags const & flags );
}
